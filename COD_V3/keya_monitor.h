
#ifndef KEYA_MONITOR_H
#define KEYA_MONITOR_H

#include <Arduino.h>
#include "system.h"


// fault-flag bit positions returned by ?FF
// the driver returns a decimal integer; each bit maps to one fault type.

#define KEYA_FF_OVERHEAT    (1UL << 0)
#define KEYA_FF_OVERVOLTAGE (1UL << 1)
#define KEYA_FF_UNDERVOLTAGE (1UL << 2)
#define KEYA_FF_SHORT_CIRCUIT (1UL << 3)
#define KEYA_FF_ESTOP       (1UL << 4)
#define KEYA_FF_MOSFET      (1UL << 6)
#define KEYA_FF_DEFAULT_CFG (1UL << 7)

//timing

#define KEYA_POLL_INTERVAL_MS  500u   // full poll cycle period
#define KEYA_QUERY_TIMEOUT_MS   30u   // max wait for one response


// per-driver telemetry snapshot.

struct KeyaDriverStatus {
    // per-channel fields: index 0 = channel 1, index 1 = channel 2
    int32_t speed_rpm[2];
    float   amps[2];
    float   heatsink_temp_c[2];  // ?T t1/t2 — per-channel heatsink temperature

    // driver-level fields
    float    temp_c;       // ?T tm — internal IC temperature
    float    battery_v;    // ?V vmot — main battery voltage
    float    driver_v;     // ?V vdr — internal driver-stage voltage
    uint32_t faults;      // bitmask, use KEYA_FF_* macros

    // communication health
    bool     comm_ok;
    uint32_t last_update_ms;
};

KeyaDriverStatus tractionStatus;  // KYDBL on Serial1 (UART1, GPIO18/19)
KeyaDriverStatus brushStatus;     // KYDBL on Serial  (UART0)


// _keyaQuery — send one query and collect the response.
// sends "?<cmd>\r" and reads until '\r' (the driver's response terminator,
// also accepts '\n' defensively) or timeout.
// returns true if at least one character was received.

static bool _keyaQuery(HardwareSerial &port, const char *cmd,
                        char *buf, size_t bufLen)
{
    while (port.available()) port.read();   // flush stale bytes

    port.print('?');
    port.print(cmd);
    port.print('\r');

    uint32_t deadline = millis() + KEYA_QUERY_TIMEOUT_MS;
    size_t   n        = 0;

    while (millis() < deadline && n < bufLen - 1) {
        if (port.available()) {
            char c = (char)port.read();
            if (c == '\r' || c == '\n') break;
            buf[n++] = c;
        } else {
            vTaskDelay(1);
        }
    }

    buf[n] = '\0';
    return n > 0;
}


// _keyaParse — extract signed integers from "KEY=V1:V2:V3" responses.
// returns the number of values successfully parsed.
static int _keyaParse(const char *buf, int32_t *vals, int maxVals)
{
    const char *p = strchr(buf, '=');
    if (!p) return 0;
    p++;

    int n = 0;
    while (n < maxVals) {
        char *end;
        vals[n] = strtol(p, &end, 10);
        if (end == p) break;
        n++;
        if (*end != ':') break;
        p = end + 1;
    }
    return n;
}


// _keyaPoll — run one full telemetry cycle for a single driver.
static void _keyaPoll(HardwareSerial &port, KeyaDriverStatus &drv,
                      SemaphoreHandle_t mtx)
{
    char    buf[64];
    int32_t v[3];
    bool    ok = true;

    // fault flags
    if (xSemaphoreTake(mtx, pdMS_TO_TICKS(50)) == pdTRUE) {
        bool q = _keyaQuery(port, "FF", buf, sizeof(buf));
        xSemaphoreGive(mtx);
        if (q && _keyaParse(buf, v, 1) >= 1) drv.faults = (uint32_t)v[0];
        else ok = false;
    } else { ok = false; }

    // motor current
    if (xSemaphoreTake(mtx, pdMS_TO_TICKS(50)) == pdTRUE) {
        bool q = _keyaQuery(port, "A", buf, sizeof(buf));
        xSemaphoreGive(mtx);
        if (q && _keyaParse(buf, v, 2) >= 2) {
            drv.amps[0] = v[0] / 10.0f;
            drv.amps[1] = v[1] / 10.0f;
        } else { ok = false; }
    } else { ok = false; }

    // shaft speed
    if (xSemaphoreTake(mtx, pdMS_TO_TICKS(50)) == pdTRUE) {
        bool q = _keyaQuery(port, "BS", buf, sizeof(buf));
        xSemaphoreGive(mtx);
        if (q && _keyaParse(buf, v, 2) >= 2) {
            drv.speed_rpm[0] = v[0];
            drv.speed_rpm[1] = v[1];
        } else { ok = false; }
    } else { ok = false; }

    // temperature
    if (xSemaphoreTake(mtx, pdMS_TO_TICKS(50)) == pdTRUE) {
        bool q = _keyaQuery(port, "T", buf, sizeof(buf));
        xSemaphoreGive(mtx);
        if (q && _keyaParse(buf, v, 3) >= 3) {
            drv.temp_c             = (float)v[0];
            drv.heatsink_temp_c[0] = (float)v[1];
            drv.heatsink_temp_c[1] = (float)v[2];
        } else { ok = false; }
    } else { ok = false; }

    // voltage
    if (xSemaphoreTake(mtx, pdMS_TO_TICKS(50)) == pdTRUE) {
        bool q = _keyaQuery(port, "V", buf, sizeof(buf));
        xSemaphoreGive(mtx);
        if (q && _keyaParse(buf, v, 3) >= 2) {
            drv.driver_v  = v[0] / 10.0f;
            drv.battery_v = v[1] / 10.0f;
        } else { ok = false; }
    } else { ok = false; }

    drv.comm_ok        = ok;
    drv.last_update_ms = millis();
}


// KeyaMonitorTask — FreeRTOS task: polls both drivers every KEYA_POLL_INTERVAL_MS.
// The main control task reads these structs to determine system health and drive the status LED.

void KeyaMonitorTask(void * /*arg*/)
{
    for (;;) {
        _keyaPoll(Serial1, tractionStatus, tractMutex);
        _keyaPoll(Serial,  brushStatus,    brushMutex);
        vTaskDelay(pdMS_TO_TICKS(KEYA_POLL_INTERVAL_MS));
    }
}

// keyaMonitorInit — create the port mutexes and launch the monitor task.
// call once from setup(), after the serial ports have been opened.
void keyaMonitorInit(void)
{
    memset(&tractionStatus, 0, sizeof(tractionStatus));
    memset(&brushStatus,    0, sizeof(brushStatus));

    tractMutex = xSemaphoreCreateMutex();
    brushMutex = xSemaphoreCreateMutex();

    xTaskCreatePinnedToCore(KeyaMonitorTask, "KeyaMonitor",
                            4096, NULL, 0, NULL, 0);
}

#endif

#ifndef ESCOBILLAS_KEYA_H
#define ESCOBILLAS_KEYA_H

#include <Arduino.h>
#include "system.h"

#define escobillasKeyaSerialPort Serial
#define escobillasKeya_BAUDRATE  115200

void escobillasKeyaInit(void)
{
    escobillasKeyaSerialPort.begin(escobillasKeya_BAUDRATE, SERIAL_8N1);
}

// BRUSH_FRONT/REAR_ONOFF: 0 or 1
// BRUSH_FRONT/REAR_LEVEL: 1–3  → output 0–1000
void velocidadEscobillasKeya(int16_t BRUSH_FRONT_ONOFF, int16_t BRUSH_FRONT_LEVEL,
                              int16_t BRUSH_REAR_ONOFF,  int16_t BRUSH_REAR_LEVEL)
{
    int16_t resultfront = BRUSH_FRONT_LEVEL * BRUSH_FRONT_ONOFF * 234;
    int16_t resultrear  = BRUSH_REAR_LEVEL  * BRUSH_REAR_ONOFF  * 234;

    if (brushMutex && xSemaphoreTake(brushMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        escobillasKeyaSerialPort.printf("!M %i %i\r", resultfront, resultrear);
        xSemaphoreGive(brushMutex);
    }
}

void StopBrushesKeya(void)
{
    if (brushMutex && xSemaphoreTake(brushMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        escobillasKeyaSerialPort.printf("!M 0 0\r");
        xSemaphoreGive(brushMutex);
    }
}

#endif

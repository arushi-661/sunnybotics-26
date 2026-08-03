#ifndef TRACCION_KEYA_H
#define TRACCION_KEYA_H

#include <Arduino.h>
#include "system.h"

#define TractionKeyaSerialPort Serial1
#define TractionKeya_BAUDRATE  115200
#define TractionKeya_RX        19
#define TractionKeya_TX        18

void tractionKeyaInit(void)
{
    TractionKeyaSerialPort.begin(TractionKeya_BAUDRATE, SERIAL_8N1,
                                  TractionKeya_RX, TractionKeya_TX);
}

void CinematicaDirectaKeya(float velocidad_lineal, float velocidad_angular,
                            int16_t ajuste_velocidad)
{
    int16_t RPM_derecha   = 0;
    int16_t RPM_izquierda = 0;

    RPM_derecha   = -(velocidad_lineal - (velocidad_angular / 2));
    RPM_izquierda =   velocidad_lineal + (velocidad_angular / 2);

    RPM_derecha   = RPM_derecha   * 11 / 30;
    RPM_izquierda = RPM_izquierda * 11 / 30;

    // dead-band: ignore small commands that don't overcome static friction
    if (RPM_derecha   < 100 && RPM_derecha   >= 0)  RPM_derecha   = 0;
    if (RPM_derecha   > -100 && RPM_derecha  <  0)  RPM_derecha   = 0;
    if (RPM_izquierda < 100 && RPM_izquierda >= 0)  RPM_izquierda = 0;
    if (RPM_izquierda > -100 && RPM_izquierda < 0)  RPM_izquierda = 0;

    if (tractMutex && xSemaphoreTake(tractMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        TractionKeyaSerialPort.printf("!M %i %i\r", RPM_derecha, RPM_izquierda);
        xSemaphoreGive(tractMutex);
    }
}

void StopMotorsKeya(void)
{
    if (tractMutex && xSemaphoreTake(tractMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        TractionKeyaSerialPort.printf("!M 0 0\r");
        xSemaphoreGive(tractMutex);
    }
}

#endif

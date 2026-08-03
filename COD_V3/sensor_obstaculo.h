// Diseñado por Spuentes.
#ifndef SENSOR_OBSTACULO_H
#define SENSOR_OBSTACULO_H

#include <Arduino.h>

// Pines de entrada desde la RPi
// HIGH = obstáculo detectado por ese lado
// LOW  = camino libre por ese lado
#define PIN_OBSTACULO_IZQ  26
#define PIN_OBSTACULO_DER  27

void sensorObstaculoInit(void)
{
    // Pull-down interno: si la RPi se desconecta o el cable se afloja,
    // el ESP32 lee LOW (sin obstáculo) y el robot puede seguir avanzando.
    // Esto es intencional: la pérdida del sensor NO es failsafe absoluto,
    // ese rol lo cumple la pérdida del SBUS.
    pinMode(PIN_OBSTACULO_IZQ, INPUT_PULLDOWN);
    pinMode(PIN_OBSTACULO_DER, INPUT_PULLDOWN);
}

void sensorObstaculoUpdate(void)
{
    // No-op: la lectura digital es instantánea y se hace dentro de las
    // funciones consultoras. Esta función existe solo para mantener la
    // interfaz que llama main.cpp.
}

bool obstaculoIzquierda(void)
{
    return (digitalRead(PIN_OBSTACULO_IZQ) == HIGH);
}

bool obstaculoDerecha(void)
{
    return (digitalRead(PIN_OBSTACULO_DER) == HIGH);
}

bool obstaculoCualquierLado(void)
{
    return obstaculoIzquierda() || obstaculoDerecha();
}

#endif // SENSOR_OBSTACULO_H

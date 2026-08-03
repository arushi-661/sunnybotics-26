#include <Arduino.h>
#include "system.h"
#include "controlremotoAT9S.h"
#include "traccion_Keya.h"
#include "escobillas_Keya.h"
#include "multitask.h"
#include "ws2812.h"
#include "sensor_obstaculo.h"
#include "autonomo.h"
#include "keya_monitor.h"

#define CHANNEL_AUTO_MODE CHANNEL_A_VALVE_ONOFF

void sistemstop(void)
{
    StopMotorsKeya();
    StopBrushesKeya();
}

void setup()
{
    pinMode(RL1, OUTPUT);
    pinMode(RL2, OUTPUT);
    digitalWrite(RL1, LOW);
    digitalWrite(RL2, LOW);

    tractionKeyaInit();
    escobillasKeyaInit();
    controlremotoInit();
    sensorObstaculoInit();
    keyaMonitorInit(); 
    MultitaskInit();
}

void ModbusTransmission(void *arg)
{
    for (;;) {
        leerControlRemoto();
        sensorObstaculoUpdate();

        if (ControlRemoto1.Ctrl_failSafe != CONTROL_CONECTADO ||
            ControlRemoto1.Ctrl_lostFrame != CONTROL_CONECTADO)
        {
            sistemstop();
            resetModoAutonomo();
            status = STATUS_LED_WARNING;
        }
        else
        {
            digitalWrite(RL2, ControlRemoto1.Ctrl_Valve == 1 ? HIGH : LOW);

            if (ControlRemoto1.Ctrl_ResetSignal == 1)
            {
                resetModoAutonomo();
                sistemstop();
                ControlRemoto1.Ctrl_FrontRear = 0;
                ControlRemoto1.Ctrl_RightLeft = 0;
                status = STATUS_LED_ERROR;
            }
            else
            {
                digitalWrite(RL2, ControlRemoto1.Ctrl_Valve == 1 ? HIGH : LOW);

                bool autoEnable = (onOff(channels[CHANNEL_AUTO_MODE], 300, 1600, 0) == 1);
                int16_t vel_lineal = ControlRemoto1.Ctrl_FrontRear;
                int16_t vel_angular = ControlRemoto1.Ctrl_RightLeft;

                ejecutarModoAutonomo(autoEnable, vel_lineal, vel_angular);

                CinematicaDirectaKeya(vel_lineal, vel_angular, ControlRemoto1.Ctrl_Valve);
                velocidadEscobillasKeya(ControlRemoto1.Ctrl_BrushFrontLevelOnOff, ControlRemoto1.Ctrl_BrushFrontLevel, ControlRemoto1.Ctrl_BrushRearLevelOnOff, ControlRemoto1.Ctrl_BrushRearLevel);

                bool driverFault = tractionStatus.faults || brushStatus.faults ||
                                   !tractionStatus.comm_ok || !brushStatus.comm_ok;
                status = driverFault ? STATUS_LED_ERROR : STATUS_LED_OK;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(2));
        digitalWrite(prueba, !digitalRead(prueba));
    }
}

void indicaciones(void *arg)
{
    for (;;) {
        statusled(status);
    }
}

void loop()
{
    vTaskDelay(pdMS_TO_TICKS(1000));
}

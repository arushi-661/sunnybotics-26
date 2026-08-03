// Diseñado por Spuentes.
#ifndef AUTONOMO_H
#define AUTONOMO_H

#include <Arduino.h>
#include "sensor_obstaculo.h"

// --- Parámetros de velocidad (rango interno -2000..+2000) ---
// NOTA: el firmware mapea joystick adelante a Ctrl_FrontRear NEGATIVO
// (ver LimitarEscalar en controlremotoAT9S.h, línea 51). Por eso
// AUTO_VEL_LINEAL es negativo para avanzar y AUTO_VEL_RETROCESO es
// positivo para retroceder.
#define AUTO_VEL_LINEAL        (-360)   // 18% adelante - mínimo seguro sobre zona muerta Keya
#define AUTO_VEL_RETROCESO      600     // 30% atrás
#define AUTO_VEL_ANGULAR_GIRO  1000     // 50% angular durante giro

// --- Parámetros de tiempo ---
#define AUTO_T_RETROCESO_MIN_MS  500UL  // Mínimo retroceso antes de revalidar sensores
#define AUTO_T_GIRO_MS           800UL  // Duración fija del giro

// --- Default cuando ambos sensores están activos a la vez ---
// +1 = girar a la izquierda, -1 = girar a la derecha.
#define AUTO_GIRO_DEFAULT_AMBOS  (+1)

// ---------------------------------------------------------------------------

enum EstadoAutonomo {
    AUTO_OFF           = 0,
    AUTO_AVANZANDO     = 1,
    AUTO_RETROCEDIENDO = 2,
    AUTO_GIRANDO       = 3
};

EstadoAutonomo estadoAuto      = AUTO_OFF;
unsigned long  t_inicio_estado = 0;
int8_t         lado_giro_memorizado = 0;  // +1 = izquierda, -1 = derecha

// Fuerza regreso a AUTO_OFF — llamar desde el failsafe y desde el reset.
void resetModoAutonomo(void)
{
    estadoAuto = AUTO_OFF;
    lado_giro_memorizado = 0;
}

// Retorna true si el modo autónomo sobreescribió out_lineal/out_angular.
// Retorna false si debe usarse el control manual.
bool ejecutarModoAutonomo(bool autoEnable, int16_t &out_lineal, int16_t &out_angular)
{
    if (!autoEnable) {
        estadoAuto = AUTO_OFF;
        lado_giro_memorizado = 0;
        return false;
    }

    switch (estadoAuto) {

        case AUTO_OFF:
            // Primer ciclo en modo auto: empezar avanzando
            estadoAuto = AUTO_AVANZANDO;
            t_inicio_estado = millis();
            out_lineal = AUTO_VEL_LINEAL;
            out_angular = 0;
            break;

        case AUTO_AVANZANDO: {
            bool obs_izq = obstaculoIzquierda();
            bool obs_der = obstaculoDerecha();

            if (obs_izq && obs_der) {
                // Ambos lados ocupados: retroceder y girar al default
                lado_giro_memorizado = AUTO_GIRO_DEFAULT_AMBOS;
                estadoAuto = AUTO_RETROCEDIENDO;
                t_inicio_estado = millis();
                out_lineal = AUTO_VEL_RETROCESO;
                out_angular = 0;
            } else if (obs_izq) {
                // Obstáculo a la izquierda: retroceder y girar a la derecha
                lado_giro_memorizado = -1;
                estadoAuto = AUTO_RETROCEDIENDO;
                t_inicio_estado = millis();
                out_lineal = AUTO_VEL_RETROCESO;
                out_angular = 0;
            } else if (obs_der) {
                // Obstáculo a la derecha: retroceder y girar a la izquierda
                lado_giro_memorizado = +1;
                estadoAuto = AUTO_RETROCEDIENDO;
                t_inicio_estado = millis();
                out_lineal = AUTO_VEL_RETROCESO;
                out_angular = 0;
            } else {
                // Camino libre: seguir avanzando
                out_lineal = AUTO_VEL_LINEAL;
                out_angular = 0;
            }
            break;
        }

        case AUTO_RETROCEDIENDO: {
            unsigned long t_transcurrido = millis() - t_inicio_estado;

            if (t_transcurrido < AUTO_T_RETROCESO_MIN_MS) {
                // Aún dentro del tiempo mínimo: retroceder sin revalidar
                out_lineal = AUTO_VEL_RETROCESO;
                out_angular = 0;
            } else {
                // Pasado el mínimo: revalidar sensores
                if (obstaculoCualquierLado()) {
                    // Sigue habiendo obstáculo: continuar retrocediendo
                    out_lineal = AUTO_VEL_RETROCESO;
                    out_angular = 0;
                } else {
                    // Camino trasero suficientemente despejado: girar
                    estadoAuto = AUTO_GIRANDO;
                    t_inicio_estado = millis();
                    out_lineal = 0;
                    out_angular = (int16_t)(lado_giro_memorizado * AUTO_VEL_ANGULAR_GIRO);
                }
            }
            break;
        }

        case AUTO_GIRANDO:
            if ((millis() - t_inicio_estado) >= AUTO_T_GIRO_MS) {
                // Giro completado: volver a avanzar
                estadoAuto = AUTO_AVANZANDO;
                t_inicio_estado = millis();
                lado_giro_memorizado = 0;
                out_lineal = AUTO_VEL_LINEAL;
                out_angular = 0;
            } else {
                // Aún dentro del tiempo de giro
                out_lineal = 0;
                out_angular = (int16_t)(lado_giro_memorizado * AUTO_VEL_ANGULAR_GIRO);
            }
            break;
    }

    return true;
}

#endif // AUTONOMO_H

#ifndef SYSTEM_H
#define SYSTEM_H

#include <Arduino.h>
#include <freertos/semphr.h>

#define RL1    4
#define RL2    0
#define prueba 25

#define STATUS_LED_ERROR   0
#define STATUS_LED_WARNING 1
#define STATUS_LED_OK      2

int8_t            status     = STATUS_LED_WARNING;
SemaphoreHandle_t tractMutex = NULL;  // guards Serial1 (traction RS232)
SemaphoreHandle_t brushMutex = NULL;  // guards Serial  (brush RS232)

#endif


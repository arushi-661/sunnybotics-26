#include  "Multi_task.h"

// multitask config
TaskHandle_t ModbusT;
TaskHandle_t statusT;

void ModbusTransmission(void *arg);
void indicaciones(void *arg);



void MultitaskInit() {
  xTaskCreatePinnedToCore(
                    ModbusTransmission,     /* Task function. */
                    "ModbusT",          /* name of task. */
                    10000,                /* Stack size of task */
                    NULL,                 /* parameter of the task */
                    1,                    /* priority of the task */
                    &ModbusT,           /* Task handle to keep track of created task */
                    0);                   /* pin task to core 0 */                  
  delay(10); 

  xTaskCreatePinnedToCore(
                    indicaciones,   /* Task function. */
                    "statusT",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &statusT,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
  delay(10); 
}







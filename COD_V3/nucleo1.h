#include  <Arduino.h>
#include  "Multi_task.h"
#include  <vector>
#include  "Wire.h"
// comunicación Sbus para control
#include  "sbus.h"
// comuncación Modbus para drivers
#include  "BrushesRTU.h"
#include <ModbusMaster.h>
// Cinematica para robot diferencial
#include  "kinematics.h"
// // // // // // // // // // // // // // // // // // // //

// MODBUS
#define HOLDING_REGISTER_ADDRESS 0
#define NUM_OF_REGISTERS 1
uint16_t holdingRegValues[NUM_OF_REGISTERS];
ModbusMaster traction;
ModbusMaster brush1;
ModbusMaster brush2;
BrushesRTU brushesM;

//SBUS
/* SBUS object, reading SBUS */
bfs::SbusRx sbus_rx(&Serial2);
/* Array for storing SBUS data */
std::array<int16_t, bfs::SbusRx::NUM_CH()> sbus_data;
// // // // // // // // // // // // // // // // // // // //

//cinematica
bfs::Kinematic Cinematica;
// variables cinematica
float velocidad_lineal;
float velocidad_angular;
int VEL_ORUGAS;
char *da[16];
int send_cmd[2];
const int uart_buffer_size = (1024 * 2);
byte error; 
// // // // // // // // // // // // // // // // // // // //

// multitask config
TaskHandle_t LecturaRF;
TaskHandle_t LecturaSensores;
TaskHandle_t Motores12;
TaskHandle_t MotoresEscobillas;

void LecturaControlRF(void *arg);
void LecturaSensoresDistancia(void *arg);
void MovimientoMotores12(void *arg);
void MovimientoMotoresEscobillas(void *arg);
// // // // // // // // // // // // // // // // // // // //

//declaración de las funciones
void SETInicialDriver1(int apagado,
	float control,
	float control_w,
	int BRU_ONOFF,
	int brushes,
	int vel_orugas,
	int vel_front,
	int vel_back);
void AsignarCanales();
// // // // // // // // // // // // // // // // // // // //


void setup() {
  //Inicicialización de los nodos modbus
  // traction.begin(3, Serial);
  // brush1.begin(1, Serial);
  Serial.begin(115200,SERIAL_8N1);  //Serial for RS485, Brushes and motion drivers
  brushesM.Begin(1,3,Serial);
  //Inicicialización
  sbus_rx.Begin(16, 17, true); //serial 2 for sbux receiver 
  Cinematica.Begin();
  delay(100);


  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
                    LecturaControlRF,     /* Task function. */
                    "LecturaRF",          /* name of task. */
                    10000,                /* Stack size of task */
                    NULL,                 /* parameter of the task */
                    1,                    /* priority of the task */
                    &LecturaRF,           /* Task handle to keep track of created task */
                    0);                   /* pin task to core 0 */                  
  delay(100); 

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
                    LecturaSensoresDistancia,   /* Task function. */
                    "LecturaSensores",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &LecturaSensores,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
  delay(100); 

  xTaskCreatePinnedToCore(
                    MovimientoMotores12,   /* Task function. */
                    "Motores12",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Motores12,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(100); 

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
                    MovimientoMotoresEscobillas,   /* Task function. */
                    "MotoresEscobillas",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &MotoresEscobillas,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
    delay(100); 
}

//Task1code: blinks an LED every 1000 ms
void LecturaControlRF(void *arg){
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());
  Cinematica.variable.driver[0]= 0;
  Cinematica.variable.driver[1]= 0;
  SETInicialDriver1(0,0,0,0,0,0,0,0);
  int b1=1,b2=1,b3=1,vel1=1,vel2=1,rst=1;
  for(;;){
    delay(1000);
  } 
}


void LecturaSensoresDistancia(void *arg){
  //Serial.print("Task2 running on core ");
  //Serial.println(xPortGetCoreID());
  for(;;){
    /*Serial.print("Tarea 2   ");
    Serial.println(xPortGetCoreID());*/
    //uint8_t *Vprint = Brushes.readCourrentm();
    delay(1000);
  }
}
void MovimientoMotores12(void *arg){
  //Serial.print("Task3 running on core ");
  //Serial.println(xPortGetCoreID());
  for(;;){
    /*Serial.print("Tarea 3   ");
    Serial.println(xPortGetCoreID());*/
    delay(1500);
  } 
}

void MovimientoMotoresEscobillas(void *arg){
  //Serial.print("Task4 running on core ");
  //Serial.println(xPortGetCoreID());
  for(;;){
    /*Serial.print("Tarea 4   ");
    Serial.println(xPortGetCoreID());*/
    delay(2000);
  }
}



void loop() {
  // uint8_t result;
  // uint16_t data[6];
  // brush1.setTransmitBuffer(0,0x0001);
  // brush1.setTransmitBuffer(0,0x0001);
  // result = brush1.writeMultipleRegisters(10004,1);
  // result = brush1.writeMultipleRegisters(10004,1);
  //state = !state;
  brushesM.StartCWBrushF();
  // driver_nuevo.StartCWBrush();
  delay(1000);
  // driver_nuevo.V100Brush();
  // delay(3000);
  // driver_nuevo.V500Brush();
  // delay(3000);
  // driver_nuevo.V3000Brush();
  // delay(3000);
  // driver_nuevo.StopCWBrush();
  // delay(500);
  // driver_nuevo.StartCCWBrush();
  // delay(10000);
  // driver_nuevo.V3000Brush();
  // delay(3000);
  // driver_nuevo.V500Brush();
  // delay(3000);
  // driver_nuevo.V100Brush();
  // delay(3000);
  // driver_nuevo.StopCCWBrush();
  // delay(3000);
} 


void SETInicialDriver1(int apagado,	float control, float control_w, int BRU_ONOFF, int brushes,	int vel_orugas,	int vel_front, int vel_back)
{
    sbus_rx.r9ds.apagado=apagado;
    sbus_rx.r9ds.vel_back=vel_back;
    sbus_rx.r9ds.vel_front=vel_front;
    sbus_rx.r9ds.vel_orugas=vel_orugas;
    sbus_rx.r9ds.brushes=brushes;
    sbus_rx.r9ds.BRU_ONOFF=BRU_ONOFF;
    sbus_rx.r9ds.control_w=control_w;
    sbus_rx.r9ds.control=control;

}

void AsignarCanales()
{
  for(int i = 0; i<16 ; i++ )
  {
      sbus_rx.r9ds.channels[i] = sbus_data[i];
  }
}
#include  "TractionRTU.h"
#define TRACTION_LEFT_ADDRESS 1 //P018 del Driver
#define TRACTION_RIGHT_ADDRESS 2 //P018 del Driver
#define TRACTION_BAUDRATE 115200
#define TractionSerialPort Serial
#define MAX485_INOUT 5

#define SEPARATION_N 1

TractionRTU motorleft;
TractionRTU motorright;



void tractionInit(void)
{   pinMode(MAX485_INOUT, OUTPUT);
    digitalWrite(MAX485_INOUT, LOW );

    motorleft.init(TractionSerialPort, TRACTION_LEFT_ADDRESS, TRACTION_BAUDRATE);
    motorleft._modbusRTURS485.preTransmission(preTransmission);
    motorleft._modbusRTURS485.postTransmission(postTransmission);
    motorleft.modbusControlled();
    motorleft.stop();

    motorright.init(TractionSerialPort, TRACTION_RIGHT_ADDRESS, TRACTION_BAUDRATE);
    motorright._modbusRTURS485.preTransmission(preTransmission);
    motorright._modbusRTURS485.postTransmission(postTransmission);
    // delay(1);
    motorright.modbusControlled();
    motorright.stop();
}



void leermotores(void)
{
    motorleft.speed(ControlRemoto1.Ctrl_FrontRear*30);
    motorright.speed(-ControlRemoto1.Ctrl_FrontRear*30);
}

void CinematicaDirecta(float velocidad_lineal,float velocidad_angular){
    int RPM_derecha=0,RPM_izquierda=0;
    RPM_derecha =-(velocidad_lineal - ((velocidad_angular * SEPARATION_N) / 2));
    RPM_izquierda = velocidad_lineal + ((velocidad_angular * SEPARATION_N) / 2);
    if (RPM_derecha < 100 & RPM_derecha>=0) RPM_derecha = 0;
    if (RPM_derecha > -100 & RPM_derecha<0) RPM_derecha = 0;
    if (RPM_izquierda < 100 & RPM_izquierda>=0) RPM_izquierda = 0;
    if (RPM_izquierda > -100 & RPM_izquierda<0) RPM_izquierda = 0;//esto es para que no se mueva si la velocidad es muy baja

    motorright.speed(RPM_derecha);
    motorleft.speed(RPM_izquierda);
    // Serial.print( String(RPM_derecha) + "\t" );
    // Serial.print( String(RPM_izquierda) + "\t" );
};








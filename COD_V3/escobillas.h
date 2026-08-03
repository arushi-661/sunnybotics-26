#include  "BrushRTU.h"
#define BRUSH_FRONT_ADDRESS 3 //P018 del Driver
#define BRUSH_REAR_ADDRESS 4 //P018 del Driver
#define BRUSHES_BAUDRATE 115200
#define BrushedSerialPort Serial
#define MAX485_INOUT 5

BrushRTU brushFront;
BrushRTU brushRear;

void preTransmission()
{   digitalWrite(MAX485_INOUT, HIGH );
    //delay(1);
}

void postTransmission()
{   digitalWrite(MAX485_INOUT, LOW );
    //delay(1);
}


void brushesInit(void)
{   pinMode(MAX485_INOUT, OUTPUT);
    digitalWrite(MAX485_INOUT, LOW );

    brushFront.init(BrushedSerialPort, BRUSH_FRONT_ADDRESS, BRUSHES_BAUDRATE);
    brushFront._modbusRTURS485.preTransmission(preTransmission);
    brushFront._modbusRTURS485.postTransmission(postTransmission);
    brushFront.modbusControlled();
    brushFront.direction(CCW);
    brushFront.stop();

    brushRear.init(BrushedSerialPort, BRUSH_REAR_ADDRESS, BRUSHES_BAUDRATE);
    brushRear._modbusRTURS485.preTransmission(preTransmission);
    brushRear._modbusRTURS485.postTransmission(postTransmission);
    brushRear.modbusControlled();
    brushRear.direction(CW);
    brushRear.stop();

}



void controlFrontBrush(void)
{
    if( ControlRemoto1.Ctrl_BrushFrontLevelOnOff==0)
    {   brushFront.stop();
        return;
    }
    if( ControlRemoto1.Ctrl_BrushFrontLevelOnOff==1)
    {   if( ControlRemoto1.Ctrl_BrushFrontLevel==1 ){ brushFront.speed(10);   }//Serial.print("V-1");}
        if( ControlRemoto1.Ctrl_BrushFrontLevel==2 ){ brushFront.speed(100);  }//Serial.print("V-2");}
        if( ControlRemoto1.Ctrl_BrushFrontLevel==3 ){ brushFront.speed(1000); }//Serial.print("V-3");}
        brushFront.run();
    }
}


void controlRearBrush(void)
{
    if( ControlRemoto1.Ctrl_BrushRearLevelOnOff==0)
    {   brushRear.stop();
        return;  
    } 
    if( ControlRemoto1.Ctrl_BrushRearLevelOnOff==1)
    {   if( ControlRemoto1.Ctrl_BrushRearLevel==1 ){ brushRear.speed(10);   }//Serial.print("V-1");}
        if( ControlRemoto1.Ctrl_BrushRearLevel==2 ){ brushRear.speed(100);  }//Serial.print("V-2");}
        if( ControlRemoto1.Ctrl_BrushRearLevel==3 ){ brushRear.speed(1000); }//Serial.print("V-3");}
        brushRear.run();
    }
}

void leerEscobillas(void)
{
    switch (ControlRemoto1.Ctrl_BrushFrontLevel)
    {
    case 1:
        brushFront.direction(CCW);
        brushFront.run();
        digitalWrite(RL1, HIGH);
        break;
    case 2:
        brushFront.stop();
        digitalWrite(RL1, LOW);
        break;
    case 3:
        brushFront.direction(CW);
        brushFront.run();
        digitalWrite(RL1, HIGH);
        break;
    default:
        break;
    }

    switch (ControlRemoto1.Ctrl_BrushRearLevel)
    {
    case 1:
        brushRear.direction(CW);
        brushRear.run();
        digitalWrite(RL2, HIGH);
        break;
    case 2:
        brushRear.stop();
        digitalWrite(RL2, LOW);
        break;
    case 3:
        brushRear.direction(CCW);
        brushRear.run();
        digitalWrite(RL2, HIGH);
        break;
    default:
        break;
    }

    if( ControlRemoto1.Ctrl_BrushFrontLevelOnOff==0 )
    { brushFront.speed(500);
    }
    if( ControlRemoto1.Ctrl_BrushFrontLevelOnOff==1 )
    { brushFront.speed(1500);
    }
    if( ControlRemoto1.Ctrl_BrushFrontLevelOnOff==3 )
    { brushFront.speed(0);
    }
    if( ControlRemoto1.Ctrl_BrushRearLevelOnOff==0 )
    { brushRear.speed(500);
    }
    if( ControlRemoto1.Ctrl_BrushRearLevelOnOff==1 )
    { brushRear.speed(1500);
    }
    if( ControlRemoto1.Ctrl_BrushRearLevelOnOff==3 )
    { brushRear.speed(0);
    }
}








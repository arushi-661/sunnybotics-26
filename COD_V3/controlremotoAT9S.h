#include "SBUS.h"
#include  "ControlRemoto.h"
#define CHANNEL_1_           0  
#define CHANNEL_2_FRONT_REAR 1  
#define CHANNEL_3_           2  
#define CHANNEL_4_RIGHT_LEFT 3  

#define CHANNEL_5_BRUSH_FRONT_ONOFF 4 
#define CHANNEL_6_BRUSH_FRONT_LEVEL 5

#define CHANNEL_7_BRUSH_REAR_ONOFF  6
#define CHANNEL_8_BRUSH_REAR_LEVEL  7

#define CHANNEL_9_RESET_SIGNAL      8 
#define CHANNEL_A_VALVE_ONOFF       9
#define CHANNEL_B_LIGTHS_ONOFF      10

#define CONTROL_CONECTADO 0





ControlRemoto ControlRemoto1;

SBUS x8r(Serial2); //RX 16
uint16_t channels[16];
bool failSafe;
bool lostFrame;
int contadorFallo=0;


void controlremotoInit(void)
{ // x8r.begin();
  x8r.begin(16, 17, true, 100000); // optional parameters for ESP32: RX pin, TX pin, inverse mode, baudrate
}

int16_t LimitarEscalar(int16_t InputValue, int16_t limitInputLow, int16_t limitInputHigh, int16_t limitOutputLow, int16_t limitOutputHigh );
int16_t onOff(int16_t InputValue, int16_t limitInputLow, int16_t limitInputHigh, int16_t invertido );
int16_t Level3(int16_t InputValue, int16_t limitInputLow, int16_t limitInputHigh );


void leerControlRemoto(void)
{ 
  if(x8r.read(&channels[0], &failSafe, &lostFrame))
  { 
    contadorFallo=0;
		ControlRemoto1.Ctrl_failSafe=failSafe;
		ControlRemoto1.Ctrl_lostFrame=lostFrame;

    ControlRemoto1.Ctrl_FrontRear = LimitarEscalar( (int16_t) channels[CHANNEL_2_FRONT_REAR], 350,1650, 2000,-2000);
    ControlRemoto1.Ctrl_RightLeft = LimitarEscalar( (int16_t) channels[CHANNEL_4_RIGHT_LEFT], 350,1650,-2000, 2000);
    
    ControlRemoto1.Ctrl_BrushFrontLevelOnOff = onOff( channels[CHANNEL_5_BRUSH_FRONT_ONOFF], 300, 1600, 0 );
    ControlRemoto1.Ctrl_BrushFrontLevel      = Level3( channels[CHANNEL_6_BRUSH_FRONT_LEVEL], 300, 1600 );
    
    ControlRemoto1.Ctrl_BrushRearLevelOnOff  = onOff( channels[CHANNEL_7_BRUSH_REAR_ONOFF],  300, 1600, 0 );
    ControlRemoto1.Ctrl_BrushRearLevel       = Level3( channels[CHANNEL_8_BRUSH_REAR_LEVEL], 300, 1600 );

    ControlRemoto1.Ctrl_ResetSignal = onOff( channels[CHANNEL_9_RESET_SIGNAL],  300, 1600, 0 );
    ControlRemoto1.Ctrl_Valve       = LimitarEscalar( (int16_t) channels[CHANNEL_A_VALVE_ONOFF], 350,1650, 3,6);
    //ControlRemoto1.Ctrl_Ligths = channels[CHANNEL_B_LIGTHS_ONOFF];
  }else{
    contadorFallo++;
    if(contadorFallo>=10){
      ControlRemoto1.Ctrl_failSafe=1;//ControlRemoto1.Ctrl_failSafe=0;
      ControlRemoto1.Ctrl_lostFrame=1;//ControlRemoto1.Ctrl_lostFrame=0;
      ControlRemoto1.Ctrl_FrontRear=0;
      ControlRemoto1.Ctrl_RightLeft=0;
      ControlRemoto1.Ctrl_BrushFrontLevelOnOff=3;
      // ControlRemoto1.Ctrl_BrushFrontLevel=2;
      ControlRemoto1.Ctrl_BrushRearLevelOnOff=3;
      // ControlRemoto1.Ctrl_BrushRearLevel=2;
      // ControlRemoto1.Ctrl_ResetSignal=0;
      // ControlRemoto1.Ctrl_Valve=0;
      //ControlRemoto1.Ctrl_Ligths=0;
    }

  }
}
void simularControlRemoto(int16_t FrontRear,int16_t RightLeft,int16_t BrushFrontLevelOnOff,int16_t BrushFrontLevel,int16_t BrushRearLevelOnOff,int16_t BrushRearLevel,int16_t ResetSignal,int16_t Valve){
  ControlRemoto1.Ctrl_failSafe=0;
  ControlRemoto1.Ctrl_lostFrame=0;
  ControlRemoto1.Ctrl_FrontRear=FrontRear;//de -2000 a 2000 
  ControlRemoto1.Ctrl_RightLeft=RightLeft;//de -2000 a 2000
  ControlRemoto1.Ctrl_BrushFrontLevelOnOff=BrushFrontLevelOnOff;//0=off, 1=on
  ControlRemoto1.Ctrl_BrushFrontLevel=BrushFrontLevel;//0=low, 1=mid, 2=high
  ControlRemoto1.Ctrl_BrushRearLevelOnOff=BrushRearLevelOnOff;//0=off, 1=on
  ControlRemoto1.Ctrl_BrushRearLevel=BrushRearLevel;//0=low, 1=mid, 2=high
  ControlRemoto1.Ctrl_ResetSignal=ResetSignal;//0=off, 1=on
  ControlRemoto1.Ctrl_Valve=Valve;//de 1 a 10
}

int16_t LimitarEscalar(int16_t InputValue, int16_t limitInputLow, int16_t limitInputHigh, int16_t limitOutputLow, int16_t limitOutputHigh )
{ int16_t OutputValue = 0 ;
  if(InputValue>=limitInputHigh){ InputValue=limitInputHigh; }
  if(InputValue<= limitInputLow){ InputValue= limitInputLow;  }
  OutputValue = map(InputValue, limitInputLow, limitInputHigh, limitOutputLow, limitOutputHigh); //map(value, fromLow, fromHigh, toLow, toHigh)
  return OutputValue;
}

int16_t onOff(int16_t InputValue, int16_t limitInputLow, int16_t limitInputHigh, int16_t invertido )
{ int16_t OutputValue = 0 ;
  //Limitadors de entrada
  if(InputValue>=limitInputHigh){ InputValue=limitInputHigh; }
  if(InputValue<= limitInputLow){ InputValue= limitInputLow;  }

  //Logica de salida
  if( InputValue > (limitInputHigh - 100) ){ OutputValue =  1; }
  if( InputValue < (limitInputLow  + 100) ){ OutputValue =  0; }
  if( invertido == 1 )                     { OutputValue =  abs(OutputValue-1) ; }

  return OutputValue;
}


int16_t Level3(int16_t InputValue, int16_t limitInputLow, int16_t limitInputHigh )
{ int16_t OutputValue = 0 ;
  //Limitadors de entrada
  if(InputValue>=limitInputHigh){ InputValue=limitInputHigh; }
  if(InputValue<= limitInputLow){ InputValue= limitInputLow;  }

  //Logica de salida
  if( InputValue < (limitInputLow  + 100) )                    { OutputValue =  1; }
  if( abs(InputValue-((limitInputLow+limitInputHigh)/2) )<100 ){ OutputValue =  2; }
  if( InputValue > (limitInputHigh - 100) )                    { OutputValue =  3; }

  return OutputValue;
}

void MostrarValores(void)
{
  Serial.print( String(ControlRemoto1.Ctrl_failSafe) + "\t" );
  Serial.print( String(ControlRemoto1.Ctrl_lostFrame) + "\t" );

  Serial.print( String(ControlRemoto1.Ctrl_FrontRear) + "\t" );
  Serial.print( String(ControlRemoto1.Ctrl_RightLeft) + "\t" );

  // Serial.print( String(ControlRemoto1.Tare_FrontRear) + "\t" );
  // Serial.print( String(ControlRemoto1.Tare_RightLeft) + "\t" );

  Serial.print( String(ControlRemoto1.Ctrl_BrushFrontLevelOnOff) + "\t" );
  Serial.print( String(ControlRemoto1.Ctrl_BrushFrontLevel) + "\t" );

  Serial.print( String(ControlRemoto1.Ctrl_BrushRearLevelOnOff) + "\t" );
  Serial.print( String(ControlRemoto1.Ctrl_BrushRearLevel) + "\t" );
  

  Serial.print( String(ControlRemoto1.Ctrl_ResetSignal) + "\t" );
  Serial.print( String(ControlRemoto1.Ctrl_Valve) + "\t" );

  Serial.print( String(ControlRemoto1.Ctrl_Ligths) + "\t" );

  Serial.println(contadorFallo);
}

void MostrarCanales(void){
  for (int i = 0; i < 15 ; i++)
  {
    Serial.print( String(channels[i]) + "\t" );
  }
  Serial.println();
}

void tararControl(bool status)
{
  if (status == false)
  {
    ControlRemoto1.Tare_FrontRear=0;
    ControlRemoto1.Tare_RightLeft=0;
    leerControlRemoto();
    ControlRemoto1.Tare_FrontRear=ControlRemoto1.Ctrl_FrontRear;
    ControlRemoto1.Tare_RightLeft=ControlRemoto1.Ctrl_RightLeft;
  }
}


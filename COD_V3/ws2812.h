#include "Adafruit_NeoPixel.h"

#define LED_COUNT 1
#define LED_PIN 22

#define ORANGE (40, 20, 0)
#define RED (20, 0, 0)
#define GREEN (0, 60, 0)
#define BLANK (0, 0, 0)

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void statusled(int status){
    if (status ==0){ //error status blink red
      
      pixels.setPixelColor(0, pixels.Color RED);
      pixels.show();
      delay(100);
      pixels.setPixelColor(0, pixels.Color BLANK);
      pixels.show();
      delay(100);
      pixels.setPixelColor(0, pixels.Color RED);
      pixels.show();
      delay(100);
      pixels.setPixelColor(0, pixels.Color BLANK);
      pixels.show();
      digitalWrite(RL1, HIGH);
      delay(1200);
    }

    if (status ==1){ // searching controller status, blink orange
      pixels.setPixelColor(0, pixels.Color ORANGE);
      pixels.show();
      delay(100);
      pixels.setPixelColor(0, pixels.Color BLANK);
      pixels.show();
      delay(100);
      pixels.setPixelColor(0, pixels.Color ORANGE);
      pixels.show();
      delay(100);
      pixels.setPixelColor(0, pixels.Color BLANK);
      pixels.show();
      //digitalWrite(RL1, HIGH);
      delay(1200);
    }
    
    else if (status==2){ //ok status, blink green
      pixels.setPixelColor(0, pixels.Color GREEN);
      pixels.show();
      delay(750);
      pixels.setPixelColor(0, pixels.Color BLANK);
      pixels.show();
      digitalWrite(RL1, LOW);
      delay(750);
    } 
}

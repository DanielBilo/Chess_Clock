#include <max7219.h>
#include "functions.h"

int leftButtonPin = 4;        //Active high
int rightButtonPin = 3;       //Active high
int rotaryCLKPin = 2;         
int rotaryBPin = 5;           //Active low
int rotaryButtonPin = 6;
char displayBrightness = 1;
volatile bool lastRotaryCLKState = 0;
int encoderPosCount = 0;
bool intFlag = 0;

MAX7219 max7219;

void setup() {
  Serial.begin(9600);
  max7219.Begin();
  max7219.MAX7219_SetBrightness(displayBrightness);

  pinMode(leftButtonPin, INPUT);
  pinMode(rightButtonPin, INPUT);
  pinMode(rotaryCLKPin, INPUT);
  pinMode(rotaryBPin, INPUT);
  pinMode(rotaryButtonPin, INPUT);
  lastRotaryCLKState = digitalRead(rotaryCLKPin);
  
  attachInterrupt(digitalPinToInterrupt(rotaryCLKPin), rotaryUpdate, CHANGE);
}

void loop() {

  if(intFlag)
  {
    Serial.println(encoderPosCount);
    intFlag = 0;
  }  
  //Display Char Demo

    // max7219.Clear();
    // max7219.DisplayChar(7, 'H', 0); //Position 7 is on the left of the display
    // delay(500);
    
    // max7219.DisplayChar(6, 'E', 0);
    // delay(500);
    // max7219.DisplayChar(5, 'L', 0);
    // delay(500);
    // max7219.DisplayChar(4, 'L', 0);
    // delay(500);
    // max7219.DisplayChar(3, 'O', 0);
    // delay(500);
    // max7219.DisplayChar(2, '1', 0);
    // delay(500);
    // max7219.DisplayChar(1, '2', 0);
    // delay(500);
    // max7219.DisplayChar(0, '3', 0);

    // delay(500);
    // max7219.Clear();


}


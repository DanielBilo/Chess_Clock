#include <max7219.h>
#include "functions.h"

int leftButtonPin = 4;        //Active high
int rightButtonPin = 3;       //Active high
int rotaryCLKPin = 2;         
int rotaryBPin = 5;           //Active low
int rotaryButtonPin = 6;

char displayBrightness = 1;

volatile bool lastRotaryCLKState = 0;
bool previousRotaryButton = 0;
bool previousLeftButtonPin = 0;
bool previousRightButtonPin = 0;
bool currentLeftButtonPin = 0;
bool currentRightButtonPin = 0;
bool currentRotaryButton = 0;
int encoderPosCount = 0;
bool intFlag = 0;
unsigned long currentMillis, previousMillis, playerLeftTime, playerRightTime, playerLeftInc, playerRightInc, playerLeftTimeInit, playerRightTimeInit;



enum {Ready, Cnt1, Cnt2, GameDone, Pause, SetTimeLeft, SetTimeRight, SetIncLeft, SetIncRight};
unsigned char state, previousState;

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

  attachInterrupt(digitalPinToInterrupt(rotaryCLKPin), rotaryUpdate, CHANGE);

  lastRotaryCLKState = digitalRead(rotaryCLKPin);
  previousRotaryButton = digitalRead(rotaryButtonPin);
  currentMillis = millis();
  previousMillis = currentMillis;
  previousLeftButtonPin = digitalRead(leftButtonPin);
  previousRightButtonPin = digitalRead(rightButtonPin);
  playerLeftTimeInit = 10 * 60 * 1000; //10 minutes temps de départ
  playerRightTimeInit = 10 * 60 * 1000; //10 minutes temps de départ
  playerLeftInc = 10 * 1000; //10 sec incrément
  playerRightInc = 10 * 1000; //10 sec incrément

  
  
}

void loop() {

  if(intFlag)
  {
    Serial.println(encoderPosCount);
    intFlag = 0;
  }  

  currentRotaryButton = digitalRead(rotaryButtonPin);
  currentLeftButtonPin = digitalRead(leftButtonPin);
  currentRightButtonPin = digitalRead(rightButtonPin);
  currentMillis = millis();

  switch(state)
  {
    case Ready:
      if(intFlag)
      {
        encoderPosCount = 0;
        intFlag = 0;
      }
      if(previousLeftButtonPin != currentLeftButtonPin)
      {
        previousLeftButtonPin = currentLeftButtonPin;
        if(currentLeftButtonPin)
        {
          previousMillis = currentMillis;
          state = Cnt1;
          playerLeftTime = playerLeftTimeInit;
          playerRightTime = playerRightTimeInit;
        }
      }
      if(previousRightButtonPin != currentRightButtonPin)
      {
        previousRightButtonPin = currentRightButtonPin;
        if(currentRightButtonPin)
        {
          previousMillis = currentMillis;
          playerLeftTime = playerLeftTimeInit;
          playerRightTime = playerRightTimeInit;
          state = Cnt2;
        }
      }
      break;

    case Cnt1:
      //Décrémentation du temps
      playerLeftTime -= (currentMillis - previousMillis);
      previousMillis = currentMillis;
      //détection de fin de tour
      if(previousRightButtonPin != currentRightButtonPin)
      previousRightButtonPin = currentRightButtonPin;
      {
        if(currentRightButtonPin)
        {
          playerLeftTime += playerLeftInc;
          state = Cnt2;
        }
      }
      //Fin de la partie, manque de temps
      if (!playerLeftTime)
      {
        state = GameDone;
      }
      //Mise sur pause
      if(currentRotaryButton != previousRotaryButton)
      {
        previousRotaryButton = currentRotaryButton;
        if(currentRotaryButton)
        {
          previousState = Cnt1;
          state = Pause;
        }
      }

      break;

    case Cnt2:
      //Décrémentation du temps
      playerRightTime -= (currentMillis - previousMillis);
      previousMillis = currentMillis;
      //détection de fin de tour
      if(previousLeftButtonPin != currentLeftButtonPin)
      {
        previousLeftButtonPin = currentLeftButtonPin;
        if(currentRightButtonPin)
        {
          playerRightTime += playerRightInc;
          state = Cnt1;
        }
      }
      //Fin de la partie, manque de temps
      if (!playerRightTime)
      {
        state = GameDone;
      }
      //Mise sur pause
      if(currentRotaryButton != previousRotaryButton)
      {
        previousRotaryButton = currentRotaryButton;
        if(currentRotaryButton)
        {
          previousState = Cnt2;
          state = Pause;
        }

      }

    
      break;
    case GameDone:
      //display game done
      if(currentRotaryButton != previousRotaryButton)
      {
        previousRotaryButton = currentRotaryButton;
        if(currentRotaryButton)
        {
          playerLeftTime = playerLeftTimeInit;
          playerRightTime = playerRightTimeInit;
          state = Ready;
        }
      }
      break;
    case Pause:
      if(currentRotaryButton != previousRotaryButton)
      {
        previousRotaryButton = currentRotaryButton;
        if(currentRotaryButton)
        {
          playerLeftTime = playerLeftTimeInit;
          playerRightTime = playerRightTimeInit;
          state = previousState;
        }
      }
      //diplay paused time
      break;
    case SetTimeLeft:
      break;
    case SetTimeRight:
      break;
    case SetIncLeft:
      break;
    case SetIncRight:
      break;
  }
      

}


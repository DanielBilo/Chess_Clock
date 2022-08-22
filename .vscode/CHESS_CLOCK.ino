#include <max7219.h>
#include "functions.h"

int leftButtonPin = 4;        //Active high
int rightButtonPin = 3;       //Active high
int rotaryCLKPin = 2;         
int rotaryBPin = 5;           //Active low
int rotaryButtonPin = 6;

char displayBrightness = 3;

volatile bool lastRotaryCLKState = 0;
bool previousRotaryButton = 0;
bool previousLeftButton = 0;
bool previousRightButton = 0;
bool currentLeftButton = 0;
bool currentRightButton = 0;
bool currentRotaryButton = 0;
int encoderPosCount = 0;
bool intFlag = 0;
bool resetFlag = 0;
bool pauseInput = 0;
int resetCounter = 0;
uint16_t debounceStateRotaryButton = 0;
uint16_t debounceStateLeftButton = 0;
uint16_t debounceStateRightButton = 0;
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
  previousLeftButton = digitalRead(leftButtonPin);
  previousRightButton = digitalRead(rightButtonPin);
  playerLeftTimeInit = 50 * 60000;              // temps de départ
  playerRightTimeInit = 50 * 60000;             // temps de départ
  playerLeftTime = playerLeftTimeInit;
  playerRightTime = playerRightTimeInit;
  playerLeftInc = 10000;                    //incrément initial
  playerRightInc = 10000;                   //incrément initial

  
  
}

void loop() {

  if(intFlag)
  {
    Serial.println(encoderPosCount);
    intFlag = 0;
  }  

  currentRotaryButton = debounce(rotaryButtonPin, &debounceStateRotaryButton);
  // currentRotaryButton = digitalRead(rotaryButtonPin);

  currentLeftButton = debounce(leftButtonPin, &debounceStateLeftButton);
  currentRightButton = debounce(rightButtonPin, &debounceStateRightButton);
  if(currentRotaryButton)
  {
    Serial.println("Rotary high");
  }
  currentMillis = millis();

  switch(state)
  {
    case Ready:
      if(intFlag)
      {
        encoderPosCount = 0;
        intFlag = 0;
      }
      if(previousLeftButton != currentLeftButton)
      {
        previousLeftButton = currentLeftButton;
        if(currentLeftButton)
        {
          previousMillis = currentMillis;
          state = Cnt1;
          playerLeftTime = playerLeftTimeInit;
          playerRightTime = playerRightTimeInit;
        }
      }
      if(previousRightButton != currentRightButton)
      {
        previousRightButton = currentRightButton;
        if(currentRightButton)
        {
          previousMillis = currentMillis;
          playerLeftTime = playerLeftTimeInit;
          playerRightTime = playerRightTimeInit;
          state = Cnt2;
        }
      }
      diplayClock(playerLeftTime, playerRightTime);
      break;

    case Cnt1:
      //Décrémentation du temps
      playerLeftTime = playerLeftTime -  (currentMillis - previousMillis);
      previousMillis = currentMillis;
      //détection de fin de tour
      if(previousRightButton != currentRightButton)
      previousRightButton = currentRightButton;
      {
        if(currentRightButton)
        {
          playerLeftTime = playerLeftTime +  playerLeftInc;
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
        if(!currentRotaryButton)
        {
          previousState = Cnt1;
          state = Pause;
        }
      }
      diplayClock(playerLeftTime, playerRightTime);
      break;

    case Cnt2:
      //Décrémentation du temps
      playerRightTime = playerRightTime - (currentMillis - previousMillis);
      previousMillis = currentMillis;
      //détection de fin de tour
      if(previousLeftButton != currentLeftButton)
      {
        previousLeftButton = currentLeftButton;
        if(currentLeftButton)
        {
          playerRightTime = playerRightTime + playerRightInc;
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
        if(!currentRotaryButton)
        {
          previousState = Cnt2;
          state = Pause;
        }

      }
      diplayClock(playerLeftTime, playerRightTime);
      break;

    case GameDone:
      //display game done
      if(currentRotaryButton != previousRotaryButton)
      {
        previousRotaryButton = currentRotaryButton;
        if(!currentRotaryButton)
        {
          playerLeftTime = playerLeftTimeInit;
          playerRightTime = playerRightTimeInit;
          state = Ready;
        }
      }
      break;

    case Pause:

      //if falling edge is detected, get ready for new input
      if(currentRotaryButton != previousRotaryButton)
      {
        if(currentRotaryButton)
        {
          pauseInput = 1;
        }
      }
      //
      if(pauseInput)
      {
        if(currentRotaryButton != previousRotaryButton)
        {
          previousRotaryButton = currentRotaryButton;
          //restart game on falling edge of button, waiting for reset if necessary
          if(currentRotaryButton)
          {
            state = previousState;
            pauseInput = 0;
            previousMillis = currentMillis;
          }
        }
          //button is being held down
          if(!currentRotaryButton)
          {
            resetCounter++;
          }
          //Button as been released
          else
          {
            resetCounter = 0;
            resetFlag = 0;
          }
        //if button is held down for long enough, reset the game
        if(resetCounter > 500)
        {
          state = Ready;
          playerLeftTime = playerLeftTimeInit;
          playerRightTime = playerRightTimeInit;
          resetCounter = 0;
          pauseInput = 0;
        }
      }
      diplayClock(playerLeftTime, playerRightTime);
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


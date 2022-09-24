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
bool resetFlag = 0;
bool pauseInput = 0;
bool flashToggle = 0;
int resetCounter = 0;
uint16_t debounceStateRotaryButton = 0;
uint16_t debounceStateLeftButton = 0;
uint16_t debounceStateRightButton = 0;
unsigned long currentMillis, previousMillis, playerLeftTime, playerRightTime, playerLeftInc, 
              playerRightInc, playerLeftTimeInit, playerRightTimeInit, previousBlinkMillis,
              settingEncoderULong;

long settingEncoderLong;



enum {Ready, Cnt1, Cnt2, GameDone, Pause, SetTimeLeft, SetTimeRight, SetIncLeft, SetIncRight};
unsigned char state, previousState;

MAX7219 max7219;

void setup() {
  max7219.Begin();
  max7219.MAX7219_SetBrightness(displayBrightness);

  pinMode(leftButtonPin, INPUT_PULLUP);
  pinMode(rightButtonPin, INPUT_PULLUP);
  pinMode(rotaryCLKPin, INPUT);
  pinMode(rotaryBPin, INPUT);
  pinMode(rotaryButtonPin, INPUT);

  attachInterrupt(digitalPinToInterrupt(rotaryCLKPin), rotaryUpdate, CHANGE);

  lastRotaryCLKState = digitalRead(rotaryCLKPin);
  previousRotaryButton = digitalRead(rotaryButtonPin);
  currentMillis = millis();
  previousMillis = currentMillis;
  previousBlinkMillis = currentMillis;
  previousLeftButton = digitalRead(leftButtonPin);
  previousRightButton = digitalRead(rightButtonPin);
  playerLeftTimeInit = 2 * 60000;              // temps de départ
  playerRightTimeInit = 2 * 60000;             // temps de départ
  playerLeftTime = playerLeftTimeInit;
  playerRightTime = playerRightTimeInit;
  playerLeftInc = 10000;                    //incrément initial
  playerRightInc = 10000;                   //incrément initial

  
  
}

void loop() {

  //update all inputs
  currentRotaryButton = debounce2(rotaryButtonPin, previousRotaryButton);
  currentLeftButton = !(debounce2(leftButtonPin, previousLeftButton));
  currentRightButton = !(debounce2(rightButtonPin, previousRightButton));
  currentMillis = millis();

  switch(state)
  {
    case Ready:
      if(encoderPosCount)
      {
        if(encoderPosCount < 0)
        {
          int temp = displayBrightness - '0';
          if((temp + encoderPosCount) <= -48) //0 in char format
          {
            displayBrightness = 0;
          }
          else
          {
            displayBrightness = displayBrightness + encoderPosCount;
          }
        }        else
        {
          if((displayBrightness + encoderPosCount) >= 0xF)
          {
            displayBrightness = 0xF;
          }
          else
          {
            displayBrightness += encoderPosCount;
          }
        }
        encoderPosCount = 0;
        max7219.MAX7219_SetBrightness(displayBrightness);

      }
      if(previousLeftButton != currentLeftButton)
      {
        previousLeftButton = currentLeftButton;
        if(currentLeftButton)
        {
          previousMillis = currentMillis;
          state = Cnt1;
        }
      }
      if(previousRightButton != currentRightButton)
      {
        previousRightButton = currentRightButton;
        if(currentRightButton)
        {
          previousMillis = currentMillis;
          state = Cnt2;
        }
      }

      if(currentRotaryButton != previousRotaryButton)
      {
        previousRotaryButton = currentRotaryButton;
        if(!currentRotaryButton) //rising edge
        {
          state = SetTimeLeft;
        }
      }
      displayClock(playerLeftTime, playerRightTime);
      break;

    case Cnt1:
      //Decrementing time
      if(playerLeftTime < (currentMillis - previousMillis))
      {
        playerLeftTime = 0;
      }
      else
      {
        playerLeftTime = playerLeftTime -  (currentMillis - previousMillis);
      }
      previousMillis = currentMillis;

      //End of turn detection
      if(previousRightButton != currentRightButton)
      previousRightButton = currentRightButton;
      {
        if(currentRightButton) //Rising edge
        {
          playerLeftTime = playerLeftTime + playerLeftInc;
          state = Cnt2;
        }
      }
      //Put on pause
      if(currentRotaryButton != previousRotaryButton)
      {
        previousRotaryButton = currentRotaryButton;
        if(!currentRotaryButton) //Rising edge
        {
          previousState = Cnt1;
          state = Pause;
        }
      }
      if(playerLeftTime <= 0)
      {
        playerLeftTime = 0;
        state = GameDone;
      }
      displayClock(playerLeftTime, playerRightTime);
      break;

    case Cnt2:
      //Decrementing time
      if(playerRightTime < (currentMillis - previousMillis))
      {
        playerRightTime = 0;
      }
      else
      {
        playerRightTime = playerRightTime -  (currentMillis - previousMillis);
      }
      previousMillis = currentMillis;

      //End of turn detection
      if(previousLeftButton != currentLeftButton)
      {
        previousLeftButton = currentLeftButton;
        if(currentLeftButton)
        {
          playerRightTime = playerRightTime + playerRightInc;
          state = Cnt1;
        }
      }
      //Put on pause
      if(currentRotaryButton != previousRotaryButton)
      {
        previousRotaryButton = currentRotaryButton;
        if(!currentRotaryButton)
        {
          previousState = Cnt2;
          state = Pause;
        }
      }
      if(playerRightTime <= 0)
      {
        playerRightTime = 0;
        state = GameDone;
      }
      displayClock(playerLeftTime, playerRightTime);
      break;

    case GameDone:
      if(currentRotaryButton != previousRotaryButton)
      {
        previousRotaryButton = currentRotaryButton;
        if(currentRotaryButton) //Falling edge
        {
          playerLeftTime = playerLeftTimeInit;
          playerRightTime = playerRightTimeInit;
          state = Ready;
        }
      }
      displayClock(playerLeftTime, playerRightTime);
      break;

    case Pause:

      //if falling edge is detected, get ready for new input
      if(currentRotaryButton != previousRotaryButton and pauseInput == 0)
      {
        previousRotaryButton = currentRotaryButton;

          pauseInput = 1;
        
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
        if(resetCounter > 200)
        {
          state = Ready;
          playerLeftTime = playerLeftTimeInit;
          playerRightTime = playerRightTimeInit;
          resetCounter = 0;
          pauseInput = 0;
        }
      }
      displayClock(playerLeftTime, playerRightTime);
      break;

    case SetTimeLeft:
      if(encoderPosCount)
      {
        settingEncoderLong = encoderPosCount * 60000;
        if(settingEncoderLong < 0)
        {
          settingEncoderULong = -settingEncoderLong;
          if(playerRightTimeInit <= settingEncoderULong)
          {
            playerRightTimeInit = 60000;
          }
          else
          {
            playerRightTimeInit = (playerRightTimeInit + (encoderPosCount * 60000));
          }
        }
        else
        {
          playerRightTimeInit = (playerRightTimeInit + (encoderPosCount * 60000));
        }
        encoderPosCount = 0;
      }

      if(currentRotaryButton != previousRotaryButton)
      {
        previousRotaryButton = currentRotaryButton;
        if(!currentRotaryButton) //rising edge
        {
          state = SetTimeRight;
        }
      }

      if(flashToggle)
      {
        displayClockHFormat(playerLeftTimeInit, playerRightTimeInit);
      }
      else
      {
        int i;
        for(i = 4; i < 8; i++)
        {
          max7219.DisplayChar(i, ' ', 0);
        }
      }
      break;

    case SetTimeRight:
      if(encoderPosCount)
      {
        settingEncoderLong = encoderPosCount * 60000;
        if(settingEncoderLong < 0)
        {
          settingEncoderULong = -settingEncoderLong;
          if(playerLeftTimeInit <= settingEncoderULong)
          {
            playerLeftTimeInit = 60000;
          }
          else
          {
            playerLeftTimeInit = (playerLeftTimeInit + (encoderPosCount * 60000));
          }
        }
        else
        {
          playerLeftTimeInit = (playerLeftTimeInit + (encoderPosCount * 60000));
        }
        encoderPosCount = 0;
      }
      if(currentRotaryButton != previousRotaryButton)
      {
        previousRotaryButton = currentRotaryButton;
        if(!currentRotaryButton) //rising edge
        {
          state = SetIncLeft;
        }
      }

      if(flashToggle)
      {
        displayClockHFormat(playerLeftTimeInit, playerRightTimeInit);
      }
      else
      {
        int i;
        for(i = 0; i < 4; i++)
        {
          max7219.DisplayChar(i, ' ', 0);
        }
      }
      break;
    case SetIncLeft:
      if(encoderPosCount)
      {
        settingEncoderLong = encoderPosCount * 1000;
        if(settingEncoderLong < 0)
        {
          settingEncoderULong = -settingEncoderLong;
          if(playerRightInc < settingEncoderULong)
          {
            playerRightInc = 0;
          }
          else
          {
            playerRightInc = (playerRightInc + (encoderPosCount * 1000));
          }
        }
        else
        {
          playerRightInc = (playerRightInc + (encoderPosCount * 1000));
        }
        encoderPosCount = 0;
      }

      if(flashToggle)
      {
        displayClock(playerLeftInc, playerRightInc);
      }
      else
      {
        int i;
        for(i = 4; i < 8; i++)
        {
          max7219.DisplayChar(i, ' ', 0);
        }
      }

      if(currentRotaryButton != previousRotaryButton)
      {
        previousRotaryButton = currentRotaryButton;
        if(!currentRotaryButton) //rising edge
        {
          state = SetIncRight;
        }
      }
      break;

    case SetIncRight:
      if(encoderPosCount)
      {
        settingEncoderLong = encoderPosCount * 1000;
        if(settingEncoderLong < 0)
        {
          settingEncoderULong = -settingEncoderLong;
          if(playerLeftInc < settingEncoderULong)
          {
            playerLeftInc = 0;
          }
          else
          {
            playerLeftInc = (playerLeftInc + (encoderPosCount * 1000));
          }
        }
        else
        {
          playerLeftInc = (playerRightInc + (encoderPosCount * 1000));
        }
        encoderPosCount = 0;
      }
      

      if(flashToggle)
      {
        displayClock(playerLeftInc, playerRightInc);
      }
      else
      {
        int i;
        for(i = 0; i < 4; i++)
        {
          max7219.DisplayChar(i, ' ', 0);
        }
      }

      if(currentRotaryButton != previousRotaryButton)
      {
        previousRotaryButton = currentRotaryButton;
        if(!currentRotaryButton) //rising edge
        {
          playerLeftTime = playerLeftTimeInit;
          playerRightTime = playerRightTimeInit;
          state = Ready;
        }
      }
      break;
  }
  if((currentMillis - previousBlinkMillis) > 300)
  {
    previousBlinkMillis = currentMillis;
    flashToggle = !flashToggle;
  }
}


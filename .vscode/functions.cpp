#include "functions.h"


void rotaryUpdate(void)
{
    int temp = digitalRead(rotaryCLKPin);
    if(digitalRead(rotaryBPin) != temp)
        encoderPosCount++;
    else
        encoderPosCount--;

    lastRotaryCLKState = temp;
    intFlag = 1;
 }

void diplayClock(unsigned long timeLeft, unsigned long timeRight)
 {  
    unsigned long hours = timeLeft / (60 * 60000);
    unsigned long minutes = ((timeLeft / 1000) - (3600 * hours)) / 60; 
    unsigned long seconds = ((timeLeft / 1000) - (3600 * hours) - (minutes * 60));
    unsigned long ms = (timeLeft - (3600000 * hours) - (60000 * minutes) - (seconds * 1000)) / 10;


    if(hours > 0)
    {
        max7219.DisplayChar(4, 48 + hours, 0);          //48 is ASCII value for 0
        max7219.DisplayChar(5, 'h', 0);                 //48 is ASCII value for 0
        max7219.DisplayChar(6, 48 + (minutes / 10), 0); //48 is ASCII value for 0
        max7219.DisplayChar(7, 48 + (minutes % 10), 0); //48 is ASCII value for 0
    }
    else if(minutes > 0)
    {   
        //Display minutes + seconde
        max7219.DisplayChar(4, 48 + (minutes / 10), 0);         //48 is ASCII value for 0
        max7219.DisplayChar(5, 48 + (minutes % 10), 1);         //48 is ASCII value for 0
        max7219.DisplayChar(6, 48 + (seconds / 10), 0);         //48 is ASCII value for 0
        max7219.DisplayChar(7, 48 + (seconds % 10), 0);         //48 is ASCII value for 0 

    }
    else
    {
        //display second + ms
        max7219.DisplayChar(4, 48 + (seconds / 10), 0);         //48 is ASCII value for 0
        max7219.DisplayChar(5, 48 + (seconds % 10), 1);         //48 is ASCII value for 0
        max7219.DisplayChar(6, 48 + (ms / 100), 0);         //48 is ASCII value for 0
        max7219.DisplayChar(7, 48 + (ms % 100), 0);         //48 is ASCII value for 0 
    }

    hours = timeRight / (60 * 60000);
    minutes = ((timeRight / 1000) - (3600 * hours)) / 60; 
    seconds = ((timeRight / 1000) - (3600 * hours) - (minutes * 60));
    ms = (timeRight - (3600000 * hours) - (60000 * minutes) - (seconds * 1000)) / 10;

    if(hours > 0)
    {
        max7219.DisplayChar(0, 48 + hours, 0);          //48 is ASCII value for 0
        max7219.DisplayChar(1, 'h', 0);                 //48 is ASCII value for 0
        max7219.DisplayChar(2, 48 + (minutes / 10), 0); //48 is ASCII value for 0
        max7219.DisplayChar(3, 48 + (minutes % 10), 0); //48 is ASCII value for 0
    }
    else if(minutes > 0)
    {   
        //Display minutes + seconde
        max7219.DisplayChar(0, 48 + (minutes / 10), 0);         //48 is ASCII value for 0
        max7219.DisplayChar(1, 48 + (minutes % 10), 1);         //48 is ASCII value for 0
        max7219.DisplayChar(2, 48 + (seconds / 10), 0);         //48 is ASCII value for 0
        max7219.DisplayChar(3, 48 + (seconds % 10), 0);         //48 is ASCII value for 0 
    }
    else
    {
        //display second + ms
        max7219.DisplayChar(0, 48 + (seconds / 10), 0);         //48 is ASCII value for 0
        max7219.DisplayChar(1, 48 + (seconds % 10), 1);         //48 is ASCII value for 0
        max7219.DisplayChar(2, 48 + (ms / 100), 0);         //48 is ASCII value for 0
        max7219.DisplayChar(3, 48 + (ms % 100), 0);         //48 is ASCII value for 0 
    }
 }

 bool debounce(int btn, uint16_t *state) {
  *state = (*state<<1) | digitalRead(btn) | 0xfe00;
  return (*state == 0xff00);
}

bool debounce2(int btn, bool state)
{
  bool stateNow = digitalRead(btn);
  if(state!=stateNow)
  {
    delay(10);
    stateNow = digitalRead(btn);
  }
  return stateNow;
  
}


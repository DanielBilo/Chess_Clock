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
    unsigned long hours = timeLeft / (60 * 60 * 1000);
    unsigned long minutes = ((timeLeft / 1000) - (3600 * hours) / 60); 
    unsigned long seconds = ((timeLeft / 1000) - (3600 * hours) - (minutes * 60));
    unsigned long ms = (timeLeft - (3600000 * hours) - (60000 * minutes) - (seconds * 1000)) / 10;

    if(hours > 0)
    {
        //display hours + minutes
    }
    else if(minutes > 0)
    {
        //display minutes + seconds 
    }
    else
    {
        //display second + ms
    }

    hours = timeRight / (60 * 60 * 1000);
    minutes = ((timeRight / 1000) - (3600 * hours) / 60); 
    seconds = ((timeRight / 1000) - (3600 * hours) - (minutes * 60));
    ms = (timeRight - (3600000 * hours) - (60000 * minutes) - (seconds * 1000)) / 10;

    if(hours > 0)
    {
        //display hours + minutes
    }
    else if(minutes > 0)
    {
        //display minutes + seconds 
    }
    else
    {
        //display second + ms
    }



 }
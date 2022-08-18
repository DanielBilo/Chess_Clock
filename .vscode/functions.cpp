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

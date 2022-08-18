#ifndef functions_h
#define functions_h

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif


extern volatile bool lastRotaryCLKState;
extern int encoderPosCount;
extern int rotaryCLKPin;         
extern int rotaryBPin;           //Active low
extern bool intFlag;

void rotaryUpdate(void);

#endif
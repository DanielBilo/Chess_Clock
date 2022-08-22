#ifndef functions_h
#define functions_h

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
#include <max7219.h>


extern volatile bool lastRotaryCLKState;
extern int encoderPosCount;
extern int rotaryCLKPin;         
extern int rotaryBPin;           //Active low
extern bool intFlag;
extern MAX7219 max7219;

void rotaryUpdate(void);
void diplayClock(unsigned long timeLeft, unsigned long timeRight);
bool debounce(int btn, uint16_t *state);


#endif
#include <max7219.h>

MAX7219 max7219;

void setup() {
  Serial.begin(9600);
  max7219.Begin();
  pinMode(13, OUTPUT);
  max7219.MAX7219_SetBrightness(1);
}

void loop() {
  String temp;
  char temp2[8];
  int y;

  //Display Char Demo
  max7219.Clear();
  max7219.DisplayChar(7, 'H', 0); //Position 7 is on the left of the display
  delay(500);
  
  max7219.DisplayChar(6, 'E', 0);
  delay(500);
  max7219.DisplayChar(5, 'L', 0);
  delay(500);
  max7219.DisplayChar(4, 'L', 0);
  delay(500);
  max7219.DisplayChar(3, 'O', 0);
  delay(500);
  max7219.DisplayChar(2, '1', 0);
  delay(500);
  max7219.DisplayChar(1, '2', 0);
  delay(500);
  max7219.DisplayChar(0, '3', 0);
  delay(500);
  max7219.Clear();
  
  digitalWrite(13,!digitalRead(13));
  //hello

}
#include <Wire.h>
#include "src/EEP.h"

//EEPROM ERASER

void setup(){
  byte mystr0[16] = "XX22XX33XX44XX55";
  int adress = 0x000;
  
  Wire.begin();
  Serial.begin(9600);

  for(int i = 0; i < 128; i++)
  {
    Serial.print("Writing to: ");
    Serial.println(adress);
    EEP.write(adress, mystr0, sizeof(mystr0));
    adress = adress + 16;
    
    delay(10);
  }
}

void loop()
{
}

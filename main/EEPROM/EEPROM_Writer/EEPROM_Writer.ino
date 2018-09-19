#include <Wire.h>
#include "src/EEP.h"

//EEPROM ERASER

void setup(){
  byte mystr0[16] = "XXXXXXXXXXXXXXXX";
  int adress = 0x000;
  
  Wire.begin();
  Serial.begin(9600);

  for(int i = 0; i < 128; i++)
  {
    Serial.println("Writing to 0x000");
    EEP.write(adress, mystr0, sizeof(mystr0));
    adress = adress + 16;
    
    delay(10);
  }
}

void loop()
{
}



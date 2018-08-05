#include <Wire.h>
#include "src/EEP.h"

void setup(){
  Wire.begin();
  Serial.begin(9600); 
  //MyEEPROM.write(0x000, 'a', 1);
}

void loop()
{
  char buffer[1];
  int adress = 0x000;
  for(int i = 0; i < 2048; ++i)
  {
    if (i%16==0)
      Serial.println();
      
    if (i%256==0)
        Serial.println("Reading from Block: " + String(i/256));

    MyEEPROM.read(adress, buffer, 1);
    Serial.print(String(buffer));
    ++adress; 
    
    delay(10);
  }
  delay(999999);
}

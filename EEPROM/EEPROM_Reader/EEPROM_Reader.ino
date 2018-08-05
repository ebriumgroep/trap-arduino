#include <Wire.h>
#include <MyEEPROM.h>

void setup(){
  Wire.begin();
  Serial.begin(9600); 
  //MyEEPROM.write(0x000, 'a', 1);
}

void loop(){
  char buffer[1];
  int adress = 0x000;
  for (int i = 0; i < 8; ++i){
    Serial.println("Reading from Block: " + String(i));
    for (int j = 0; j < 256; ++j){
      MyEEPROM.read(adress, buffer, 1);
      Serial.print(String(buffer));
      ++adress;
      if (j%16==0)
        Serial.println();
    }
    delay(100);
  }

  delay(999999);
}

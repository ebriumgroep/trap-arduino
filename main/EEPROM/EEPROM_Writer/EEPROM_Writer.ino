#include <Wire.h>
#include "src/EEP.h"

void setup(){
  byte mystr0[16] = "2018/02/21,03:22";
  byte mystr1[16] = "2018/02/21,03:99";
  
  Wire.begin();
  Serial.begin(9600);
  
  Serial.println("Writing to 0x000");
  EEP.write(0x000, mystr0, sizeof(mystr0));

  Serial.println("Writing to 0x010");
  EEP.write(0x010, mystr1, sizeof(mystr1));

 
}

void loop(){

}



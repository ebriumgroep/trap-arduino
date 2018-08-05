#include <Wire.h>
#include <MyEEPROM.h>



void setup(){
  byte mystr0[16] = "2018/02/21,03:34";
  byte mystr1[16] = "2018/02/21,03:34";
  
  Wire.begin();
  Serial.begin(9600);
  
  Serial.println("Writing to 0x000");
  MyEEPROM.write(0x000, mystr0, sizeof(mystr0));

  Serial.println("Writing to 0x010");
  MyEEPROM.write(0x010, mystr1, sizeof(mystr1));

 
}

void loop(){

}

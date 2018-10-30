#include "src/EEP.h"
#include <Wire.h>

void setup()
{
    Serial.begin(9600);
    Wire.begin();

    pinMode(11, OUTPUT);
    pinMode(12, OUTPUT);

    delay(500);

    digitalWrite(11, HIGH);
    delay(500);
    digitalWrite(12, LOW);
    delay(500);

    char data[6];
    memset(data, 0, 6);

    EEP.write(0x000, "TEST1", 5);
    EEP.read(0x000, data, 5);

    if (strstr(data, "TEST1") != NULL) {
        Serial.println(F("Read success: EEPROM 1"));
    } else {
        Serial.println(F("Read failure: EEPROM 1"));
    }

    digitalWrite(11, LOW);
    delay(500);
    digitalWrite(12, HIGH);
    delay(500);

    memset(data, 0, 6);

    EEP.write(0x000, "TEST2", 5);
    EEP.read(0x000, data, 5);

    if (strstr(data, "TEST2") != NULL) {
        Serial.println(F("Read success: EEPROM 2"));
    } else {
        Serial.println(F("Read failure: EEPROM 2"));
    }

}

void loop() {}

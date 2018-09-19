//Import our Libraries
#include "src/DHT.h"    // Temperature and Humidity Sensor
#include "src/GSM.h"    // GSM Modem
#include "src/Time.h"   // Time
#include "src/GF.h"     // General Functions
#include "src/EEP.h"    // Eeprom (External)
#include <Wire.h>

using namespace Funcs;

// gsm initial values
char adrs [] = "http://erbium.requestcatcher.com/test";
GSM gsm(2, 3);
DHT dht(4, 22);

// Has the modem been started
bool started = false;

// eeprom hex adresses
int addressMW = 0x000;
int addressMR = 0x000;
int addressTW = 0x000;
int addressTR = 0x000;

//eeprom comparison bool
bool AddressWLower = false;

//buffer length const
int buflen = 16;
int dBuflen = 32;

// Sensor Constants and Variables
const int SENSOR = 6, AMODEM = 7, EEPMOTH = 10, EEPTEMP = 11;

// Temp
int trigger[2] = {1,1};
int completed[2] = {1,0};

void setup()
{
  // Starts the Serial connection for debugging
  Serial.begin(9600);

  // Starts the temperature sensor
  dht.begin();

  // Starts the i2c bus
  Wire.begin();

  // Initialises the input pins
  pinMode(SENSOR, INPUT);
  pinMode(AMODEM, OUTPUT);
  pinMode(EEPMOTH, OUTPUT);
  pinMode(EEPTEMP, OUTPUT);
}

void moth_sense()
{
  bool state = false;
  state = digitalRead(SENSOR);
  if (!state)
  {
    Serial.println("FOUND");                          // Afvoer as die program 'n mot vang
    byte mystr[16];
    dtostrf(now(), 16, 0, mystr);
    EEP.write(addressMW, mystr, sizeof(mystr));
    addressMW += 16;
    if (addressMW > 2048)
      addressMW = 0;
    delay(1000);
   }
}

void temp_sense()
{
    Serial.println("SENSING_TEMP");
          
    float t = dht.readTemperature();
    char tStr[6];
    Serial.println(t);
          
    byte strTemp[32];
    dtostrf(now(), 16, 0, strTemp);
    strTemp[16] = ',';
    floatToChar(t, tStr);
    for (int i = 0; i < 6 && i < sizeof(strTemp); i++)
    {
      strTemp[i + 17] = tStr[i];
    }
    digitalWrite(EEPTEMP, 1);
    EEP.write(addressTW, strTemp, sizeof(strTemp));
    addressTW += 32;
    if (addressTW > 2048)                               //2432
      addressTW = 0;   
}

void transmitRun()
{
    Serial.println("IN TRANSMISSION");                   // Afvoer as die program begin om data te stuur
    while (!started)
    {
      started = gsm.start();
    }
    bool posted = false;
    while (!posted)
    {
      if (gsm.postRequest())
      {
        if (gsm.readRequest())
        {
          String ans = gsm.getAnswer();
          Serial.println("This is the output from the POST request");
          Serial.println(ans);
          if (ans = "Request caught")
          {
            posted = true;
            Serial.println("Done with transmission.");
          }
          else
          {
            Serial.println("Error in transmission, retrying...");
          }
        }
      }
    }
}

void transmit_moth()
{
   Serial.println("TRANSMITTING: MOTH_COUNT");    // Afvoer as die program begin om data te stuur oor motte
  
   char output[17];                       // Transmission variable
   output[16] = '\0';                     // Null termination for transmission

   // Enkel bit veranderlike
   char buffer[1];  

   for(int h = 0; h < 5; ++h)             // Die "5" verteenwoordig die aantal lyne wat die program moet transmit. Dit is tydelik en moet verander word sodra ons data van die server kan kry
   {    
      for(int i = 0; i < buflen; ++i)         // Lees een lyn van 16 bits
      {
        EEP.read(addressMR, buffer, 1);
        ++addressMR; 

        if (buffer[0] != ' ')             // Vervang lee waardes met 'n nul
        {
          output[i] = buffer[0];
        }
        else
        {
          output[i] = '0';
        }
      }
      // Testing
      Serial.print("Data: ");
      Serial.println(output);

      gsm.setMessage(output, false);
      transmitRun(); 

      Serial.println("Done............");
      delay(500);
   }  
}

void transmit_temp()
{
   Serial.println("TRANSMITTING: TEMPERATURE");    // Afvoer as die program begin om data te stuur oor motte

   char output[17];                       // Transmission variable
   output[16] = '\0';                     // Null termination for transmission

   // Enkel bit veranderlike
   char buffer[1];  

   for(int h = 0; h < 5; ++h)             // Die "5" verteenwoordig die aantal lyne wat die program moet transmit. Dit is tydelik en moet verander word sodra ons data van die server kan kry
   {    
      for(int i = 0; i < 16; ++i)         // Lees een lyn van 16 bits
      {
        EEP.read(addressTR, buffer, 1);
        ++addressTR; 

        if (buffer[0] != ' ')             // Vervang lee waardes met 'n nul
        {
          output[i] = buffer[0];
        }
        else
        {
          output[i] = '0';
        }
      }
      // Testing
      Serial.print("Data: ");
      Serial.println(output);

      gsm.setMessage(output, false);
      transmitRun(); 

      Serial.println("Done............");
      delay(1000);
   }  
}

void loop()
{
  moth_sense();
  
  if(((now() % (trigger[0]*(3600/60))) == 0) && (completed[0] == 0))
  {
    temp_sense();
    completed[0] = 1;
  }

  if(((now() % (trigger[1]*(3600/60))) == 0) && (completed[1] == 0))
  {
    transmit_moth();
    transmit_temp();
    
    Serial.println();
    Serial.println("DATA TRANSMISSION COMPLETED!");
    
    completed[1] = 1;
  }
}


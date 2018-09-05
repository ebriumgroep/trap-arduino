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
char msg [] = "Startup Message";

// eeprom hex adresses
int addressW = 0;
int addressR = 0;

//eeprom comparison bool
bool AddressWLower = false;

GSM gsm(2, 3, adrs, msg);
DHT dht(4, 22);

// The state of the system
enum state
{
  SENSING_GENR, // Sensing General
  SENSING_TEMP, // Sensing Temperature
  TRANSMITTING,  // Transmitting
  TRANS_GENERL,  
  TRANS_TEMPRA
};
state control = SENSING_GENR;

// Sensor Constants and Variables
const int SENSOR = 6, AMODEM = 7;

// This should all be dynamically allocated in the setup function from the settings textfile
// Only for current testing purposes
int arrReadT[] =  {1, 3, 5, 7, 9 };
int arrReadC[] =  {0, 0, 0, 0, 0 };
int arrTranD[] =  {2, 4, 6, 8, 10};
int arrTranC[] =  {0, 0, 0, 0, 0 };

// Pointers
int *start;

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

  // Initialises the day it was started
  start = new int;
  *start = day();
}

void loop()
{
  // Is it time to sense for temperature
  for (int a = 0; a < 5; a++)
  {
    if ((minute() == arrReadT[a]) and (arrReadC[a] == 0))
    {
      control = SENSING_TEMP;
      arrReadC[a] = 1;
      break;
    }
  }

  // Is it time to transmit data
  for (int a = 0; a < 5; a++)
  {
    if ((minute() == arrTranD[a]) and (arrTranC[a] == 0))
    {
      control = TRANSMITTING;
      arrTranC[a] = 1;
      break;
    }
  }

  // Execute the tasks
  switch (control)
  {
    case TRANS_GENERL:
    {
      int sta = 0, sto = 5, adr = 0;
      char buf[1] = {'0'};
      
      for(int a=sta; a<sto; ++a)
      {
        EEP.read(adr, buf, 1);  
      }
      
    }
    case TRANS_TEMPRA:
    {
      //
    }
    // False Codling Moths Sensor
    case SENSING_GENR:
      {     
        bool state = false;
        state = digitalRead(SENSOR);
        if (!state)
        {
          Serial.println("found");
          byte mystr[16];
          dtostrf(now(), 16, 0, mystr);
          EEP.write(addressW, mystr, sizeof(mystr));
          addressW += 16;
          if (addressW > 2432)
            addressW = 0;
          delay(1000);
        }
        break;
      }
    case SENSING_TEMP:
      {
        Serial.println("SENSING_TEMP");
        
        float t = dht.readTemperature();
        //float h = dht.readHumidity();

        char tem[8] = {0};
        dtostrf(t, 6, 2, tem);

        //char hum[8] = {0};
        //dtostrf(h, 6, 2, hum);

        gsm.setMessage(tem);
        
        control = SENSING_GENR;
        break;
      }
    case TRANSMITTING:
      {
        Serial.println("TRANSMITTING");
        
        bool started = false;
        while (!started)
        {
          started = gsm.start();
        }
        
        if (gsm.postRequest())
        {
          if (gsm.readRequest())
          {
            String ans = gsm.getAnswer();
            Serial.println("This is the output from the POST request");
            Serial.println(ans);
            Serial.println("Done....");

            
          }
          control = SENSING_GENR;
        }
        
        gsm.gsmOff();
        break;
      }
  }

  if (day() != *start)
  {
    *start = day();
    for (int a = 0; a < 5; a++)
      arrReadC[a] = 0;
    for (int a = 0; a < 1; a++)
      arrTranC[a] = 0;
  }
}


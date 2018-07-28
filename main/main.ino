//Import our Libraries
#include "src/DHT.h"    // Temperature and Humidity Sensor
#include "src/GSM.h"    // GSM Modem
#include "src/Time.h"   // Time
//#include "src/TEX.h"    // Text
#include "src/GF.h"     // General Functions
#include "src/SDCOM.h"    //
using namespace Funcs;

char adrs [] = "http://webhook.site/ee4798db-b937-4c84-8973-6ffc0687eabc";
char msg [] = "Startup Message";

GSM gsm(2, 3, adrs, msg);
DHT dht(4, 22);

// The state of the system
enum state
{
  SENSING_GENR, // Sensing General
  SENSING_TEMP, // Sensing Temperature and Humidity
  TRANSMITTING  // Transmitting Data
};
state control = SENSING_GENR;

// Sensor Constants and Variables
const int SENSOR = 6, AMODEM = 7;

// This should all be dynamically allocated in the setup function from the settings textfile
// Only for current testing purposes
int arrReadT[] =  {2, 4, 6, 8, 10};
int arrReadC[] =  {0, 0, 0, 0, 0 };
int arrTranD[] =  {1, 3, 5, 7, 9 };
int arrTranC[] =  {0, 0, 0, 0, 0 };

// Pointers
int *start;

// Files
//File *temphum, *counter;
SDCOM sdCom(8);

void setup()
{
  // Starts the Serial connection for debugging
  Serial.begin(9600);

  /*if (!SD.begin(8)) {
    while (1);
  }*/

  // Starts the temperature sensor
  dht.begin();

  // Initialises the input pins
  pinMode(SENSOR, INPUT);
  pinMode(AMODEM, OUTPUT);

  // Initialises the day it was started
  start = new int;
  *start = day();

  // Start the GSM modem
  while (!gsm.start())
  {
      // Sends an test signal on startup
      gsm.postRequest();
  }

  // Turn the GSM modem off
  gsm.gsmOff();
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
    // False Codling Moths Sensor
    case SENSING_GENR:
      {
        bool state = false;
        state = digitalRead(SENSOR);
        if (!state)
        {
          Serial.println("found");    // -> Save the state to file (See the Design Requirement Manual)
          //counter = new File;
          /**counter = SD.open("c", FILE_WRITE);
          
          if (*counter)
          {
            counter->println(String(1));
            counter->close();
          }
          delete counter;*/
          String mes = "found";
          sdCom.write(mes,100008);
        }
        delay(100);
        break;
      }
    case SENSING_TEMP:
      {
        // Read the temperature/humidity and assign it to the GSM modem
        float t = dht.readTemperature();
        float h = dht.readHumidity();
        char* mes = &String(String(t) + String(",") + String(h))[0];
        //String *mes = new String(String(t) + String(",") + String(h));
        gsm.setMessage(mes);
        Serial.println(mes); // -> Save the state to file (See the Design Requirement Manual)
        /*temphum = new File;
        *temphum = SD.open("t", FILE_WRITE);
        if (*temphum)
        {
          temphum->println(String(t) + String(",") + String(h));
          temphum->close();
        }
        delete temphum;*/
        
        //sdCom.write(mes, 100008);

        control = SENSING_GENR;
        break;
      }
    case TRANSMITTING:
      {
        bool started = false;
        while (!started)
        {
          started = gsm.start();
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

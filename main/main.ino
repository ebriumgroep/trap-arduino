//Import our Libraries
#include "src/dht_library/DHT.h"    // Temperature and Humidity Sensor
#include "src/gsm_library/GSM.h"    // GSM Modem
#include "src/time_library/Time.h"  // Time

#include "src/tex_library/TEX.h"    // For text files. (Under development)

GSM *gsm;
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
TEX *THFile = new TEX("THFile.txt");
TEX *mothCountFile = new TEX("MotCountFile.txt");
void setup()
{
  // Starts the Serial connection for debugging
  Serial.begin(9600);
  
  // Starts the temperature sensor
  dht.begin();
  
  // Initialises the input pins
  pinMode(SENSOR, INPUT);
  pinMode(AMODEM, OUTPUT);

  // Initialises the day it was started
  start = new int;
  *start = day();
  // Sends an test signal on startup
  gsm = new GSM(2, 3);
  bool *started = new bool;
  *started = false;
  while(!(*started))
  {
  	 if (gsm->start())
  	 {
  		 gsm->setAddress("http://erbium.requestcatcher.com/test");
  		 gsm->setMessage("Everything is Working");
  		 if (gsm->postRequest())
  		 {
         *started = true;
  		 }
  	 }
  }
  delete started;
  delete gsm;
}

void loop()
{     
  // Is it time to sense for temperature
  for(int a=0; a<5; a++)
  {
    if((minute() == arrReadT[a]) and (arrReadC[a] == 0))
    {
      control = SENSING_TEMP;
      arrReadC[a] = 1;
      break;  
    }
  }

  // Is it time to transmit data
  for(int a=0; a<5; a++)
  {
    if((minute() == arrTranD[a]) and (arrTranC[a] == 0))
    {
      control = TRANSMITTING;
      arrTranC[a] = 1;
      break;
    }
  }

  // Execute the tasks
  switch(control)
  {
    // False Codling Moths Sensor
    case SENSING_GENR:  
    {
      bool state = false;
      state = digitalRead(SENSOR);
      if(state)
      {
        Serial.println("found");    // -> Save the state to file (See the Design Requirement Manual)
        mothCountFile->append(String(year()) + "/" + String(month()) + "/" + String(day()) + "," + String(hour()) + ":" + String(minute()) + "," + "1");
        delay(2000);
      }
      delay(1000);
      break;
    }
    case SENSING_TEMP:
    {
      // Read the temperature/humidity and assign it to the GSM modem
      float t = dht.readTemperature();
      float h = dht.readHumidity();

      Serial.println(String(t)+String(",")+String(h));  // -> Save the state to file (See the Design Requirement Manual)
      THFile->append(String(year()) + "/" + String(month()) + "/" + String(day()) + "," + String(hour()) + ":" + String(minute()) + "," + "1" + String(t)+ "," + String(h));
      
      control = SENSING_GENR;
      break;
    }
    case TRANSMITTING:
    {
      // Start the GSM modem again
      gsm = new GSM(2, 3);
      
      bool *started = new bool;
      *started = false;
      while(!(*started))
      {
        // Starts the GSM modem
        if (gsm->start())
        {
          gsm->setAddress("http://erbium.requestcatcher.com/test");
          gsm->setMessage(String("Test Data")); // -> This should be read from the textfiles saved earlier (See the Design Requirement Manual)
          
          //Sending the False Codling Moths(FSM) data
          mothCountFile->openRM();
          String message = mothCountFile->readln();
          while (message == "~")
          {
              message = mothCountFile->readln();
          }
          gsm->setMessage(message);  
          mothCountFile->closeRM();
          
          //Sending the temperature and humidity data
          THFile->openRM();
          message = THFile->readln();
          while (message == "~")
          {
              message = THFile->readln();
          }
          gsm->setMessage(message);  
          THFile->closeRM();
          *started = true;
        }
      }
      // Use the GSM modem to post the temperature to the server
      if (gsm->postRequest())
      {
        // Reads the returned data from the post request to determine if the transmission was successful
        if(true)
          control = SENSING_GENR;
        else
          control = TRANSMITTING;
      }
      else
        control = TRANSMITTING; 
         
      // Delete GSM object to save memory
      delete started;
      delete gsm;
      //delete dataFile;
      break;     
    }
  }

  if(day() != *start)
  {   
    *start = day();
    for(int a=0; a<5; a++)
      arrReadC[a] = 0;
    for(int a=0; a<1; a++)
      arrTranC[a] = 0;
  }
}

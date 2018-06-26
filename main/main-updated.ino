//Import our Libraries
#include "src/dht_library/DHT.h"    // Temperature and Humidity Sensor
#include "src/gsm_library/GSM.h"    // GSM Modem

// Import Temprorary Standard Libraries
#include <Time.h>

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

// Sensorr Constants
const int SENSOR = 6;

// Variables
int start = day();

// This should all be dynamically allocated in the setup function from the settings textfile
// Only for current testing purposes
int arrReadTemp[] = {1, 2, 3, 4, 5};
int arrReadCount[] = {0, 0, 0, 0, 0};
int arrTransData[] = {10};
int arrTransCount[] = {0};

void setup()
{
	// Starts the Serial connection for debugging
	Serial.begin(9600);

  // Points the GSM pointer to the GSM object
  gsm = new GSM(2, 3);

  // Initialises the input pins
  pinMode(SENSOR, INPUT);

	// Starts the temperature sensor
	dht.begin();

	// Starts the GSM modem
	if (gsm->start())
	{
		// Initialize the test post address and message. This may be changed throughout the run of the program in the void loop function.
		gsm->setAddress("http://erbium.requestcatcher.com/test");
		gsm->setMessage("Everything is Working");
		if (gsm->postRequest())
		{
			Serial.println("Startup Successful");
		}
	}
}

void loop()
{ 
  // The current minute for debugging
  Serial.println(String("Current Minute: ")+String(minute()));
  
  // Is it time to sense for temperature
  for(int a=0; a<5; a++)
  {
    if((minute() == arrReadTemp[a]) and (arrReadCount[a] == 0))
    {
      Serial.println("T&H Active");
      control = SENSING_TEMP;
      arrReadCount[a] = 1;
      break;  
    }
  }

  // Is it time to transmit data
  for(int a=0; a<1; a++)
  {
    if((minute() == arrTransData[a]) and (arrTransCount[a] == 0))
    {
      Serial.println("Transmit Active");
      control = TRANSMITTING;
      arrTransCount[a] = 1;
      break;
    }
  }

  // Execute the tasks
  switch(control)
  {
    case SENSING_GENR:  
    {
      Serial.println("SENSING_GENR");
      
      bool state = false;
      state = digitalRead(SENSOR);
      if(state)
      {
        delay(2000);
        // -> To Text File 
      }

      // Delay the sensor for debugging
      delay(1000);
      
      break;
    }
    case SENSING_TEMP:
    {
      Serial.println("SENSING_TEMP");
      
      // Read the temperature/humidity and assign it to the GSM modem
      float t = dht.readTemperature();
      float h = dht.readHumidity();
      
      // -> To Text File
      
      control = SENSING_GENR;
      break;
    }
    case TRANSMITTING:
    {
      Serial.println("TRANSMITTING");
      
      // This will be in an while loop reading from the entries in the files.
      String data = "test data";
      int hash = gsm->hash(data);

      gsm->setMessage(String(hash)+String(",")+String(data));

      // Use the GSM modem to post the temperature to the server
      if (gsm->postRequest())
      {
        // Reads the returned data from the post request to determine if the transmission was sucsessfull
        if(true)
        {
          Serial.println("Data Successfully Transmitted");
          control = SENSING_GENR;
        }
        else
        {
          Serial.println("Something went wrong in transmission, trying again");
          control = TRANSMITTING;
        }
      }
      break;     
    }
  }

  if(day() != start)
  {
    
    Serial.println("Counters Reset");
    
    start = day();
    for(int a=0; a<5; a++)
    {
      arrReadCount[a] = 0;
    }
    for(int a=0; a<1; a++)
    {
      arrTransCount[a] = 0;
    }
  }
}
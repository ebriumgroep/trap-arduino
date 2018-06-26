//Import our libraries
#include "src/dht_library/DHT.h"
#include "src/gsm_library/GSM.h"

GSM *gsm;
DHT dht(4, 22);

void setup()
{
	// Starts the Serial port for debugging
	Serial.begin(9600);

	// Start the GSM modem
  gsm = new GSM(2, 3);

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
	
	// Delete GSM object to save memory
	delete gsm;
}

void loop()
{
	// Wait for the sensor
	delay(2000);
	
	// Start the GSM modem again
	gsm = new GSM(2, 3);

	// Read the temperature and assign it to the GSM modem
	float t = dht.readTemperature();
  float h = dht.readHumidity();
  
  String data = String("Date,")+String("Time,")+String("Trap-ID,")+String(t)+String(",")+String(h);
  int hash = gsm->hash(data);

	gsm->setMessage(data);  // Compare the returned hash with the locally generated hash.

	// Use the GSM modem to post the temperature to the server
	if (gsm->postRequest())
	{
		Serial.println("Temperature Posted Successfully");
	}
	
	// Delete GSM object to save memory
	delete gsm;
}

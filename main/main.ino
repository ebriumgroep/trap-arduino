#include "src/DHT.h"    // Temperature and Humidity Sensor
#include "src/GSMModule.h"    // GSM Modem
#include "src/Time.h"   // Time
#include "src/GF.h"     // General Functions
#include "src/EEP.h"    // Eeprom (External)
#include <Wire.h>

#define DEVICE_ID 1

/** 
 * For production, printing should be removed.
 * To remove printing through serial, comment the line
 * #define DEBUG. 
 * To restore serial printing uncomment #define DEBUG
 **/
//#define DEBUG

#ifndef DEBUG
    #define Debugln(a) 
    #define Debug(a) 
#else
    #define Debugln(a) Serial.println(a)
    #define Debug(a) Serial.print(a)
#endif

using namespace Funcs;

GSMModule gsm(2, 3);
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

// Sensor Constants and Variables
const byte SENSOR = 6, AMODEM = 7, EEPMOTH = 10, EEPTEMP = 11;

// Temp
int trigger[2] = {1,1};
int completed[2] = {1,0};

/**
 * Note: The Arduino Uno uses a 32 bit unsigned integer to represent time.
 *       This will overflow in 2038.
 **/

/**
 * Initial setup of the Arduino.
 * Initialise:
 *          Serial connection (for debugging)
 *          Temperature sensor (DHT)
 *          I2C Bus (Wire)
 *          Input pins
 **/
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
//    pinMode(EEPMOTH, OUTPUT);
//    pinMode(EEPTEMP, OUTPUT);
    pinMode(12, OUTPUT); // moth eeprom
    pinMode(11, OUTPUT); // temp eeprom

    //Testing data
    /*
    char mothDataTest[10][19] = {"2015/01/01,08:10,3","2015/02/10,10:30,5",
                                 "2015/03/20,12:20,2","2016/05/11,16:59,3",
                                 "2016/06/30,22:30,5","2016/07/26,13:00,3",
                                 "2017/08/07,13:10,3","2017/09/14,00:17,2",
                                 "2018/10/31,15:15,2","2018/12/26,13:10,5"}; 
                                 */

    // TODO add device setup functionality.
    //deviceSetup();
}

/**
 * Retreive settings and request time.
 **/
void deviceSetup()
{
    char url[20] = "www.placeholder.a";
    bool settings = false;
    while (!settings) {
        bool posted = false;
        while (!gsm.start()) {}

        for (byte i = 0; i < 3; ++i) {
            if (gsm.postRequest(url, "G")) {
                posted = true;
                break;
            }
        }

        if (posted) {
            char* response = gsm.readResponse();

            for (byte k = 0; response != NULL && k < strlen(response); ++k) {
                // TODO loop through the response and set settings
            }
        }
    }
}

/**
 * Concatenate the date to the given parameter
 *
 * Pad the data with zeros (0) such that when the date is added, it will 
 * always 11 bytes long.
 **/
void addDate(char data[], const byte DATA_LEN)
{
    char date[DATA_LEN];
    memset(date, 0, DATA_LEN);

    itoa(now(), date, 10);
    for (byte i = 0; i < DATA_LEN - strlen(date) - 1; ++i) {
        strncat(data, "0", DATA_LEN - strlen(data) - 1);
    }

    strcat(data, date);
}


/**
 * Write the time (amount of seconds from 1 Jan, 1970) to the EEPROM 
 * if a moth is sensed.
 **/
void moth_sense()
{
    const byte DATA_LEN = 12;
    bool state = false;
    state = digitalRead(SENSOR);
    if (!state) {
        Debugln(F("\nMoth Sense"));

        char data[DATA_LEN];
        memset(data, 0, DATA_LEN);

        addDate(data, DATA_LEN);

        Debugln("Date:");
        Debugln(data);

        // Write the data string to the eeprom, with the null 
        // terminating character
        EEP.write(addressMW, data, strlen(data) + 1);

        addressMW += 16;
        if (addressMW > 2048) {
          addressMW = 0;
        }
        delay(1000);
    }
}

/**
 * Write the time (amount of seconds from 1 Jan, 1970) to the EEPROM 
 * with the current temperature.
 **/
void temp_sense()
{
    Debugln(F("\nTemp Sense"));

    const byte DATA_LEN = 19;
    const byte DATE_LEN = 12;
    const byte TEMP_LEN = 7;

    char temperatureStr[TEMP_LEN];
    memset(temperatureStr, 0, TEMP_LEN);

    char data[DATA_LEN];
    memset(data, 0, DATA_LEN);

    float temperature = dht.readTemperature();

    // add the date to the data string
    addDate(data, DATE_LEN);
    
    // concat a comma (',')
    strncat(data, ",", DATA_LEN - strlen(data) - 1);

    dtostrf(temperature, 3, 2, temperatureStr);

    // Pad the temperatureStr with zeros.
    for (byte i = 0; i < 6 - strlen(temperatureStr); ++i) {
        strncat(data, "0", DATA_LEN - strlen(data) - 1);
    }

    strncat(data, temperatureStr, DATA_LEN - strlen(data) - 1);

    Debugln(F("Data:"));
    Debugln(data);

    digitalWrite(EEPTEMP, 1);
    // Write the data string to the eeprom, with the null terminating character
    EEP.write(addressTW, data, strlen(data) + 1);
    //TODO see if a variable size will be possible
    addressTW += 32;
    if (addressTW > 2048) {                          //2432
        addressTW = 0;
    }
}

#define RETRIES 1
/**
 * Starts the GSM module.
 * Post a request, and if successful read the response. 
 * Retry until a successful post request was sent and a 
 * successful response is received.
 **/
void transmitRun()
{
    char url[] = "http://erbium.000webhostapp.com/recordInsect.php";
    Debugln(F("Transmit"));
    while (!gsm.start()) {}
    
    char data[17];
    char device_id[3];
    memset(data, 0, 17);
    memset(device_id, 0, 3);
    addDate(data, 11);
    strncat(data, ",", 16 - strlen(data));
    itoa(DEVICE_ID, device_id, 10);
    strncat(data, device_id, 16 - strlen(data));
    bool posted = false;
    for (byte i = 0; i < RETRIES; ++i) {
        if (gsm.postRequest(url, data)) {
            posted = true;
            break;
        }
    }
    if (!posted) {
        Debugln(F("Failed to post data"));
        return;
    }

    posted = false;
    char* response = NULL;
    response = gsm.readResponse();
    Serial.print(F("Response from server:"));
    Serial.println(response);
    for (byte k = 0; response != NULL && k < strlen(response); ++k) {
        Serial.print(k);
        Serial.print(F(": "));
        Serial.println(response[k]);
    }
    delete [] response;
    if (!posted) {
        Debugln(F("No response received"));
    }
    Debugln(F("Transmit Done"));
}

/**
 * Transmit moth data accumulated in the EEPROM
 *
 * \todo Add functionality to read from the EEPROM. (Will be implemented when 
 *       the Arduino is able to read from two EEPROMs)
 **/
void transmit_moth()
{
   Debugln(F("\nSend moth"));
    char input[19] = "2018/04/26,13:10,5";
    for (byte i = 0; i < 10; i++) {
        //mothDataTest[i][19]
        input[15] = (char)(i + 48);
        //strcpy(input, output.c_str());
        Debug(F("Data: "));
        Debugln(input);
        //gsm.setMessage(input, false);
        delay(10000);
        Debugln(F("Transmit"));
        transmitRun(); 
    }
    Debugln(F("Done............"));
    delay(500);
}

/**
 * Transmit temperature 
 **/
void transmit_temp()
{
    Debugln(F("Send Temp"));

    char output[17];                       // Transmission variable
    memset(output, 0, 17);

    // Enkel bit veranderlike
    char buffer;

    for(byte h = 0; h < 5; ++h)             // Die "5" verteenwoordig die aantal lyne wat die program moet transmit. Dit is tydelik en moet verander word sodra ons data van die server kan kry
    {    
        for(byte i = 0; i < 16; ++i)         // Lees een lyn van 16 bits
        {
            EEP.read(addressTR, buffer, 1);
            ++addressTR; 

            if (buffer != ' ')             // Vervang lee waardes met 'n nul
            {
              output[i] = buffer;
            }
            else
            {
              output[i] = '0';
            }
        }
        // Testing
        Debug(F("Data: "));
        Debugln(output);

        //gsm.setMessage(output, false);
        transmitRun(); 

        Debugln(F("Done............"));
        delay(1000);
    }  
}

/**
 * Loop function periodically called by the Arduino.
 * Transmit amount of moths and the temperature.
 **/
void loop()
{
    moth_sense();

    // kyk vir spesifieke tyd. en sense dan
    if(((now() % (trigger[0]*60)) == 0) && (completed[0] == 0)) {
        temp_sense();
        completed[0] = 1;
    }

    if(((now() % (trigger[1]*60)) == 0) && (completed[1] == 0)) {
        transmit_moth();
        //transmit_temp();

        Debugln(F("\nDATA TRANSMISSION COMPLETED!"));

        completed[1] = 1;
    }
}

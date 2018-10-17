/*
GSM.cpp (formerly gsmTransmitter.ino) -
Multi-instance GSM Transmitter library for Arduino

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

The latest version of this library can always be found at
https://github.com/ebriumgroep/trap-arduino
*/

#include "GSM.h"

#ifndef DEBUG
    #define Debugln(a)
    #define Debug(a)
#else
    #define Debugln(a) Serial.println(a)
    #define Debug(a) Serial.print(a)
#endif    

char* GSM::getAnswer()
{
	return ans;
}

void GSM::setMessage(const char me [], bool db = false)
{
    memset(mes, 0, MESS_LEN);
	strncpy(mes, me, MESS_LEN - 1);
	Debugln(mes);
}

GSM::GSM(byte tx, byte rx)
{
    Modem = new SoftwareSerial(tx, rx);
    Modem->begin(9600);
}

char* GSM::request(const byte& instruction)
{
	//char eepCommands[11][25] = {"AT", "AT+CSQ", "AT+CPIN?", "AT+CREG?", "AT+QIFGCNT=0", "AT+QICSGP=1,\"internet\"", "AT+QHTTPURL=37,50", "@U", "AT+QHTTPPOST=37,50,10", "@M", "AT+QHTTPREAD=30"};
	const char eepCommands[11][23] = {"AT", "AT+CSQ", "AT+CPIN?", "AT+CREG?", 

"AT+QIFGCNT=0", 
"AT+QICSGP=1,\"internet\"", 
"AT+QHTTPURL=48,60", 
"@U", 
"AT+QHTTPPOST=19,60,30", 
"@M", 

                                "AT+QHTTPREAD=30"};
	//2018/04/26,13:10,5
	//char url[38] = "http://erbium.requestcatcher.com/test";
	const char url[] = "http://erbium.000webhostapp.com/recordInsect.php";
	const char cmeError[] = "+CME ERROR: 3822";

	char command[CMD_LEN];
    memset(command, 0, CMD_LEN);

    Debugln(F("Yoh2"));
	if(eepCommands[instruction][0] == '@') {
		switch(eepCommands[instruction][1]) {
			case 'U':
                strncpy(command, url, CMD_LEN - 1);
				break;
			case 'M':
                strncpy(command, mes, CMD_LEN - 1);
				break;				
		}
	} else {
        strncpy(command, eepCommands[instruction], CMD_LEN - 1);
    }
	Serial.print(F("Command: "));
	Serial.println(command);

	byte count = 0;
	char *buffer = new char [BUFF_LEN];
    memset(buffer, 0, BUFF_LEN);

    byte size = strlen(command);
    for (byte i = 0; i < size; ++i) {
        Modem->write(command[i]);
    }

	Modem->println();

    Serial.print(F("Bufer: "));
    Serial.println(buffer);
    Serial.print(F("Check: "));
    Serial.println((int)check(buffer));
    if (check(buffer) != -1) {
        for (byte i = 1; i < 10; ++i) {
            while (Modem->available() && strlen(buffer) < BUFF_LEN) {
                char read[2];
                memset(read, 0, 2);
                read[0] = Modem->read();
                strncat(buffer, read, BUFF_LEN - strlen(buffer) - 1);
            }
            Serial.print(F("Buffer: "));
            Serial.println(buffer);

            if (strstr(buffer, cmeError) == NULL) {
                return buffer;
            }
            Modem->flush();
            delay(500);
        }
    }
	return buffer;
}


#ifdef __arm__
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

void GSM::execute(byte set[], byte size, bool read = false)
{
	char *output;
	byte process = 0, count = 0;
	//char cmeError[] = "+CME ERROR: 3822";
	char cmeError[] = "+CME ERROR";
	bool error = false;
	
    for (byte i = 0; i < size; ++i) {
        bool error = true;
        Serial.print(F("Execute: "));
        Serial.println(i);
        for (byte retry = 0; retry < MAX_RETRY && error; ++retry) {
            if (retry > 0) {
                Serial.print(F("Retrying "));
            }
            Serial.print("set: ");
            Serial.println(set[i]);

            //execute eep command and record output
            output = request(set[i]);
            Serial.println(F("Response: "));
            Serial.println(output);

            if (strstr(output, cmeError) == NULL) {
                Serial.println(F("No Error"));
                if (read) {
                    memset(ans, 0, ANSW_LEN);
                    strncpy(ans, output, ANSW_LEN - 1);
                }
                error = false;
            } else {
                //TODO appropriate action when an error is returned as response
                Serial.println(F("Resp Err"));
            }

            delete [] output;
        }
        if (error) {
            Serial.println(F("Max retries reached."));
            break;
        }
    }
    //
    Serial.println(F("FREEE"));
}

bool GSM::isOn()
{
    return (check(request(0)) == -1);
}

void GSM::gsmOn()
{
	if (!isOn()) {
		digitalWrite(7, HIGH);
		delay(1000); // 800 also works, but 1000 is safe
		digitalWrite(7, LOW);
		delay(5000);
	}
}

void GSM::gsmOff()
{
	if (isOn()) {
		digitalWrite(7, HIGH);
		delay(1000); // 800 also works, but 1000 is safe
		digitalWrite(7, LOW);
		delay(5000);
	}
}

char GSM::check(char ret [])
{
    if (strlen(ret) == 0) {
        return -1;
    }
    if (!strstr(ret, ">")) {
        return -1;
    }

    if (!strstr(ret, "OK")) {
        return -1;
    }

    if (!strstr(ret, "CONNECT")) {
        return -1;
    }

    return 0;
}

// Start sequence length
#define START_LEN 4

bool GSM::start()
{
    Serial.println(F("\nSTART"));
    byte startupSet[START_LEN] = {0, 1, 2, 3};
	execute(startupSet, START_LEN);
	return true;
}

// Post sequence length
#define POST_LEN 6

bool GSM::postRequest()
{
    Serial.println(F("\nPOST"));
    byte postRequestSet[POST_LEN] = {4, 5, 6, 7, 8, 9};
	execute(postRequestSet, POST_LEN);
	return true;
}

// Start sequence length
#define READ_LEN 1

bool GSM::readRequest()
{
    Serial.println(F("\nREAD"));
    byte readRequestSet[READ_LEN] = {10};
	execute(readRequestSet, READ_LEN, true);
	return true;
}

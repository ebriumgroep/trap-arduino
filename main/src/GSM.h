/*
GSM.h (formerly gsmTransmitter.ino) -
Multi-instance GSM Transmitter library for Arduino

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

The latest version of this library can always be found at
https://github.com/ebriumgroep/trap-arduino
*/

#ifndef GSM_h
#define GSM_h

#include "GSM.h"
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "GF.h"
using namespace Funcs;

class GSM
{
private:
		// Constant Data
		static const int numCommands = 11;
		String orgCommands[numCommands] = {"AT", "AT+CSQ", "AT+CPIN?", "AT+CREG?",
						"AT+QIFGCNT=0", "AT+QICSGP=1,\"internet\"", "@A", "@U", "@B", "@M", "AT+QHTTPREAD=50"};
		String commands[numCommands];
		int startupSet[5] = {0, 0, 2, 3, -1};            	// The commands to execute in sequence to start the GSM modem
		int postRequestSet[7] = {4, 5, 6, 7, 8, 9, -1};  	// The commands to execute in sequence to do a post request to an server
		int readRequestSet[2] = {10, -1};		 			// The commands to execute in sequence to read an server response

		// Internal Variables
		SoftwareSerial *Modem;
		String answer = "";
		String key = "BN+uZa4{$tS={=F#pRflx:}()n7@sM&8";

		// Internal Methods
		String request(int);			// Execute an single command from the commands array
		bool execute(int [], bool);		// Execute a list of functions with the request method
		bool resolve(int);				// Resolves error codes
		int arrLength(int []);			// Determine the length of an array
		int check(String);				// Checks for error codes in the output
		bool isOn();					// Checks if the GSM modem is on
		
public:

		// Getter Methods
		String getAnswer();				// Under Construction

		// Setter Methods
		void setMessage(String);		// Under Construction

		// Control Methods
		void gsmOn();					// Turns the GSM modem on
		void gsmOff();					// Turns the GSM modem off
		bool start();					// Done		
		bool postRequest();				// Done
		bool readRequest();				// Under Construction

		// Constructor
		GSM(int, int, String, String);		// Done
};

#endif

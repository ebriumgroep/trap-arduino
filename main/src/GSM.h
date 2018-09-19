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
		// AT-Commands
		const int numCommands = 14;
		char mes[37];
		char ans[10];
		
		// Command-Sets
		int startupSet[5] = {0, 1, 2, 3, -1};      						// The commands to execute in sequence to start the GSM modem
		int postRequestSet[7] = {4, 5, 6, 7, 8, 9, -1};  				// The commands to execute in sequence to do a post request to an server
		int readRequestSet[2] = {10, -1};			 					// The commands to execute in sequence to read an server response

		// Internal Variables
		SoftwareSerial *Modem;
		char key [32] = {'B','N','+','u','Z','a','4','{','$','t','S','=','{','=','F','#','p','R','f','l','x',':','}','(',')','n','7','@','s','M','&','8'};

		// Internal Methods
		char* request(int);							// Execute an single command from the commands array
		void execute(int [], bool);					// Execute a list of functions with the request method
		int arrLength(int []);						// Determine the length of an array
		int check(char []);							// Checks for error codes in the output
		bool isOn();								// Checks if the GSM modem is on
		
public:
		void setMessage(const char [], bool);		//

		// Control Methods
		void gsmOn();								// Turns the GSM modem on
		void gsmOff();								// Turns the GSM modem off
		bool start();								//		
		bool postRequest();							//
		bool readRequest();							//

		// Constructor
		GSM(int, int);								// Done
		
		char* getAnswer();							//
};

#endif

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

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "GF.h"
using namespace Funcs;

// Appropriate sizes for arrays
#define MESS_LEN 37
#define ANSW_LEN 64
#define CMD_LEN 64
#define BUFF_LEN 64

// Settings
#define MAX_RETRY 10

class GSM
{
    private:
        // AT-Commands
        const byte numCommands = 14;
        char mes[MESS_LEN];
        char ans[ANSW_LEN];

        // Internal Variables
        SoftwareSerial *Modem;
        char key [32] = {'B','N','+','u','Z','a','4','{','$','t',
                         'S','=','{','=','F','#','p','R','f','l',
                         'x',':','}','(',')','n','7','@','s','M','&','8'};

        // Internal Methods

        // Execute an single command from the commands array
        char* request(const byte&);

        // Execute a list of functions with the request method
        void execute(byte [], byte, bool);

        // Checks for error codes in the output
        char check(char []);

        // Checks if the GSM modem is on
        bool isOn();

    public:
        void setMessage(const char [], bool);

        // Control Methods

        // Turns the GSM modem on
        void gsmOn();

        // Turns the GSM modem off
        void gsmOff();

        bool start();

        bool postRequest();

        bool readRequest();

        // Constructor
        GSM(byte, byte);

        char* getAnswer();
};

#endif

#ifndef GSM_MODULE_H
#define GSM_MODULE_H

#include <Arduino.h>
#include <SoftwareSerial.h>

class GSMModule
{
    public:

        /**
         *
         **/
        GSMModule(byte, byte);

        /**
         *
         **/
        bool start();

        /**
         *
         **/
        bool postRequest(char[], char[]);

        /**
         * 
         **/
        char* readResponse();

    private:

        SoftwareSerial* modem;

        char* executeAT(const char[], int, const char[]);


};

#endif

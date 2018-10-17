#include "GSMModule.h"

#define DEBUG_GSM

#ifndef DEBUG_GSM
    #define Debugln(a)
    #define Debug(a)
#else
    #define Debugln(a) Serial.println(a)
    #define Debug(a) Serial.print(a)
#endif

GSMModule::GSMModule(byte tx, byte rx)
{
    modem = new SoftwareSerial(tx, rx);
    modem->begin(9600);
}

#define BUFF_LEN 128
char* GSMModule::executeAT(const char command[], int delays = 500, 
        const char search[] = "")
{
    char* buffer = new char[BUFF_LEN];
    memset(buffer, 0, BUFF_LEN);
    modem->println(command);
    //execute command
    delay(delays);
    //int time = millis();
    //while (time + delays > millis()) {}
    while (!modem->available()) {}
    delay(200);
    for (byte i = 0; i < BUFF_LEN - 1 && modem->available(); ++i) {
        buffer[i] = modem->read();
    }

    if (strlen(search) != 0 && 
            (strstr(search, buffer) != NULL || 
             strstr(buffer, search) != NULL)) {
        if (strstr(buffer, "CONNECT") || strstr(buffer, "OK")) {
            return buffer;
        }
        int end_time = 10000 + millis();
        while (!modem->available() && end_time > millis()) {}
        delay(200);
//        for (byte i = 0; i < BUFF_LEN - 1 && modem->available(); ++i) {
        while (modem->available() && strlen(buffer) < BUFF_LEN) {
            char temp[2];
            memset(temp, 0, 2);
            temp[0] = modem->read();
            strncat(buffer, temp, BUFF_LEN - strlen(buffer) - 1);
        }
    }
    modem->flush();
    delay(600L);
    //time = millis();
    //while (time + 600 > millis()) {}
    return buffer;
}

bool GSMModule::start()
{
    Debugln(F("\nStart"));
    char* gsmResponse = NULL;

    gsmResponse = executeAT("AT");
    if (strstr(gsmResponse, "OK") == NULL) {
        Debugln(gsmResponse);
        delete [] gsmResponse;
        return false;
    }
    Debugln(F("OK: AT"));
    delete [] gsmResponse;
    gsmResponse = NULL;

    gsmResponse = executeAT("AT+CSQ");
    if (strstr(gsmResponse, "OK") == NULL) {
        Debugln(gsmResponse);
        delete [] gsmResponse;
        return false;
    }
    Debugln(F("OK: AT+CSQ"));
    delete [] gsmResponse;
    gsmResponse = NULL;

    gsmResponse = executeAT("AT+CPIN?");
    if (strstr(gsmResponse, "READY") == NULL) {
        Debugln(gsmResponse);
        delete [] gsmResponse;
        return false;
    }
    Debugln(F("OK: AT+CPIN?"));
    delete [] gsmResponse;
    gsmResponse = NULL;

    gsmResponse = executeAT("AT+CREG?");
    if (strstr(gsmResponse, "OK") == NULL) {
        Debugln(gsmResponse);
        delete [] gsmResponse;
        return false;
    }
    Debugln(F("OK: AT+CREG?"));
    delete [] gsmResponse;
    gsmResponse = NULL;

    return true;
}

bool GSMModule::postRequest(char url[], char data[])
{
    //TODO do post sequence
    Debugln(F("\nPost"));
    Debugln(data);
    char* gsmResponse = NULL;

    gsmResponse = executeAT("AT+QIFGCNT=0");
    if (strstr(gsmResponse, "OK") == NULL) {
        Debugln(F("Error: AT+QIFGCNT=0"));
        Debugln(gsmResponse);
        delete [] gsmResponse;
        return false;
    }
    Debugln(F("OK: AT+QIFGCNT=0"));
    delete [] gsmResponse;
    gsmResponse = NULL;

    gsmResponse = executeAT("AT+QICSGP=1,\"internet\"");
    if (strstr(gsmResponse, "OK") == NULL) {
        Debugln(F("Error: AT+QICSGP=1"));
        Debugln(gsmResponse);
        delete [] gsmResponse;
        return false;
    }
    Debugln(F("OK: AT+QICSGP=1"));
    delete [] gsmResponse;
    gsmResponse = NULL;

    char httpCmd[18] = "AT+QHTTPURL=";
    char len[4] = "";
    itoa(strlen(url), len, 10);
    strncat(httpCmd, len, 17 - strlen(httpCmd));
    strncat(httpCmd, ",10", 17 - strlen(httpCmd));

    gsmResponse = executeAT(httpCmd);
    if (strstr(gsmResponse, "CONNECT") == NULL) {
        Debug(F("Error: "));
        Debugln(httpCmd);
        Debugln(gsmResponse);
        delete [] gsmResponse;
        return false;
    }
    Debug(F("OK: "));
    Debugln(httpCmd);
    delete [] gsmResponse;
    gsmResponse = NULL;

    gsmResponse = executeAT(url);
    if (strstr(gsmResponse, "OK") == NULL) {
        Debugln(F("Error: URL"));
        Debugln(gsmResponse);
        delete [] gsmResponse;
        return false;
    }
    Debugln(F("OK: URL"));
    delete [] gsmResponse;
    gsmResponse = NULL;

    char postCmd[23] = "AT+QHTTPPOST=";
    memset(len, 0, 4);
    itoa(strlen(data), len, 10);
    Debugln(F("LEN"));
    Debugln(len);
    strncat(postCmd, len, 22 - strlen(postCmd));
    strncat(postCmd, ",20,20", 22 - strlen(postCmd));
    gsmResponse = executeAT(postCmd, 1000, "AT+QHTTPPOST=");
    if (strstr(gsmResponse, "CONNECT") == NULL) {
        Debug(F("Error: "));
        Debugln(postCmd);
        Debug(F("Response: "));
        Debugln(gsmResponse);
        delete [] gsmResponse;
        return false;
    }
    Debug(F("OK: "));
    Debugln(postCmd);
        Debugln(gsmResponse);
    delete [] gsmResponse;
    gsmResponse = NULL;

    gsmResponse = executeAT(data);
    if (strstr(gsmResponse, "OK") == NULL) {
        Debugln(F("Error: Data"));
        Debugln(gsmResponse);
        delete [] gsmResponse;
        return false;
    }
    Debugln(F("OK: Data"));
    delete [] gsmResponse;
    gsmResponse = NULL;

    return true;
}

char* GSMModule::readResponse()
{
    Debugln(F("\nRead"));

    char* gsmResponse = executeAT("AT+QHTTPREAD=30", 20000);
    if (strstr(gsmResponse, "CONNECT") == NULL) {
        Debugln(F("Error: Read"));
        Debugln(gsmResponse);
    } else {
        Debugln(F("OK: Data"));
        Debugln(gsmResponse);
    }
    return gsmResponse;
}

/*
GSM.h (formerly gsmTransmitter.ino) -
Multi-instance GSM Transmitter library for Arduino

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

The latest version of this library can always be found at
https://github.com/ebriumgroep/trap-arduino
*/

#include "GSM.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// SETTER METHODS
void GSM::setAdress(String ad)
{
    adress = ad;
}

void GSM::setMesage(String me)
{
    mesage = me;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor and Destructor
GSM::GSM(int tx, int rx)
{
    // Create and inintalise the modem object
    MODEM = new SoftwareSerial(tx,rx);
    MODEM->begin(9600);

    // Pinmodes
    pinMode(7, OUTPUT);

    // For Debugging
    Serial.begin(9600);
}

GSM::~GSM()
{
    //Destructor
}
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
// Internal Methods
char* GSM::request(int instruction)
{
    int count = 0, timeout = 0, i = 0;
    bool flag = false;
    char* buffer = new char[64];

    for (int a = 0; a < 64; a++)
    {
        buffer[a] = '\0';
    }

    if(commands[instruction][0] == '@')
    {
        switch(commands[instruction][1])
        {
            case 'U':
                MODEM->println(adress);
                break;
            case 'M':
                MODEM->println(mesage);
                break;
            case 'A':
                MODEM->println(String("AT+QHTTPURL= ")+String(adress.length())+String(",50"   ));
                break;
            case 'B':
                MODEM->println(String("AT+QHTTPPOST=")+String(mesage.length())+String(",50,50"));
                break;
        }
    }
    else
    {
        while (commands[instruction][i] != '\0')
        {
            byte b = commands[instruction][i];
            if (b == '|')
                MODEM->write(0x1a);
            else
                MODEM->write(b);
            ++i;
        }
    }

    MODEM->println();
    delay(100);

    while (!flag and timeout < 5)
    {
        while (MODEM->available())
        {
            buffer[count++] = MODEM->read();
            if (count == 64)
                break;
            flag = true;
        }
        delay(2000);
        if(MODEM->available())
            flag = false;
        ++timeout;
    }
    return buffer;
}

bool GSM::execute(int set[])
{
    bool completed = false, fine = true;
    char* output;
    int process = 0, errorCode = -1;
    while (!completed and fine and process < arrLength(set))
    {
        Serial.println(process);
        output = request(set[process]);
        errorCode = check(output);
        Serial.println(errorCode);
        if (errorCode != -1)
            fine = false;
        if (!fine)
        {
            fine = resolve(errorCode);
            --process;
        }
        if (fine)
        {
            errorCode = -1;
            ++process;
        }
        else
        {
            break;
        }
        delete [] output;
    }
    return fine;
}

int GSM::arrLength(int arr[])
{
    int i = 0;
    while (arr[i] != -1)
    {
        ++i;
    }
    return i;
}

bool GSM::resolve(int errorCode)
{
  switch (errorCode) {
    case 0:
      gsmOn();
      break;
    case 10:
      return false;
    case 11:
      return false;
    case 30:
      return false;
    case 100:
      return false;
    default:
      return false;
  }
  return true;
}

void GSM::gsmOn()
{
    digitalWrite(7, HIGH);
    delay(1000); // 800 also works, but 1000 is safe
    digitalWrite(7, LOW);
    delay(10000); // 9 seconds also works, but 10 is safe
}

void GSM::gsmOf()
{
    digitalWrite(7, HIGH);
    delay(1000); // 800 also works, but 1000 is safe
    digitalWrite(7, LOW);
    delay(2000); // wait until shutdown is done
}

int GSM::check(char ret[])
{
    char err[] = {'\0', '\0', '\0', '\0'};
    if (ret[0] == '\0')
        err[0] = 0; // Nothing got returned
    for (int a = 0; a < 64; ++a)
    {
        // SMS MESSAGE
        if (ret[a] == '>')
        {
            return -1;
        }
        // OK MESSAGE
        if (ret[a] == 'O')
            if (ret[a + 1] == 'K')
            {
                return -1;
            }
        // CONNECT MESSAGE
        if (ret[a] == 'C')
            if (ret[a+1] == 'O')
                if (ret[a+2] == 'N')
                    if (ret[a+3] == 'N')
                        if (ret[a+4] == 'E')
                            if (ret[a+5] == 'C')
                                if (ret[a+6] == 'T')
                                {
                                    return -1;
                                }
        // CME ERROR
        if (ret[a] == 'C')
            if (ret[a + 1] == 'M')
                if (ret[a + 2] == 'E')
                    for (int i = a; i < a + 4; ++i)
                    {
                        if (isDigit(ret[i + 11]))
                        err[i - a] = ret[i + 11];
                    }
    }
    return atoi(err);
}
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
//Controll Methods
bool GSM::start()
{
    return execute(startupSet);
}

bool GSM::postr()
{
    return execute(postRequestSet);
}
/////////////////////////////////////////////////////////////////////////////////////////////

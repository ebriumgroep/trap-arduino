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

//////////////////////////////////////////////////////////////////////////////////////////////////
// SETTER METHODS
void GSM::setAddress(String ad)
{
	address = ad;
}

void GSM::setMessage(String me)
{
	message = me;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor
GSM::GSM(int tx, int rx)
{
	// Create and initialize the modem object
	Modem = new SoftwareSerial(tx, rx);
	Modem->begin(9600);

	// Pin modes
	pinMode(7, OUTPUT);

	// For Debugging
	Serial.begin(9600);
}

//Destructor
GSM::~GSM()
{
	gsmOff();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
// Internal Methods
char *GSM::request(int instruction)
{
	int count = 0, timeout = 0, i = 0;
	bool flag = false;
	char *buffer = new char[64];

	for (int a = 0; a < 64; a++)
	{
		buffer[a] = '\0';
	}

	if (commands[instruction][0] == '@')
	{
		switch (commands[instruction][1])
		{
			case 'U':
				Modem->println(address);
				break;
			case 'M':
				Modem->println(message);
				break;
			case 'A':
				Modem->println(String("AT+QHTTPURL= ") + String(address.length()) + String(",50"));
				break;
			case 'B':
				Modem->println(String("AT+QHTTPPOST=") + String(message.length()) + String(",50,50"));
				break;
		}
	} else
	{
		while (commands[instruction][i] != '\0')
		{
			byte b = commands[instruction][i];
			if (b == '|')
				Modem->write(0x1a);
			else
				Modem->write(b);
			++i;
		}
	}

	Modem->println();
	delay(100);

	while (!flag and timeout < 5)
	{
		while (Modem->available())
		{
			buffer[count++] = Modem->read();
			if (count == 64)
				break;
			flag = true;
		}
		delay(2000);
		if (Modem->available())
			flag = false;
		++timeout;
	}
	return buffer;
}

bool GSM::execute(int set[])
{
	bool fine = true;
	char *output;
	int process = 0, errorCode = -1;
	while (fine and process < arrLength(set))
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
		} else
		{
			break;
		}
		delete[] output;
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
	switch (errorCode)
	{
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

bool GSM::isOn()
{
	if (check(request(0)) == -1)
		return true;
	else
		return false;
}

void GSM::gsmOn()
{
	if (!isOn())
	{
		digitalWrite(7, HIGH);
		delay(1000); // 800 also works, but 1000 is safe
		digitalWrite(7, LOW);
		delay(10000); // 9 seconds also works, but 10 is safe
	}
}

void GSM::gsmOff()
{
	if (isOn())
	{
		digitalWrite(7, HIGH);
		delay(1000); // 800 also works, but 1000 is safe
		digitalWrite(7, LOW);
		delay(2000); // wait until shutdown is done
	}
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
			return -1;
		// OK MESSAGE
		if (ret[a] == 'O')
			if (ret[a + 1] == 'K')
				return -1;
		// CONNECT MESSAGE
		if (ret[a] == 'C')
			if (ret[a + 1] == 'O')
				if (ret[a + 2] == 'N')
					if (ret[a + 3] == 'N')
						if (ret[a + 4] == 'E')
							if (ret[a + 5] == 'C')
								if (ret[a + 6] == 'T')
									return -1;
		// CME ERROR
		if (ret[a] == 'C')
			if (ret[a + 1] == 'M')
				if (ret[a + 2] == 'E')
					for (int i = a; i < a + 4; ++i)
					{
						if (isDigit(ret[11 + i]))
							err[i - a] = ret[11 + i];
					}
	}
	return atoi(err);
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
//Control Methods
bool GSM::start()
{
	return execute(startupSet);
}

bool GSM::postRequest()
{
	return execute(postRequestSet);
}
/////////////////////////////////////////////////////////////////////////////////////////////

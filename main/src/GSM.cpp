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
// Getter Methods
String GSM::getAnswer()
{
	return answer;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Setter Methods
void GSM::setMessage(String me)
{
	int ha = hash(me);
	//String message = encrypt(String(ha)+String(",")+String(me),key);
	String message = String(ha)+String(",")+String(me);
	for(int a = 0; a<numCommands; ++a)
	{
		if(orgCommands[a][0] == '@')
		{
			switch(orgCommands[a][1])
			{
				case 'M':
					orgCommands[a] = String("@M" + message);
					break;
				case 'B':
					orgCommands[a] = String("@BAT+QHTTPPOST=" + String(message.length()) + ",50,50");
					break; 	
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor
GSM::GSM(int tx, int rx, String ad, String me)
{
	int ha = hash(me);
	String message = String(ha)+String(",")+String(me);

	for(int a = 0; a<numCommands; ++a)
	{
		if(orgCommands[a][0] == '@')
		{
			switch(orgCommands[a][1])
			{
				case 'U':
					orgCommands[a] = String("@U" + ad);
					break;
				case 'M':
					orgCommands[a] = String("@M" + message);
					break;
				case 'A':
					orgCommands[a] = String("@AAT+QHTTPURL=" + String(ad.length()) + ",50");
					break;
				case 'B':
					orgCommands[a] = String("@BAT+QHTTPPOST=" + String(message.length()) + ",50,50");
					break; 	
			}
		}
	}

	Modem = new SoftwareSerial(tx, rx);
	Modem->begin(9600);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
// Internal Methods
String GSM::request(int instruction)
{
	for (int a = 0; a < numCommands; ++a)
	{
		if(orgCommands[a][0] == '@')
		{
			switch(orgCommands[a][1])
			{
				case 'U':
					commands[a] = orgCommands[a].substring(2);
					break;
				case 'M':
					commands[a] = orgCommands[a].substring(2);
					break;
				case 'A':
					commands[a] = orgCommands[a].substring(2);
					break;
				case 'B':
					commands[a] = orgCommands[a].substring(2);
					break; 	
			}
		}
		else
			commands[a] = orgCommands[a];
	}
	int count = 0, timeout = 0, i = 0;
	bool flag = false;
	char *buffer = new char[32];

	for (int a = 0; a < 32; ++a)
	{
		buffer[a] = '\0';
	}

	while(commands[instruction][i] != '\0')
	{
		byte b = commands[instruction][i];
		Modem->write(b);
		++i;
	}

	Modem->println();
	delay(100);

	while (!flag and timeout < 5)
	{
		while (Modem->available())
		{
			buffer[count++] = Modem->read();
			if (count == 32)
				break;
			flag = true;
		}
		delay(2000);
		if (Modem->available())
			flag = false;
		++timeout;
	}
	String ret = String(buffer);
	for(int a = 0; a<32; ++a)
	{
		Serial.print(buffer[a]);
	}
	Serial.println();
	delete [] buffer;
	return ret;
}

bool GSM::execute(int set[], bool read = false)
{
	bool fine = true;
	String output;
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
		}
	}
	
	if(read)
	{
		// Phrase the output and save it to answer
		answer = output;	
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
		delay(5000);
	}
}

void GSM::gsmOff()
{
	if (isOn())
	{
		digitalWrite(7, HIGH);
		delay(1000); // 800 also works, but 1000 is safe
		digitalWrite(7, LOW);
		delay(5000);
	}
}

int GSM::check(String ret)
{
	char err[] = {'\0', '\0', '\0', '\0'};

	if (ret[0] == '\0')
	{
		err[0] = 0; // Nothing got returned
		return 0;
	}

	for (int a = 0; a < 32; ++a)
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
			if (ret[a + 1] == 'O')
				if (ret[a + 2] == 'N')
					if (ret[a + 3] == 'N')
						if (ret[a + 4] == 'E')
							if (ret[a + 5] == 'C')
								if (ret[a + 6] == 'T')
								{
									return -1;
								}
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
	Serial.println("a");
	return execute(startupSet);
}

bool GSM::postRequest()
{
	Serial.println("b");
	return execute(postRequestSet);
}

bool GSM::readRequest()
{
	Serial.println("c");
	return execute(readRequestSet, true);
}
/////////////////////////////////////////////////////////////////////////////////////////////

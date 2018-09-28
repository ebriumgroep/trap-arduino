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

char* GSM::getAnswer()
{
	return ans;
}

void GSM::setMessage(const char me [], bool db = false)
{	
	/*char ha[3];
	itoa(hash(me),ha,10);
	
	for(int a =  0; a < 16; ++a)
		mes[a] = me[a];
	
	mes[16] = ',';
	
	for(int b = 17; b < 33; ++b)
		if(db)
			mes[b] = me[b-1];
		else
			mes[b] = 'x';
	
	mes[33] = ',';
	
	for(int c = 34; c < 37; ++c)
		mes[c] = ha[c-34];
	
	mes[37] = '\0';*/
	for (int i = 0; i < arrLength(me) || i < 37; i++)
	{
		mes[i] = me[i];	
	}
	//mes = me;
	
	// test output of the message
	Serial.println(mes);
}

GSM::GSM(int tx, int rx)
{	
	Modem = new SoftwareSerial(tx, rx);
	Modem->begin(9600);
}

char* GSM::request(int instruction)
{
	//char eepCommands[11][25] = {"AT", "AT+CSQ", "AT+CPIN?", "AT+CREG?", "AT+QIFGCNT=0", "AT+QICSGP=1,\"internet\"", "AT+QHTTPURL=37,50", "@U", "AT+QHTTPPOST=37,50,10", "@M", "AT+QHTTPREAD=30"};
	char eepCommands[11][25] = {"AT", "AT+CSQ", "AT+CPIN?", "AT+CREG?", "AT+QIFGCNT=0", "AT+QICSGP=1,\"internet\"", "AT+QHTTPURL=48,60", "@U", "AT+QHTTPPOST=19,60,30", "@M", "AT+QHTTPREAD=30"};
	//2018/04/26,13:10,5
	//char url[38] = "http://erbium.requestcatcher.com/test";
	char url[49] = "http://erbium.000webhostapp.com/recordInsect.php";
	char cmeError[17] = "+CME ERROR: 3822";
	char command[64];
	if(eepCommands[instruction][0] == '@')
	{
		switch(eepCommands[instruction][1])
		{
			case 'U':
				for(int a = 0; a < 49; ++a)
					command[a] = url[a];
				break;
			case 'M':
				for(int a = 0; a < 19; ++a)			//37
					command[a] = mes[a];
				break;				
		}
	}
	else
	{
		for(int a = 0; a < 25; ++a)
			command[a] = eepCommands[instruction][a];
	}
	//Serial.print("Command: ");
	//Serial.println(command);

	int count = 0, i = 0;
	char *buffer = new char [64];

	for (int a = 0; a < 64; ++a)
		buffer[a] = '#';
	
	while(command[i] != '\0')
	{
		byte b = command[i];
		Modem->write(b);
		++i;
	}
	Modem->println();
	int c = 0;
	while(check(buffer) != -1 && c < 10)
	{		
		while(Modem->available())
		{
			buffer[count++] = Modem->read();
			if (count == 64)
				break;
		}
		count = 0;
		//Serial.println(buffer);
		while(buffer[count] == cmeError[count])
		{
			count++;
			if (count == 17)
			{
				return buffer;
			}
		}
		count = 0;
		Modem->flush();
	
		Serial.println("stuck...");
		c++;
		delay(500);
	}
	if (c >= 30)
	{
		buffer[0] = '0';
		return buffer;
	}
	return buffer;
}

void GSM::execute(int set[], bool read = false)
{
	char *output;
	int process = 0, errorCode = -1, cmeRe = 10, count = 0;
	char cmeError[17] = "+CME ERROR: 3822";
	bool error = false;
	
	while (process < arrLength(set) || error)
	{
		output = request(set[process]);
		
		Serial.println("**********");
		for(int a = 0; a<64; ++a)
			Serial.print(output[a]);
		Serial.println();
		Serial.println("**********");
	
		if(read)
		{
			for (int i = 0; i < arrLength(output) && i < 40; i++)
			{
				ans[0] = output[i + 30];
				if (ans[0] == '0' || ans[0] == '1')
				{
					break;
				}
				//Serial.println(ans);
			}
			
		} 
		
		while (count < 17 && output[count] == cmeError[count])
		{
			count++;
		}	
		if (count == 15 && cmeRe > 0)
		{
			error = true;
			cmeRe--;
		}
		else
		{
			error = false;
		}

		count = 0;
		delete [] output;
		
		++process;
	}
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

int GSM::arrLength(char *arr)
{
	int i = 0;
	while (arr[i] != '\0')
	{
		++i;
	}
	++i;
	return i;
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

int GSM::check(char ret [])
{
	int result = 0;

	for (int a = 0; a < 32; ++a)
	{
		// SMS MESSAGE
		if (ret[a] == '>')
		{
			result = -1;
			break;
		}
		// OK MESSAGE
		if (ret[a] == 'O')
			if (ret[a + 1] == 'K')
			{
				result = -1;
				break;
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
									result = -1;
									break;
								}
	}
	return result;
}

bool GSM::start()
{
	execute(startupSet);
	return true;
}

bool GSM::postRequest()
{
	execute(postRequestSet);
	return true;
}

bool GSM::readRequest()
{
	execute(readRequestSet, true);
	return true;
}

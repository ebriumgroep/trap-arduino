/*
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

The latest version of this library can always be found at
https://github.com/ebriumgroep/trap-arduino
*/

#include "TEX.h"

TEX::TEX (String n)
{
	fileName = n;
	path = "";
}

TEX::TEX (String n, String p)
{
	fileName = n;
	path = p;
}

bool TEX::write(String e)
{
	dataFile = SD.open(path + fileName, FILE_WRITE); //Open file in writing mode or create file if not existing
	if (!dataFile)
	{
		return false; //Error with creating file or opening file
	}
	dataFile.println(e); //Write line of text
	dataFile.close();
	return true;
}

bool TEX::append(String e)
{
	dataFile = SD.open(path + fileName, O_RDWR | O_APPEND); //Open file in append mode
	if (!dataFile)
	{
		return write(e); //Standard writing function is called 
	}
	dataFile.println(e); //Write line of text
	dataFile.close();
	return true;
}

bool TEX::openRM()
{
	dataFile = SD.open(path + fileName);
	if (dataFile)
	{
		return true;
	}
	return false;
}

String TEX::readln()
{
	//Reading text characters in a char array 
	char line[100];
	line[0] = dataFile.read();
	int count = 1;
	while(line[count - 1] != '\n')
	{
      line[count] = dataFile.read();
      count++;
  	}
  	//Converting char array to String var
  	String finalLine = "";
  	for (int i = 0; i < count; i++)
  	{
  		finalLine += line[i]; 
  	}
  	return finalLine;
}

void TEX::closeRM()
{
	dataFile.close();
}

void TEX::changeFileName(String n)
{
	fileName = n;
}

String TEX::getFileName()
{
	return fileName;
}

void TEX::changePath(String p)
{
	path = p;
}

String TEX::getPath()
{
	return path;
}

String TEX::getFullPath()
{
	return path + fileName;
}

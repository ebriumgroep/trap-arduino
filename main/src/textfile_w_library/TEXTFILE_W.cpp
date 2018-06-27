/*
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

The latest version of this library can always be found at
https://github.com/ebriumgroep/trap-arduino
*/

#include "TEXTFILE_W.h"

TEXTFILE_W::TEXTFILE_W (String n)
{
	fileName = n;
	path = "";
}

TEXTFILE_W::TEXTFILE_W (String n, String p)
{
	fileName = n;
	path = p;
}

bool TEXTFILE_W::write(String e)
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

bool TEXTFILE_W::append(String e)
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

void TEXTFILE_W::changeFileName(String n)
{
	fileName = n;
}

String TEXTFILE_W::getFileName()
{
	return fileName;
}

void TEXTFILE_W::changePath(String p)
{
	path = p;
}

String TEXTFILE_W::getPath()
{
	return path;
}

String TEXTFILE_W::getFullPath()
{
	return path + fileName;
}

/*
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

The latest version of this library can always be found at
https://github.com/ebriumgroep/trap-arduino
*/

#ifndef TEX_h
#define TEX_h

#include "TEX.h"
#include <SD.h>
#include <SPI.h>
#include <SoftwareSerial.h>

class TEX
{
	private:
		File dataFile; //Text file data
		String fileName; //Name of the file
		String path; //Path of the file (end path with \)
	public:
		//Constructors
		TEX (String);
		TEX (String, String);

		//Operating functions
		bool write(String);
		bool append(String);
		bool openRM();
		String readln();
		void closeRM();
		//Get and Set functions
		void changeFileName(String);
		String getFileName();
		void changePath(String);
		String getPath();
		String getFullPath();
};

#endif


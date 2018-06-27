/*
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

The latest version of this library can always be found at
https://github.com/ebriumgroep/trap-arduino
*/

#ifndef TEXTFILE_W_h
#define TEXTFILE_W_h

#include "TEXTFILE_W.h"
#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

class TEXTFILE_W
{
	private:
		File dataFile; //Text file data
		String fileName; //Name of the file
		String path; //Path of the file (end path with \)
	public:
		//Constructors
		TEXTFILE_W (String);
		TEXTFILE_W (String, String);

		//Operating functions
		bool write(String);
		bool append(String);

		//Get and Set functions
		void changeFileName(String);
		String getFileName();
		void changePath(String);
		String getPath();
		String getFullPath();
};

#endif


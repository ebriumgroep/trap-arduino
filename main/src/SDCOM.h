#ifndef SDCOM_h
#define SDCOM_h

#include "SDRaw.h"
//#include <Arduino.h>

//#define SDCARDSS (byte) //SDCard SS Pin
//#define SDCARDSTARTSECTOR (unsigned int)

class SDCOM
{
	public:
		//Constructors
		SDCOM(byte);
		//Operating functions
		~SDCOM();
		String read(unsigned long );
		unsigned char write(String , unsigned long);

	private:
		SDRaw *com;
};

#endif
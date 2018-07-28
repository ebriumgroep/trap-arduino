#include "SDCOM.h"

SDCOM::SDCOM(byte pin)
{
	com = new SDRaw(pin);
}

SDCOM::~SDCOM()
{
	delete com;
}

 String SDCOM::read(unsigned long Rstartblock)
{
	return com->readstring(Rstartblock);
}

unsigned char SDCOM::write(String writestring, unsigned long Wstartblock)
{
	com->writestring(writestring, Wstartblock);
}
/*
GF.cpp -
General Functions library for Arduino

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

The latest version of this library can always be found at
https://github.com/ebriumgroep/trap-arduino
*/

#include "GF.h"

int GF::hash(String text)
{
	long int sum = 0, key = 0;
	int count = 0, length = 0;
	for (int i = 0; i < text.length(); ++i) // adds the ASCII value for each letter to sum
		sum += int(text[i]); // gets ASCII value of letter
	while (key < 100 && count++ < 3) // ensures key is at least 3 characters long with no infinite loop
		key = sum = sum * sum; // squares the sum
	for(; sum != 0; sum /= 10, ++length); // gets the length of the squared sum
	char *sKey = new char[3];
	for (int i = 0; i < 3; ++i)
		sKey[i] = String(key)[((length/2) - 1) + i]; // gets the middle three characters
	int ret = atoi(sKey); // converts char array to int
	delete [] sKey;
	return ret; // returns an int between 100 and 999
}

String GF::encrypt(String text, String key)
{
	return text;
}

String GF::decrypt(String encrypted, String key)
{
	return encrypted;
}
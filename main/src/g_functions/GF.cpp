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
namespace Funcs
{
int hash(String text)
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

String encrypt(String text, String key)
{
	String encrypted = text;
	int c, i = 0;
	for (int j = 0; j < encrypted.length(); ++j)
	{
		if (i % 2 == 0)
		{
			c = int(encrypted[j]) + int(key[i++]);
			if (c < 128)
				encrypted[j] = char(c);
			else
				encrypted[j] = char(c - 128);
		}
		else
			c = int(encrypted[j]) - int(key[i++]);
			if (c > -1)
				encrypted[j] = char(c);
			else
				encrypted[j] = char(c + 128);
		if (i > 32)
			i = 0;
	}
	return encrypted;
}

String decrypt(String encrypted, String key)
{
	String text = encrypted;
	int c, i = 0;
	for (int j = 0; j < text.length(); ++j)
	{
		if (i % 2 == 0)
		{
			c = int(text[j]) - int(key[i++]);
			if (c > -1)
				text[j] = char(c);
			else
				text[j] = char(c + 128);
		}
		else
			c = int(text[j]) + int(key[i++]);
			if (c < 128)
				text[j] = char(c);
			else
				text[j] = char(c - 128);
		if (i > 32)
			i = 0;
	}
	return text;
}
}
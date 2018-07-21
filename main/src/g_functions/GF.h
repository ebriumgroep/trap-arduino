/*
GF.h -
General Functions library for Arduino

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

The latest version of this library can always be found at
https://github.com/ebriumgroep/trap-arduino
*/

#include <Arduino.h>

namespace Funcs
{
	int hash(String text);							// Hashes a String using a ASCII addition and Mid-Square method
	String encrypt(String text, String key);		// Encrypts a String with a 256-bit key
	String decrypt(String encrypted, String key);	// Decrypts a String with a 256-bit key
}

/*
  GF.h -
  General Functions library for Arduino

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  The latest version of this library can always be found at
  https://github.com/ebriumgroep/trap-arduino
*/

#ifndef GF_h
#define GF_h

#include <Arduino.h>

namespace Funcs
{
int hash(char text[]);							// Hashes a String using a ASCII addition and Mid-Square method
void encrypt(char text[], char key[]);			// Encrypts a text line with a 256-bit alphanumeric key, pass by reference
void decrypt(char encrypted[], char key[]);		// Decrypts a text line with a 256-bit alphanumeric key, pass by reference
}

#endif

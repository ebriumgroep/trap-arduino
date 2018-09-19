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
int hash(char text[])
{
  long int sum = 0;
  char sKey[3];
  int count = 0, length = 0;
  for (int i = 0; i < String(text).length(); ++i) // adds the ASCII value for each letter to sum
    sum += int(text[i]); // gets ASCII value of letter
  while (sum < 100 and count++ < 3) // ensures sum is at least 3 characters long with no infinite loop
    sum = sum * sum; // squares the sum
  length = String(sum).length(); // gets the length of the sum
  for (int i = 0; i < 3; ++i)
    sKey[i] = String(sum)[((length / 2) - 1) + i]; // gets the middle three characters
  int ret = atoi(sKey); // converts char array to int
  return ret; // returns an int between 100 and 999
}

void encrypt(char text[], char key[])
{
  int c, i = 0;
  for (int j = 0; j < String(text).length(); ++j, ++i) // for every character
  {
    if (text[j] != ',') // skip if a comma is detected
      if (j % 2 == 0) // add ASCII codes for odd, deduct ASCII codes for even
      {
        c = int(text[j]) + int(key[i]) - 48; // add ASCII codes
        text[j] = (c > 126 ? char(c - 79) : char(c)); // ensures alphanumeric character
      }
      else
      {
        c = int(text[j]) - int(key[i]) + 48; // deduct ASCII codes
        text[j] = (c < 48 ? char(c + 79) : char(c)); // ensures alphanumeric character
      }
    if (i > 30) // loop through key
      i = -1;
  }
}

void decrypt(char encrypted [], char key [])
{
  int c, i = 0;
  for (int j = 0; j < String(encrypted).length(); ++j, ++i)
  {
    if (encrypted[j] != ',') // skip if a comma is detected
      if (j % 2 == 0) // add ASCII codes for odd, deduct ASCII codes for even
      {
        c = int(encrypted[j]) - int(key[i]) + 48; // deduct ASCII codes
        encrypted[j] = (c < 48 ? char(c + 79) : char(c)); // ensures alphanumeric character
      }
      else
      {
        c = int(encrypted[j]) + int(key[i]) - 48; // add ASCII codes
        encrypted[j] = (c > 126 ? char (c - 79) : char(c)); // ensures alphanumeric character
      }
    if (i > 30) // loop through key
      i = -1;
  }
}


void floatToChar(float num, char buff[])
{
  float num2;
  int num3, count = 0;
  char num4;
  buff[0] = '0';

  for (int i = 3; i > -2; i--)
  {
    num2 = num / (pow(10, i));
    num3 = (int) num2;
    num3 = num3 % 10;
    num4 = (char) num3;
    if (num4 != '0' || buff[0] != '0')
    {
      buff[count] = num4;
      count++;
    }
    if (i == 0)
    {
      buff[count] = '.';
      count++;
    }
  }
}
}

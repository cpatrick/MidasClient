/*=========================================================================

  Program:   Dionysus
  Module:    kwCrypt.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwCrypt.h"

#include <string.h>

kwCrypt::kwCrypt()
{
  key = 
   "Qu09hdlZq!*^%'$~#KAUYo3gJrCO7v2&HiPV6Fw1Wes<TEN8f4LjRmIGMbySpx5czanDX>tBk";
  key2= 
   "Qu09hdlZq!*^%-$~#KAUYo3gJrCO7v2/HiPV6Fw1Wes_TEN8f4LjRmIGMbySpx5czanDX?tBk";
}

kwCrypt::~kwCrypt()
{
}

/** Crypt a buffer */
const char* kwCrypt::Crypt(const char* buffer)
{
  char* coded = new char[strlen(buffer)+1];
  unsigned long shift = strlen(buffer);
  for(unsigned long j=0;j<shift;j++)
    {
    if(key.find(buffer[j]) == -1)
      {
      coded[j] = buffer[j];
      }
    else
      {
      unsigned long ch = (key.find(buffer[j]) + shift) % key.length();
      coded[j] = key2[ch];
      }
    }
  coded[strlen(buffer)]=0;

  return coded;

}

/** Decrypt a buffer */
const char* kwCrypt::Decrypt(const char* buffer)
{
  char* decoded = new char[strlen(buffer)+1];
  unsigned long shift = strlen(buffer);
  for (unsigned int i=0; i<strlen(buffer); i++)
    {
    if (key2.find(buffer[i]) == -1)
      {
      decoded[i] = buffer[i];
      }
    else {
      unsigned long pos = (key2.find(buffer[i])-shift+key2.length()) % 
                                                            + key2.length();
      decoded[i] = key[pos];
      }
    }

  decoded[strlen(buffer)]=0;
  return decoded;
}

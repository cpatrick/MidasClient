/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "midasUUID.h"
#include <time.h>

#define UUID_LENGTH 45

std::string midasUUID::GenerateUUID()
{
  srand ( (unsigned int)time(NULL) );
  // Generate a random number for the uuid.
  std::stringstream randomchars;
  for(unsigned int i = 0; i < UUID_LENGTH; i++)
    {
    int nextDigit = rand() % 16;
    randomchars << std::hex << nextDigit;
    }
  return randomchars.str();
}

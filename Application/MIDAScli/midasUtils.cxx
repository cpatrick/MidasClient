/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "midasUtils.h"
#include <time.h>

#define UUID_LENGTH 45

//-------------------------------------------------------------------
std::string midasUtils::GenerateUUID()
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

//-------------------------------------------------------------------
std::string midasUtils::EscapeForURL(std::string s)
{
  kwsys::SystemTools::ReplaceString(s, "%", "%25");
  kwsys::SystemTools::ReplaceString(s, " ", "%20");
  kwsys::SystemTools::ReplaceString(s, "&", "%26");
  kwsys::SystemTools::ReplaceString(s, "=", "%3D");
  kwsys::SystemTools::ReplaceString(s, "?", "%3F");
  kwsys::SystemTools::ReplaceString(s, ",", "%2C");
  kwsys::SystemTools::ReplaceString(s, "+", "%2B");
  kwsys::SystemTools::ReplaceString(s, "$", "%24");
  kwsys::SystemTools::ReplaceString(s, ":", "%3A");
  kwsys::SystemTools::ReplaceString(s, ";", "%3B");
  kwsys::SystemTools::ReplaceString(s, "/", "%2F");
  kwsys::SystemTools::ReplaceString(s, "@", "%40");
  kwsys::SystemTools::ReplaceString(s, "\"", "%22");
  kwsys::SystemTools::ReplaceString(s, "<", "%3C");
  kwsys::SystemTools::ReplaceString(s, ">", "%3E");
  kwsys::SystemTools::ReplaceString(s, "#", "%23");

  return s;
}

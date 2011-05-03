/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "midasStdOutLog.h"
#include <iostream>

midasStdOutLog::midasStdOutLog()
{
}

midasStdOutLog::~midasStdOutLog()
{
}

void midasStdOutLog::Error(std::string text)
{
  std::cerr << text;
}

void midasStdOutLog::Message(std::string text)
{
  std::cout << text;
}

void midasStdOutLog::Status(std::string text)
{
  //no-op in the CLI view until we get a curses view :)
}


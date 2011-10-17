/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "midasLogAware.h"

midasLogAware::midasLogAware()
{
  Log = NULL;
}

midasLogAware::~midasLogAware()
{
}

midasLog * midasLogAware::GetLog()
{
  return Log;
}

void midasLogAware::SetLog(midasLog* log)
{
  Log = log;
}

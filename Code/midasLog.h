/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef MIDASLOG_H
#define MIDASLOG_H

#include "midasStandardIncludes.h"

/**
 * Abstract class for logging messages
 */
class midasLog
{
  public:
    midasLog() {}
    virtual ~midasLog() {}

    virtual void Error(const std::string& text) = 0; 
    virtual void Message(const std::string& text) = 0;
    virtual void Status(const std::string& text) = 0;
};

#endif

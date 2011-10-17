/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __midasStdOutLog_H
#define __midasStdOutLog_H

#include "midasLog.h"

/**
 * Logger to stdout/stderr -- used in the CLI
 */
class midasStdOutLog : public midasLog
{
public:
  midasStdOutLog();
  ~midasStdOutLog();

  void Error(std::string text);

  void Message(std::string text);

  void Status(std::string text);

};

#endif

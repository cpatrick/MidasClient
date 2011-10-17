/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef MIDASLOGAWARE_H
#define MIDASLOGAWARE_H

#include "midasStandardIncludes.h"
#include "midasLog.h"

/**
 * A class that needs access to the log should extend this class
 */
class midasLogAware
{
public:

  midasLogAware();
  virtual ~midasLogAware();

  virtual midasLog * GetLog();
  virtual void SetLog(midasLog* log);

protected:
  midasLog* Log;
};

#endif

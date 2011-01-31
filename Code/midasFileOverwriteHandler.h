/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef MIDASFILEOVERWRITEHANDLER_H
#define MIDASFILEOVERWRITEHANDLER_H

#include "midasStandardIncludes.h"

/**
 * Abstract class for handling when a user attempts to download
 * a file to a location that already exists
 */
class midasFileOverwriteHandler
{
public:
  midasFileOverwriteHandler() {}
  virtual ~midasFileOverwriteHandler() {}

  enum Action {
    UseExisting = 0,
    Overwrite = 1
  };

  virtual Action HandleConflict(std::string path) = 0;
};

#endif

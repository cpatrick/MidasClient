/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef MIDASPROGRESSREPORTER_H
#define MIDASPROGRESSREPORTER_H

#include "midasStandardIncludes.h"

/**
 * Abstract class for updating progress
 */
class midasProgressReporter
{
  public:
    midasProgressReporter() {}
    ~midasProgressReporter() {}

    virtual void UpdateProgress(double current, double max) = 0;
    virtual void ResetProgress() = 0;
    virtual void SetMessage(std::string message) = 0;
    virtual void SetIndeterminate() = 0;
};

#endif
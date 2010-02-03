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

// The default length of the progress bar
#define DEFAULT_LENGTH 25

class midasProgressReporter
{
  public:
    midasProgressReporter(int length = DEFAULT_LENGTH);
    ~midasProgressReporter();

    void UpdateProgress(double current, double max);
  protected:
    void PrintBar();

    int currLength;
    int oldLength;
    int maxLength;
};

#endif
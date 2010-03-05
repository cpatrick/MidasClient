/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef MIDASDOTPROGRESSREPORTER_H
#define MIDASDOTPROGRESSREPORTER_H

#include "midasStandardIncludes.h"
#include "midasProgressReporter.h"

// The default length of the progress bar
#define DEFAULT_LENGTH 25

class midasDotProgressReporter : midasProgressReporter
{
  public:
    midasDotProgressReporter(int length = DEFAULT_LENGTH);
    ~midasDotProgressReporter();

    void UpdateProgress(double current, double max);
    void ResetProgress();
    void SetMessage(std::string message);
  protected:
    void PrintBar();

    bool Done;
    int currLength;
    int oldLength;
    int maxLength;
};

#endif
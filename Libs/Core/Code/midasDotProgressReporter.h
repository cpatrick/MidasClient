/******************************************************************************
 * Copyright 2011 Kitware Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/


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

  void UpdateOverallCount(int value);

  void UpdateTotalProgress(double value);

  void ResetProgress();

  void SetMessage(std::string message);

  void SetIndeterminate();

  void SetMaxCount(int max);

protected:
  void PrintBar();

  bool Done;
  int  currLength;
  int  oldLength;
  int  maxLength;
};

#endif

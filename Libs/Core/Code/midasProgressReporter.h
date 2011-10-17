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


#ifndef MIDASPROGRESSREPORTER_H
#define MIDASPROGRESSREPORTER_H

#include "midasStandardIncludes.h"

/**
 * Abstract class for updating progress
 */
class midasProgressReporter
{
public:

  midasProgressReporter();
  virtual ~midasProgressReporter();

  virtual void UpdateProgress(double current, double max) = 0;

  virtual void UpdateOverallCount(int value) = 0;

  virtual void UpdateTotalProgress(double current) = 0;

  virtual void SetMaxCount(int maxCount);

  virtual void SetMaxTotal(double maxTotal);

  virtual void ResetProgress() = 0;

  virtual void ResetOverall();

  virtual void SetMessage(std::string message) = 0;

  virtual void SetIndeterminate() = 0;

  virtual void SetUnit(const std::string& unit);

  virtual std::string GetUnit();

protected:
  int         m_MaxCount;
  double      m_MaxTotal;
  double      m_Total;
  std::string m_Unit;
};

#endif

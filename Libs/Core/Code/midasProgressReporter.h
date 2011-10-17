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

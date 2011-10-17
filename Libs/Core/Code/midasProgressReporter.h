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
  midasProgressReporter() : m_MaxCount(0), m_MaxTotal(0), m_Total(0), m_Unit("B")
  {
  }
  virtual ~midasProgressReporter()
  {
  }

  virtual void UpdateProgress(double current, double max) = 0;

  virtual void UpdateOverallCount(int value) = 0;

  virtual void UpdateTotalProgress(double current) = 0;

  virtual void SetMaxCount(int maxCount)
  {
    m_MaxCount = maxCount;
  }
  virtual void SetMaxTotal(double maxTotal)
  {
    m_MaxTotal = maxTotal;
  }
  virtual void ResetProgress() = 0;

  virtual void ResetOverall()
  {
    m_Total = 0;
  }
  virtual void SetMessage(std::string message) = 0;

  virtual void SetIndeterminate() = 0;

  virtual void SetUnit(const std::string& unit)
  {
    m_Unit = unit;
  }
  virtual std::string GetUnit()
  {
    return m_Unit;
  }
protected:
  int         m_MaxCount;
  double      m_MaxTotal;
  double      m_Total;
  std::string m_Unit;
};

#endif

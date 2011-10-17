/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "midasProgressReporter.h"

midasProgressReporter::midasProgressReporter() 
  : m_MaxCount(0), m_MaxTotal(0), m_Total(0), m_Unit("B")
{
}

midasProgressReporter::~midasProgressReporter()
{
}

void midasProgressReporter::SetMaxCount(int maxCount)
{
  m_MaxCount = maxCount;
}

void midasProgressReporter::SetMaxTotal(double maxTotal)
{
  m_MaxTotal = maxTotal;
}

void midasProgressReporter::ResetOverall()
{
  m_Total = 0;
}

void midasProgressReporter::SetUnit(const std::string& unit)
{
  m_Unit = unit;
}

std::string midasProgressReporter::GetUnit()
{
  return m_Unit;
}

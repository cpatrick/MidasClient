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

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


#include "midasDotProgressReporter.h"

midasDotProgressReporter::midasDotProgressReporter(int length)
{
  this->ResetProgress();
  this->maxLength = length;
}

midasDotProgressReporter::~midasDotProgressReporter()
{
}

void midasDotProgressReporter::UpdateProgress(double current, double max)
{
  if( max == 0 || this->Done )
    {
    return;
    }
  double fraction = current / max;

  this->currLength = static_cast<int>(
      fraction * static_cast<double>(maxLength) );

  this->PrintBar();
  if( current == max )
    {
    this->Done = true;
    std::cout << " Done" << std::endl;
    }
}

void midasDotProgressReporter::SetMaxCount(int max)
{
  midasProgressReporter::SetMaxCount(max);
  std::cout << "Total count: " << max << std::endl;
}

void midasDotProgressReporter::UpdateOverallCount(int value)
{
  (void)value;
}

void midasDotProgressReporter::UpdateTotalProgress(double value)
{
  (void)value;
}

void midasDotProgressReporter::PrintBar()
{
  int toWrite = this->currLength - this->oldLength;
  for( int i = 0; i < toWrite; i++ )
    {
    std::cout << ".";
    }

  this->oldLength = this->currLength;
}

void midasDotProgressReporter::SetMessage(std::string message)
{
  std::cout << std::setw(32) << message << "  ";
}

void midasDotProgressReporter::ResetProgress()
{
  this->Done = false;
  this->oldLength = 0;
  this->currLength = 0;
}

void midasDotProgressReporter::SetIndeterminate()
{
}


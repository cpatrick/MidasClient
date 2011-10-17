/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

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
  if (max == 0 || this->Done) return;
  double fraction = current / max;

  this->currLength = static_cast<int>(
    fraction * static_cast<double>(maxLength));
  
  this->PrintBar();
  if(current == max)
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

  for(int i = 0; i < toWrite; i++)
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

/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "midasProgressReporter.h"

midasProgressReporter::midasProgressReporter(int length)
{
  this->oldLength = 0;
  this->currLength = 0;
  this->maxLength = length;
}

midasProgressReporter::~midasProgressReporter()
{
}

void midasProgressReporter::UpdateProgress(double current, double max)
{
  if (max == 0) return;
  double fraction = current / max;

  this->currLength = (int)(fraction * (double)maxLength);
  
  this->PrintBar();
}

void midasProgressReporter::PrintBar()
{
  int toWrite = this->currLength - this->oldLength;

  for(int i = 0; i < toWrite; i++)
    {
    std::cout << ".";
    }

  this->oldLength = this->currLength;
}

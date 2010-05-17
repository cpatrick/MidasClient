/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "GUIProgress.h"

#include <QProgressBar>

GUIProgress::GUIProgress(QProgressBar* progressBar)
{
  this->m_progressBar = progressBar;
  this->m_progressBar->setMaximum(100);
  this->m_progressBar->setTextVisible(false);
  this->ResetProgress();
}

GUIProgress::~GUIProgress()
{
}

void GUIProgress::UpdateProgress(double current, double max)
{
  if (max == 0 || this->Done) return;
  double fraction = current / max;
  int percent = static_cast<int>(fraction * 100.0);
  
  this->m_progressBar->setValue(percent);

  if(current == max)
    {
    this->Done = true;
    }
}

void GUIProgress::SetMessage(std::string message)
{
}

void GUIProgress::ResetProgress()
{
  this->Done = false;
  this->m_progressBar->setValue(0);
}

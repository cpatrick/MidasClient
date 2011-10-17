/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "GUIProgress.h"

GUIProgress::GUIProgress(QProgressBar* progressBar)
{
  this->m_progressBar = progressBar;
  this->m_progressBar->setMinimum(0);
  this->m_progressBar->setMaximum(100);
  this->m_progressBar->setTextVisible(false);
  this->ResetProgress();
  this->LastAmount = 0;
  this->StartTime = 0;
  this->StartAmount = 0;

  connect(this, SIGNAL(UpdateProgressMin(int)),
    m_progressBar, SLOT( setMinimum(int) ) );
  connect(this, SIGNAL(UpdateProgressMax(int)),
    m_progressBar, SLOT( setMaximum(int) ) );
  connect(this, SIGNAL(UpdateProgressValue(int)),
    m_progressBar, SLOT( setValue(int) ) );
}

GUIProgress::~GUIProgress()
{
}

void GUIProgress::UpdateProgress(double current, double max)
{
  m_Total += (current - this->LastAmount); //record difference
  this->LastAmount = current;
  emit CurrentProgress(current, max);
  emit OverallProgressTotal(m_Total, m_MaxTotal);

  if(current == max)
    {
    this->LastAmount = 0;
    this->StartTime = 0;
    this->StartAmount = 0;
    }

  double currentTime = midasUtils::CurrentTime();
  if(this->StartTime == 0)
    {
    this->StartTime = currentTime;
    this->StartAmount = current;
    }
  else if(currentTime - this->StartTime > SPEED_CALC_INTERVAL)
    {
    double speed = (current - this->StartAmount) / (currentTime - this->StartTime);
    emit Speed(speed);

    double estimatedTimeLeft = (m_MaxTotal - m_Total) / speed;
    emit EstimatedTime(estimatedTimeLeft);

    this->StartTime = currentTime;
    this->StartAmount = current;
    }
}

void GUIProgress::UpdateOverallCount(int value)
{
  emit OverallProgressCount(value, m_MaxCount);
}

void GUIProgress::UpdateTotalProgress(double value)
{
  m_Total += value;
  emit OverallProgressTotal(m_Total, m_MaxTotal);
}

void GUIProgress::SetMessage(std::string message)
{
  emit ProgressMessage(message.c_str());
}

void GUIProgress::SetIndeterminate()
{
  emit UpdateProgressMin(0);
  emit UpdateProgressMax(0);
}

void GUIProgress::ResetProgress()
{
  this->StartAmount = 0;
  this->StartTime = 0;
  this->LastAmount = 0;
  this->Done = false;
  emit UpdateProgressMax(100);
  emit UpdateProgressValue(0);
}

void GUIProgress::ResetOverall()
{
  m_Total = 0;
  m_MaxTotal = 0;
  emit OverallProgressTotal(0, 0);
}

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
  this->Speeds = new RingBufferD(SPEEDS_RINGBUFFER_SIZE);
  this->LastTime = 0;
  this->LastAmount = 0;

  connect(this, SIGNAL(UpdateProgressMin(int)),
    m_progressBar, SLOT( setMinimum(int) ) );
  connect(this, SIGNAL(UpdateProgressMax(int)),
    m_progressBar, SLOT( setMaximum(int) ) );
  connect(this, SIGNAL(UpdateProgressValue(int)),
    m_progressBar, SLOT( setValue(int) ) );
}

GUIProgress::~GUIProgress()
{
  delete this->Speeds;
}

void GUIProgress::UpdateProgress(double current, double max)
{
  double currentTime = kwsys::SystemTools::GetTime();
  emit CurrentProgress(current, max);
  if (max == 0 || this->Done) return;
  double fraction = current / max;
  int percent = static_cast<int>(fraction * 100.0);
  
  emit UpdateProgressMax(100);
  emit UpdateProgressValue(percent);

  if(this->LastTime != 0)
    {
    double elapsedTime = currentTime - this->LastTime;
    double bytesDownloaded = current - this->LastAmount;
    double speed = bytesDownloaded / elapsedTime;
    emit Speed(speed); //"instantaneous" speed of upload/download
    this->Speeds->insert(speed);

    m_Total += bytesDownloaded;
    }
  else
    {
    emit Speed(0);

    m_Total += current;
    }
  this->LastTime = currentTime;
  this->LastAmount = current;

  if(current == max)
    {
    this->Done = true;
    this->LastTime = 0;
    }

  double estimatedTimeLeft = (m_MaxTotal - m_Total) / this->Speeds->avg();
  emit EstimatedTime(estimatedTimeLeft); //smooth ETA
  emit OverallProgressTotal(m_Total, m_MaxTotal);
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
  this->Done = false;
  emit UpdateProgressMax(100);
  emit UpdateProgressValue(0);
}

//--- RingBuffer impl ---------------------------------------------
RingBufferD::RingBufferD(int size)
: m_Size(size)
{
  m_Buffer = new double[size];
  for(int i = 0; i < size; i++)
    {
    m_Buffer[i] = 0;
    }
  m_Index = 0;
}

RingBufferD::~RingBufferD()
{
  delete m_Buffer;
}

void RingBufferD::insert(double val)
{
  m_Buffer[m_Index] = val;
  m_Index = (m_Index + 1) % m_Size;
}

double RingBufferD::avg()
{
  double total = 0;
  int count = 0;
  for(int i = 0; i < m_Size; i++)
    {
    if(m_Buffer[i] != 0)
      {
      count++;
      total += m_Buffer[i];
      }
    }
  if(count > 0)
    {
    return total / (double) count;
    }
  else
    {
    return 0;
    }
}

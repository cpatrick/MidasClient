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


#ifndef __GUIProgress_H
#define __GUIProgress_H

#include "midasProgressReporter.h"
#include <QObject>
#include <QProgressBar>

/** How many seconds to wait between calculating speed and ETA */
#define SPEED_CALC_INTERVAL 1.0

class GUIProgress : public QObject, public midasProgressReporter
{
  Q_OBJECT
public:
  GUIProgress(QProgressBar* progressBar);
  ~GUIProgress();

  void UpdateProgress(double current, double max);

  void UpdateOverallCount(int value);

  /** Don't call this in a progress callback, only call it if you are skipping
   *  the progress callback */
  void UpdateTotalProgress(double current);

  void ResetProgress();

  void ResetOverall();

  void SetMessage(std::string message);

  void SetIndeterminate();

signals:
  void UpdateProgressMin(int val);

  void UpdateProgressMax(int val);

  void UpdateProgressValue(int val);

  void ProgressMessage(const QString& message);

  void OverallProgressCount(int current, int max);

  void OverallProgressTotal(double current, double max);

  void CurrentProgress(double current, double max);

  void Speed(double bytesPerSec);

  void EstimatedTime(double secondsLeft);

protected:
  QProgressBar* m_progressBar;

  bool   Done;
  double LastAmount;
  double StartTime;
  double StartAmount;
};

#endif

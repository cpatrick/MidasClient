/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

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

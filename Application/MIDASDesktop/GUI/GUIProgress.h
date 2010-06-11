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

class GUIProgress : public QObject, public midasProgressReporter
{
  Q_OBJECT
public:
  GUIProgress(QProgressBar* progressBar);
  ~GUIProgress();

  void UpdateProgress(double current, double max);
  void ResetProgress();
  void SetMessage(std::string message);
  void SetIndeterminate();

signals:
  void UpdateProgressMin(int val);
  void UpdateProgressMax(int val);
  void UpdateProgressValue(int val);
protected:
  QProgressBar* m_progressBar;

  bool Done;
};

#endif
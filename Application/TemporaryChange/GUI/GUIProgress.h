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

class QProgressBar;

class GUIProgress : midasProgressReporter
{
  public:
    GUIProgress(QProgressBar* progressBar);
    ~GUIProgress();

    void UpdateProgress(double current, double max);
    void ResetProgress();
    void SetMessage(std::string message);
  protected:
    QProgressBar* m_progressBar;

    bool Done;
};

#endif
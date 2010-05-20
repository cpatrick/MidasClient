/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __GUILogger_H
#define __GUILogger_H

#include "midasLog.h"
#include <QTextEdit>

class GUILogger : public midasLog
{
public:
  GUILogger(QTextEdit* log);
  ~GUILogger();

  void Error(std::string text);
  void Message(std::string text);

private:
  QTextEdit* m_log;
};

#endif
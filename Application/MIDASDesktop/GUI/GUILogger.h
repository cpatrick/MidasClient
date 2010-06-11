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

#include <QObject>
#include <QTextEdit>
#include <QString>
#include <QColor>

class GUILogger : public QObject, public midasLog
{
  Q_OBJECT
public:
  GUILogger(QTextEdit* log);
  ~GUILogger();

  void Error(std::string text);
  void Message(std::string text);

signals:
  void Text(const QString& text);
  void ChangeTextColor(const QColor& text);

private:
  QTextEdit* m_log;
};

#endif
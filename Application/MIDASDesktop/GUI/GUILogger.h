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
#include <QString>
#include <QColor>

class MIDASDesktopUI;

class GUILogger : public QObject, public midasLog
{
  Q_OBJECT
public:
  GUILogger(MIDASDesktopUI* parent);
  ~GUILogger();

  void Error(std::string text);
  void Message(std::string text);
  void Status(std::string text);

signals:
  void Status(const QString& text);
  void Text(const QString& text);
  void ChangeTextColor(const QColor& text);
  void ErrorOccurred();

private:
  MIDASDesktopUI* m_Parent;
};

#endif
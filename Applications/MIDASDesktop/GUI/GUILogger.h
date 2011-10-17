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
public slots:
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

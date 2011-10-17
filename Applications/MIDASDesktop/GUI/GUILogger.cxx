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


#include "GUILogger.h"
#include "MIDASDesktopUI.h"

#include <QColor>
#include <QString>
#include <QTextEdit>

inline void removeNewLines(std::string& text)
{
  QString qstr(text.c_str() );

  qstr = qstr.replace('\r', "");
  qstr = qstr.replace('\n', "");
  text = qstr.toStdString();
}

GUILogger::GUILogger(MIDASDesktopUI* parent) : m_Parent(parent)
{
  connect(this, SIGNAL(ChangeTextColor(const QColor &) ),
          m_Parent->GetLogTextEdit(), SLOT( setTextColor(const QColor &) ) );
  connect(this, SIGNAL(Text(const QString &) ),
          m_Parent->GetLogTextEdit(), SLOT( append(const QString &) ) );
  connect(this, SIGNAL(Status(const QString &) ),
          m_Parent, SLOT( DisplayStatus(const QString &) ) );
  connect(this, SIGNAL(ErrorOccurred() ),
          m_Parent, SLOT( AlertErrorInLog() ) );
}

GUILogger::~GUILogger()
{
}

void GUILogger::Error(std::string text)
{
  removeNewLines(text);

  emit ChangeTextColor(QColor(255, 0, 0) );
  emit Text(QString(text.c_str() ) );
  emit ErrorOccurred();
}

void GUILogger::Message(std::string text)
{
  removeNewLines(text);

  emit ChangeTextColor(QColor(0, 0, 0) );
  emit Text(QString(text.c_str() ) );
}

void GUILogger::Status(std::string text)
{
  emit Status(QString(text.c_str() ) );
}


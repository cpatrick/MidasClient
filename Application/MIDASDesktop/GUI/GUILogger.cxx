/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "GUILogger.h"

#include <QColor>
#include <QString>

inline void removeNewLines(std::string& text)
{
  QString qstr(text.c_str());
  qstr = qstr.replace('\r', "");
  qstr = qstr.replace('\n', "");
  text = qstr.toStdString();
}

GUILogger::GUILogger(QTextEdit* log)
{
  this->m_log = log;

  connect(this, SIGNAL(ChangeTextColor(const QColor&)),
    m_log, SLOT( setTextColor(const QColor&) ) );
  connect(this, SIGNAL(Text(const QString&)),
    m_log, SLOT( append(const QString&) ) );
}

GUILogger::~GUILogger()
{
}

void GUILogger::Error(std::string text)
{
  removeNewLines(text);

  emit ChangeTextColor(QColor(255, 0, 0));
  emit Text(QString(text.c_str()));
}

void GUILogger::Message(std::string text)
{
  removeNewLines(text);

  emit ChangeTextColor(QColor(0, 0, 0));
  emit Text(QString(text.c_str()));
}

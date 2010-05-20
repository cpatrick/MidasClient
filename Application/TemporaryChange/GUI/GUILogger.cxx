/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <QColor>
#include <QString>
#include "GUILogger.h"

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
  //this->m_log->setAutoFormatting(QTextEdit::AutoNone);
}

GUILogger::~GUILogger()
{
}

void GUILogger::Error(std::string text)
{
  removeNewLines(text);
  this->m_log->setTextColor(QColor(255, 0, 0));
  this->m_log->append(text.c_str());
  //emit this->m_log->textChanged();
}

void GUILogger::Message(std::string text)
{
  removeNewLines(text);
  this->m_log->setTextColor(QColor(0, 0, 0));
  this->m_log->append(text.c_str());
  //emit this->m_log->textChanged();
}

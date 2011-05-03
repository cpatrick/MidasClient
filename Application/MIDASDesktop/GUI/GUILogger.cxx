/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "GUILogger.h"
#include "MIDASDesktopUI.h"

#include <QColor>
#include <QString>
#include <QTextEdit>

inline void removeNewLines(std::string& text)
{
  QString qstr(text.c_str());
  qstr = qstr.replace('\r', "");
  qstr = qstr.replace('\n', "");
  text = qstr.toStdString();
}

GUILogger::GUILogger(MIDASDesktopUI* parent) : m_Parent(parent)
{
  connect(this, SIGNAL(ChangeTextColor(const QColor&)),
    m_Parent->getLogTextEdit(), SLOT( setTextColor(const QColor&) ) );
  connect(this, SIGNAL(Text(const QString&)),
    m_Parent->getLogTextEdit(), SLOT( append(const QString&) ) );
  connect(this, SIGNAL(Status(const QString&)),
    m_Parent, SLOT( displayStatus(const QString&) ) );
  connect(this, SIGNAL(ErrorOccurred()),
    m_Parent, SLOT( alertErrorInLog() ) );
}

GUILogger::~GUILogger()
{
}

void GUILogger::Error(std::string text)
{
  removeNewLines(text);

  emit ChangeTextColor(QColor(255, 0, 0));
  emit Text(QString(text.c_str()));
  emit ErrorOccurred();
}

void GUILogger::Message(std::string text)
{
  removeNewLines(text);

  emit ChangeTextColor(QColor(0, 0, 0));
  emit Text(QString(text.c_str()));
}

void GUILogger::Status(std::string text)
{
  emit Status(QString(text.c_str()));
}

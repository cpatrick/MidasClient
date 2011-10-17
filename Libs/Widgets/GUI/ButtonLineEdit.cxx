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
#include "ButtonLineEdit.h"
#include "ButtonEditUI.h"
#include "MidasItemTreeItem.h"

#include <QBoxLayout>

ButtonLineEdit::ButtonLineEdit(MidasItemTreeItem* item, MIDASFields field, ButtonEditUI* handler, QWidget* parent,
                               std::string text)
  : QWidget(parent), m_Field(field), m_Item(item)
{
  m_TextEdit = new QLineEdit();
  // m_TextEdit->setReadOnly(true);

  m_AddButton = new QPushButton();
  m_AddButton->setText(text.c_str() );

  QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight, parent);
  layout->setSpacing(0);
  layout->setMargin(0);
  layout->addWidget(m_TextEdit);
  layout->addWidget(m_AddButton);

  setLayout(layout);

  connect(m_AddButton, SIGNAL( released() ),
          handler, SLOT( exec() ) );
  connect(handler, SIGNAL( text(const QString &) ),
          this, SLOT( appendText(const QString &) ) );
}

ButtonLineEdit::~ButtonLineEdit()
{
  delete this->layout();
  delete m_TextEdit;
  delete m_AddButton;
}

void ButtonLineEdit::appendText(const QString& text)
{
  QString value = m_TextEdit->text().trimmed();

  value = value == "" ? text : value + " / " + text;

  m_TextEdit->setText(value);
}

QString ButtonLineEdit::getData()
{
  return m_TextEdit->text().trimmed();
}

void ButtonLineEdit::setData(const QString& value)
{
  m_TextEdit->setText(value);
}


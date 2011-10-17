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
#ifndef __ButtonLineEdit_H
#define __ButtonLineEdit_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

#include "MidasResourceDescTable.h"

class MidasItemTreeItem;
class ButtonEditUI;

class ButtonLineEdit : public QWidget
{
  Q_OBJECT
public:
  ButtonLineEdit(MidasItemTreeItem* item, MIDASFields field, ButtonEditUI* handler, QWidget* parent = 0,
                 std::string text = "Add");
  ~ButtonLineEdit();

  QString getData();

  void setData(const QString& value);

public slots:
  void appendText(const QString& text);

protected:
  QLineEdit*   m_TextEdit;
  QPushButton* m_AddButton;

  MIDASFields        m_Field;
  MidasItemTreeItem* m_Item;
};

#endif

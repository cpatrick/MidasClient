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
#ifndef __ButtonDelegate_H
#define __ButtonDelegate_H

#include "MidasResourceDescTable.h"

#include <QItemDelegate>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QWidget>

class MidasItemTreeItem;
class ButtonEditUI;

class ButtonDelegate : public QItemDelegate
{
  Q_OBJECT
public:
  ButtonDelegate(QObject* parent = 0);
  ~ButtonDelegate();

  void setItem(MidasItemTreeItem* item);

  void setField(MIDASFields field);

  void setEditUI(ButtonEditUI* editUI);

  QWidget * createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;

  void setEditorData(QWidget* editor, const QModelIndex& index) const;

  void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;

  void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;

protected:

  MidasItemTreeItem* m_Item;
  ButtonEditUI*      m_EditUI;
  MIDASFields        m_Field;
};

#endif

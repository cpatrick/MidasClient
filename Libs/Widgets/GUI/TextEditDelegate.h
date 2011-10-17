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
#ifndef __TextEditDelegate_H
#define __TextEditDelegate_H

#include "MidasResourceDescTable.h"

#include <QItemDelegate>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QWidget>

class MidasTreeItem;

class TextEditDelegate : public QItemDelegate
{
  Q_OBJECT
public:
  TextEditDelegate(QObject* parent = 0);
  ~TextEditDelegate();

  void setItem(MidasTreeItem* item);

  void setField(MIDASFields field);

  QWidget * createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;

  void setEditorData(QWidget* editor, const QModelIndex& index) const;

  void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;

  void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;

protected:

  MidasTreeItem* m_Item;
  MIDASFields    m_Field;
};

#endif

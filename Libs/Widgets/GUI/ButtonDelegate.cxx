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
#include "ButtonDelegate.h"
#include "ButtonLineEdit.h"
#include "ButtonEditUI.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QLineEdit>

ButtonDelegate::ButtonDelegate(QObject* parent)
  : QItemDelegate(parent)
{
  m_EditUI = NULL;
  m_Item = NULL;
}

ButtonDelegate::~ButtonDelegate()
{
}

void ButtonDelegate::setItem(MidasItemTreeItem* item)
{
  m_Item = item;
}

void ButtonDelegate::setField(MIDASFields field)
{
  m_Field = field;
}

void ButtonDelegate::setEditUI(ButtonEditUI* editUI)
{
  m_EditUI = editUI;
}

QWidget * ButtonDelegate::createEditor(QWidget* parent,
                                       const QStyleOptionViewItem& option,
                                       const QModelIndex& index) const
{
  (void)option;
  (void)index;
  return new ButtonLineEdit(m_Item, m_Field, m_EditUI, parent);
}

void ButtonDelegate::setEditorData(QWidget* editor,
                                   const QModelIndex& index) const
{
  ButtonLineEdit* edit = static_cast<ButtonLineEdit *>(editor);
  std::string     value = index.model()->data(
      index, Qt::DisplayRole).toString().toStdString();

  edit->setData(value.c_str() );
}

void ButtonDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                  const QModelIndex& index) const
{
  (void)index;
  ButtonLineEdit* edit = static_cast<ButtonLineEdit *>(editor);
  model->setData(index, edit->getData(), Qt::EditRole);
}

void ButtonDelegate::updateEditorGeometry(QWidget* editor,
                                          const QStyleOptionViewItem & option,
                                          const QModelIndex& index) const
{
  (void)index;
  editor->setGeometry(option.rect);
}


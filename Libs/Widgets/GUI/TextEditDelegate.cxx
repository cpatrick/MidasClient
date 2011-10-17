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
#include "TextEditDelegate.h"
#include "MidasTreeItem.h"

#include <QTextEdit>
#include <QPainter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

TextEditDelegate::TextEditDelegate(QObject* parent)
  : QItemDelegate(parent)
{
  m_Item = NULL;
}

TextEditDelegate::~TextEditDelegate()
{
}

void TextEditDelegate::setItem(MidasTreeItem* item)
{
  m_Item = item;
}

void TextEditDelegate::setField(MIDASFields field)
{
  m_Field = field;
}

QWidget * TextEditDelegate::createEditor(QWidget* parent,
                                         const QStyleOptionViewItem& option,
                                         const QModelIndex& index) const
{
  (void)option;
  (void)index;
  QTextEdit* editor = new QTextEdit(parent);
  editor->setMinimumHeight(100);
  return editor;
}

void TextEditDelegate::setEditorData(QWidget* editor,
                                     const QModelIndex& index) const
{
  QTextEdit*  edit = static_cast<QTextEdit *>(editor);
  std::string value = index.model()->data(
      index, Qt::DisplayRole).toString().toStdString();

  edit->setText(value.c_str() );
}

void TextEditDelegate::setModelData(QWidget* editor,
                                    QAbstractItemModel* model,
                                    const QModelIndex& index) const
{
  QTextEdit* edit = static_cast<QTextEdit *>(editor);

  model->setData(index, edit->toPlainText(), Qt::EditRole);
}

void TextEditDelegate::updateEditorGeometry(QWidget* editor,
                                            const QStyleOptionViewItem & option,
                                            const QModelIndex& index) const
{
  (void)index;
  editor->setGeometry(option.rect);
}


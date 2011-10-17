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

QWidget* TextEditDelegate::createEditor(QWidget* parent,
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
  QTextEdit* edit = static_cast<QTextEdit*>(editor);
  std::string value = index.model()->data(
    index, Qt::DisplayRole).toString().toStdString();
  
  edit->setText(value.c_str());
}

void TextEditDelegate::setModelData(QWidget* editor,
                                    QAbstractItemModel* model,
                                    const QModelIndex& index) const
{
  QTextEdit* edit = static_cast<QTextEdit*>(editor);
  model->setData(index, edit->toPlainText(), Qt::EditRole);
}

void TextEditDelegate::updateEditorGeometry(QWidget* editor,
                                          const QStyleOptionViewItem &option,
                                          const QModelIndex& index) const
{
  (void)index;
  editor->setGeometry(option.rect);
}

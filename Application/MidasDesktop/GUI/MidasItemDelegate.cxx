#include "MidasItemDelegate.h"

#include <QSpinBox>
#include <QLineEdit>

#include "Logger.h"
#include "MidasClientGlobal.h"

EditTextDelegate::EditTextDelegate(QObject *parent)
  : QItemDelegate(parent)
  {
  //LOGGER_FUNC(); 
  }

QWidget *EditTextDelegate::createEditor(QWidget *parent,
   const QStyleOptionViewItem &/* option */,
   const QModelIndex &/* index */) const
  {
  LOGGER_FUNC_START();
  QLineEdit *editor = new QLineEdit(parent);
  LOGGER_FUNC_END();
  return editor;
  }

void EditTextDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
  {
  LOGGER_FUNC_START();
  QString value = index.model()->data(index, Qt::EditRole).toString();
  QLineEdit *lineedit = static_cast<QLineEdit*>(editor);
  lineedit->setText( value );
  LOGGER_FUNC_END();
  }

void EditTextDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
  {
  LOGGER_FUNC_START(); 
  QLineEdit *lineedit = static_cast<QLineEdit*>(editor);
  //lineedit->interpretText();
  QString value = lineedit->text();

  model->setData(index, value, Qt::EditRole);
  LOGGER_FUNC_END();
  }

void EditTextDelegate::updateEditorGeometry(QWidget *editor,
  const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
  {
  LOGGER_FUNC_START();
  editor->setGeometry(option.rect);
  LOGGER_FUNC_END();
  }
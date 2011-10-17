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


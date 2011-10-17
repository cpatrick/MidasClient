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

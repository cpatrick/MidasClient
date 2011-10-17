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

#ifndef __MidasItemDelegate_H
#define __MidasItemDelegate_H

#include <QItemDelegate>

class EditTextDelegate : public QItemDelegate
{
  Q_OBJECT

public:
  EditTextDelegate(QObject *parent = 0);

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                       const QModelIndex &index) const;

  void setEditorData(QWidget *editor, const QModelIndex &index) const;
  void setModelData(QWidget *editor, QAbstractItemModel *model,
                     const QModelIndex &index) const;

  void updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif //__MidasItemDelegate_H
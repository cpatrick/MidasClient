#ifndef __MidasTreeViewBase_H
#define __MidasTreeViewBase_H

#include <QTreeView>
#include <QWidget>

class midasSynchronizer;

class MidasTreeViewBase : public QTreeView
{
  Q_OBJECT

public:

  MidasTreeViewBase(QWidget* parent) : QTreeView(parent)
    {
    this->setAlternatingRowColors(true);
    this->setHeaderHidden(true);
    }
  virtual ~MidasTreeViewBase() {}

  virtual void SetSynchronizer(midasSynchronizer* synch) = 0;

  virtual void Clear() = 0;
  virtual void Initialize() = 0;
  virtual void Update() = 0;
};

#endif

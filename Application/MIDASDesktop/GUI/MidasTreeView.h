#ifndef __MidasTreeView_H
#define __MidasTreeView_H

#include <QTreeView>

class MidasTreeView : public QTreeView
{
  Q_OBJECT

public:
  MidasTreeView(QWidget* parent) : QTreeView(parent) {}
};

#endif
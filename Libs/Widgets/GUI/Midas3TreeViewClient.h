#ifndef __Midas3TreeViewClient_H
#define __Midas3TreeViewClient_H

#include "Midas3TreeView.h"

class Midas3TreeModelClient;

class Midas3TreeViewClient : public Midas3TreeView
{
  Q_OBJECT
public:

  Midas3TreeViewClient(QWidget* parent = 0);
  ~Midas3TreeViewClient();
public slots:
  void expandAll();

  void collapseAll();

  void addResource(mdo::Object *);

  void updateResource(mdo::Object *);

  void deleteResource(mdo::Object *);

signals:
  void bitstreamsDropped(const Midas3ItemTreeItem* parentItem, const QStringList& files);

  void bitstreamOpenRequest();

protected:
  void fetchItemData(Midas3TreeItem* item);

  void dragMoveEvent(QDragMoveEvent* event);

  void dropEvent(QDropEvent* event);

  void mouseDoubleClickEvent(QMouseEvent* event);

};

#endif // __Midas3TreeViewClient_H

#ifndef __MidasTreeViewClient_H
#define __MidasTreeViewClient_H

#include "MidasTreeView.h"

class MidasTreeModelClient;

class MidasTreeViewClient : public MidasTreeView
{
  Q_OBJECT

public:
  
  MidasTreeViewClient(QWidget* parent = 0);
  ~MidasTreeViewClient();

public slots:
  void expandAll();
  void collapseAll();

  void addResource(mdo::Object*);
  void updateResource(mdo::Object*);
  void deleteResource(mdo::Object*);

signals:
  void bitstreamsDropped(const MidasItemTreeItem* parentItem, const QStringList& files);
  void bitstreamOpenRequest();

protected:
  void fetchItemData(MidasTreeItem* item);

  void dragMoveEvent(QDragMoveEvent* event);
  void dropEvent(QDropEvent* event);
  void mouseDoubleClickEvent(QMouseEvent* event);

};

#endif //__MidasTreeViewClient_H

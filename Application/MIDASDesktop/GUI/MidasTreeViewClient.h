#ifndef __MidasTreeViewClient_H
#define __MidasTreeViewClient_H

#include "MidasTreeView.h"
#include <string>

class MidasTreeModelClient;

class MidasTreeViewClient : public MidasTreeView
{
  Q_OBJECT

public:
  
  MidasTreeViewClient(QWidget * parent = 0);
  ~MidasTreeViewClient();

public slots:
  void expandAll();
  void collapseAll();

  void addResource(mdo::Object*);
  void updateResource(mdo::Object*);
  void deleteResource(mdo::Object*);

signals:
  void bitstreamsDropped(const MidasItemTreeItem* parentItem, const QStringList & files);
  void bitstreamOpenRequest();
  void resourceDropped(int type, int id);

protected:
  void fetchItemData(MidasTreeItem* item);

  virtual void dragEnterEvent ( QDragEnterEvent * event );
  virtual void dragLeaveEvent ( QDragLeaveEvent * event ); 
  virtual void dragMoveEvent ( QDragMoveEvent * event ); 
  virtual void dropEvent( QDropEvent * event );
  virtual void mouseDoubleClickEvent( QMouseEvent * event);

};

#endif //__MidasTreeViewClient_H

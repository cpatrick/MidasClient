#ifndef __MidasTreeViewClient_H
#define __MidasTreeViewClient_H

#include "MidasTreeView.h"
#include <string>

class MidasTreeModelClient;
class midasDatabaseProxy;

class MidasTreeViewClient : public MidasTreeView
{
  Q_OBJECT

public:
  
  MidasTreeViewClient(QWidget * parent = 0);
  ~MidasTreeViewClient();

  void SetDatabaseProxy(midasDatabaseProxy* proxy);

public slots:
  void expandAll();
  void collapseAll();

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

  midasDatabaseProxy* m_Database;
 };

#endif //__MidasTreeViewClient_H

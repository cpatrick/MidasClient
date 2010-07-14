#ifndef __MidasTreeViewClient_H
#define __MidasTreeViewClient_H

#include "MidasTreeView.h"
#include <string>

class QItemSelection;
class MidasTreeItem;
class MidasCommunityTreeItem;
class MidasCollectionTreeItem;
class MidasItemTreeItem;
class MidasBitstreamTreeItem;
class QContextMenuEvent;
class MidasTreeModelClient;
class midasDatabaseProxy;
class midasLog;

class MidasTreeViewClient : public MidasTreeView
{
  Q_OBJECT

public:
  
  /** Constructor */
  MidasTreeViewClient(QWidget * parent = 0);
  ~MidasTreeViewClient();

  void SetDatabaseProxy(midasDatabaseProxy* proxy);
  void SetLog(midasLog* log);

public slots:
  void expandAll();
  void collapseAll();

signals:

  void midasTreeViewContextMenu( QContextMenuEvent * e ); 
  void bitstreamsDropped(const MidasItemTreeItem* parentItem, const QStringList & files);
  void bitstreamOpenRequest();
  void resourceDropped(int type, int id);

protected:
  virtual void dragEnterEvent ( QDragEnterEvent * event );
  virtual void dragLeaveEvent ( QDragLeaveEvent * event ); 
  virtual void dragMoveEvent ( QDragMoveEvent * event ); 
  virtual void dropEvent( QDropEvent * event );
  virtual void mouseDoubleClickEvent( QMouseEvent * event);
  void contextMenuEvent ( QContextMenuEvent * e );
 };

#endif //__MidasTreeViewClient_H

#ifndef __MidasTreeViewServer_H
#define __MidasTreeViewServer_H

#include "MidasTreeView.h"
#include <QMutex>

class MidasTreeModelServer;
class ExpandTreeThread;

namespace mws{
  class WebAPI;
}

class MidasTreeViewServer : public MidasTreeView
{
  Q_OBJECT

public:
  
  /** Constructor */
  MidasTreeViewServer(QWidget * parent = 0); 
  ~MidasTreeViewServer();

public slots:
  void selectByUuid(std::string uuid, bool select = false);
  void alertFetchedMore();

  void expansionDone();
  void selectByIndex(const QModelIndex& index);

signals:
  void fetchedMore();
  void enableActions(bool);

  void startedExpandingTree();
  void finishedExpandingTree();

protected:
  void fetchItemData(MidasTreeItem* item);

  void dragMoveEvent(QDragMoveEvent* event);
  void dropEvent(QDropEvent* event);

  ExpandTreeThread*     m_ExpandTreeThread;
  QMutex                m_Mutex;
 };

#endif //__MidasTreeViewServer_H

#ifndef __MidasTreeViewServer_H
#define __MidasTreeViewServer_H

#include "MidasTreeView.h"
#include <QMutex>
#include <QPoint>
#include <string>

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

  /** Set the web API */
  void SetWebAPI(mws::WebAPI* api);

  void mousePressEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);

public slots:
  void selectByUuid(std::string uuid);
  void decorateByUuid(std::string uuid);
  void alertFetchedMore();

  void expansionDone();
  void selectByIndex(const QModelIndex& index);

signals:
  void fetchedMore();

  void startedExpandingTree();
  void finishedExpandingTree();

protected:
  void fetchItemData(MidasTreeItem* item);

  mws::WebAPI*          m_WebAPI;
  ExpandTreeThread*     m_ExpandTreeThread;
  QMutex                m_Mutex;
  QPoint                m_DragStart;
 };

#endif //__MidasTreeViewServer_H

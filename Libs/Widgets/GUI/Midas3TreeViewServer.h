#ifndef __Midas3TreeViewServer_H
#define __Midas3TreeViewServer_H

#include "Midas3TreeView.h"
#include <QMutex>

class Midas3TreeModelServer;
class ExpandTreeThread;

namespace mws
{
class WebAPI;
}

class Midas3TreeViewServer : public Midas3TreeView
{
  Q_OBJECT
public:

  /** Constructor */
  Midas3TreeViewServer(QWidget * parent = 0);
  ~Midas3TreeViewServer();
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
  void fetchItemData(Midas3TreeItem* item);

  void dragMoveEvent(QDragMoveEvent* event);

  void dropEvent(QDropEvent* event);

  ExpandTreeThread* m_ExpandTreeThread;
  QMutex            m_Mutex;
};

#endif // __Midas3TreeViewServer_H

#ifndef __MidasTreeModelServer_H
#define __MidasTreeModelServer_H

#include "MidasTreeModel.h"
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <string>
#include <vector>

namespace mws {
  class WebAPI;
  class Community;
} 

namespace mdo {
  class Community;
}

class MidasTreeItem;
class MidasCommunityTreeItem;
class MidasCollectionTreeItem;
class MidasItemTreeItem;

class midasSynchronizer;

class MidasTreeModelServer : public MidasTreeModel
{
  Q_OBJECT

public:

  MidasTreeModelServer(QObject* parent = 0);
  ~MidasTreeModelServer();

  void Populate();

  void fetchMore ( const QModelIndex & parent );
  void fetchCollection(MidasCollectionTreeItem* parent);
  void fetchItem(MidasItemTreeItem* parent);

signals:
  void serverPolled();
  void fetchedMore();

public slots:

  void itemExpanded ( const QModelIndex & index );
  void emitLayoutChanged() { emit layoutChanged(); }
};

#endif //__MidasTreeModelServer_H

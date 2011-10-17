#ifndef __Midas3TreeModelServer_H
#define __Midas3TreeModelServer_H

#include "Midas3TreeModel.h"
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <string>
#include <vector>

namespace mws {
  class WebAPI;
}

namespace m3ws {
  class Folder;
}

namespace m3do {
  class Folder;
}

class Midas3TreeItem;
class Midas3FolderTreeItem;
class Midas3ItemTreeItem;
class midasSynchronizer;

class Midas3TreeModelServer : public Midas3TreeModel
{
  Q_OBJECT

public:

  Midas3TreeModelServer(QObject* parent = 0);
  ~Midas3TreeModelServer();

  void Populate();

  void fetchMore(const QModelIndex& parent);
  void fetchFolder(Midas3FolderTreeItem* folder);
  void fetchItem(Midas3ItemTreeItem* parent);

signals:
  void serverPolled();
  void fetchedMore();

public slots:

  void itemExpanded(const QModelIndex& index);
  void emitLayoutChanged() { emit layoutChanged(); }
};

#endif //__MidasTreeModelServer_H

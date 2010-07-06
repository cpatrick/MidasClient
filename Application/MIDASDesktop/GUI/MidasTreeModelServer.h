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

class MidasTreeModelServer : public MidasTreeModel
{
  Q_OBJECT

public:

  MidasTreeModelServer(QObject *parent = 0);
  ~MidasTreeModelServer();

  void SetWebAPI(mws::WebAPI* api);
  virtual void Populate();

  QVariant data(const QModelIndex &index, int role) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  QVariant headerData(int section, Qt::Orientation orientation,
                     int role = Qt::DisplayRole) const;

  QModelIndex parent(const QModelIndex &index) const;
  bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const; 
  int columnCount(const QModelIndex &parent = QModelIndex()) const;

  void fetchMore ( const QModelIndex & parent ); 
  bool canFetchMore ( const QModelIndex & parent ) const;
  void fetchCollection(MidasCollectionTreeItem* parent);
  void fetchItem(MidasItemTreeItem* parent);

  const inline MidasTreeItem *midasTreeItem(const QModelIndex &index) const
    {
    return index.isValid() ? reinterpret_cast<const MidasTreeItem*>(index.internalPointer()): NULL;
    }

signals:
  void serverPolled();
  void fetchedMore();

public slots:

  void itemExpanded ( const QModelIndex & index );
  void itemCollapsed ( const QModelIndex & index );
  void decorateByUuid( std::string uuid );
  void emitLayoutChanged() { emit layoutChanged(); }

private:
  void decorateRecurse( MidasTreeItem* node, std::string uuid);

  mws::WebAPI*                     m_WebAPI;
};

#endif //__MidasTreeModelServer_H

#ifndef __MidasTreeModel_H
#define __MidasTreeModel_H

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

class MidasTreeModel : public QAbstractItemModel
{
  Q_OBJECT

public:

  MidasTreeModel(QObject *parent = 0);
  ~MidasTreeModel();

  void SetWebAPI(mws::WebAPI* api);
  virtual void Populate();
  
  void clear(const QModelIndex &index); 

  QVariant data(const QModelIndex &index, int role) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  QVariant headerData(int section, Qt::Orientation orientation,
                     int role = Qt::DisplayRole) const;
  QModelIndex index(int row, int column,
                   const QModelIndex &parent = QModelIndex()) const;

  QModelIndex getIndexByUuid(std::string uuid);

  QModelIndex parent(const QModelIndex &index) const;
  bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const; 
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
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

private:
  void decorateRecurse( MidasTreeItem* node, std::string uuid);
  QModelIndex getIndexRecurse(MidasTreeItem* node, std::string uuid,
                              QModelIndex parent);

  mws::WebAPI*                     m_WebAPI;
  QList<MidasCommunityTreeItem*>   m_TopLevelCommunities;
};

#endif //__MidasTreeModel_H

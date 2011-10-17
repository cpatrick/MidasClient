#ifndef __MidasTreeModelClient_H
#define __MidasTreeModelClient_H

#include "midasStandardIncludes.h"
#include "MidasTreeModel.h"
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <string>
#include <vector>

class MidasCommunityTreeItem;
class MidasCollectionTreeItem;
class MidasItemTreeItem;

class MidasTreeModelClient : public MidasTreeModel
{
  Q_OBJECT
public:

  MidasTreeModelClient(QObject *parent = 0);
  ~MidasTreeModelClient();

  void Populate();

  void addResource(mdo::Object *);

  void updateResource(mdo::Object *);

  void deleteResource(mdo::Object *);

  void fetchMore(const QModelIndex & parent);

  void fetchCommunity(MidasCommunityTreeItem* parent);

  void fetchCollection(MidasCollectionTreeItem* parent);

  void fetchItem(MidasItemTreeItem* parent);

};

#endif // __MidasTreeModelClient_H

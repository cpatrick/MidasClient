#ifndef __MidasTreeModelClient_H
#define __MidasTreeModelClient_H

#include "midasStandardIncludes.h"
#include "MidasTreeModel.h"
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <string>
#include <vector>

class MidasTreeItem;
class MidasCommunityTreeItem;

class MidasTreeModelClient : public MidasTreeModel
{
  Q_OBJECT

public:

  MidasTreeModelClient(QObject *parent = 0);
  ~MidasTreeModelClient();

  void Populate();

  bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;

  void addResource(mdo::Object*);
  void updateResource(mdo::Object*);
  void deleteResource(mdo::Object*);

};

#endif //__MidasTreeModelClient_H

#ifndef __MidasTreeModelClient_H
#define __MidasTreeModelClient_H

#include "midasStandardIncludes.h"
#include "MidasTreeModel.h"
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <string>
#include <vector>

class midasDatabaseProxy;

class MidasTreeItem;
class MidasCommunityTreeItem;

class MidasTreeModelClient : public MidasTreeModel
{
  Q_OBJECT

public:

  MidasTreeModelClient(QObject *parent = 0);
  ~MidasTreeModelClient();

  void SetDatabase(midasDatabaseProxy* database) { this->m_Database = database; }

  void Populate();

  bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;

private:
  midasDatabaseProxy*  m_Database;
};

#endif //__MidasTreeModelClient_H

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
class midasLog;

class MidasTreeItem;
class MidasCommunityTreeItem;

class MidasTreeModelClient : public MidasTreeModel
{
  Q_OBJECT

public:

  MidasTreeModelClient(QObject *parent = 0);
  ~MidasTreeModelClient();

  void SetDatabase(midasDatabaseProxy* database) { this->m_Database = database; }
  void SetLog(midasLog* log);
  void Populate();

  bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;

public slots:

  void itemExpanded ( const QModelIndex & index );
  void itemCollapsed ( const QModelIndex & index );

private:
  midasLog*            m_Log;
  midasDatabaseProxy*  m_Database;
};

#endif //__MidasTreeModelClient_H

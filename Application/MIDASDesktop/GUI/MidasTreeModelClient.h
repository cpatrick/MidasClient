#ifndef __MidasTreeModelClient_H
#define __MidasTreeModelClient_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <string>
#include <vector>

class midasDatabaseProxy;
class midasLog;

class MidasTreeItem;
class MidasCommunityTreeItem;

class MidasTreeModelClient : public QAbstractItemModel
{
  Q_OBJECT

public:

  MidasTreeModelClient(QObject *parent = 0);
  ~MidasTreeModelClient();

  void SetDatabase(midasDatabaseProxy* database) { this->m_Database = database; }
  void SetLog(midasLog* log);
  void Populate();
  
  void clear(const QModelIndex &index); 

  QVariant data(const QModelIndex &index, int role) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  QVariant headerData(int section, Qt::Orientation orientation,
                     int role = Qt::DisplayRole) const;
  QModelIndex index(int row, int column,
                   const QModelIndex &parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex &index) const;
  bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const; 
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;

  void fetchMore ( const QModelIndex & parent ); 
  bool canFetchMore ( const QModelIndex & parent ) const;

  void insertMidasCommunity(const QModelIndex & parent, 
                              const std::string& name, 
                              const std::string& short_description, 
                              const std::string& links_text); 

  const inline MidasTreeItem *midasTreeItem(const QModelIndex &index) const 
    {
    return index.isValid() ? reinterpret_cast<const MidasTreeItem*>(index.internalPointer()): NULL;
    }

public slots:

  void itemExpanded ( const QModelIndex & index ); 
  void itemCollapsed ( const QModelIndex & index ); 

private:
  midasLog*                        m_Log;
  midasDatabaseProxy*              m_Database;
  QList<MidasCommunityTreeItem*>   m_TopLevelCommunities;
};

#endif //__MidasTreeModelClient_H

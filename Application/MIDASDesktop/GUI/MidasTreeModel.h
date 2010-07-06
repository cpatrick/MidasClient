#ifndef __MidasTreeModel_H
#define __MidasTreeModel_H

#include "midasStandardIncludes.h"
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QList>

class MidasCommunityTreeItem;

class MidasTreeModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  MidasTreeModel(QObject *parent);
  ~MidasTreeModel();

  void registerResource(std::string uuid, QModelIndex index);
  QModelIndex getIndexByUuid(std::string uuid);
  void clearIndexMap();

  virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

protected:
  std::map<std::string, QModelIndex> m_IndexMap;
  QList<MidasCommunityTreeItem*>     m_TopLevelCommunities;
  bool AlterList;
};

#endif
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

  virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
  virtual int rowCount(const QModelIndex &parent) const;

  virtual void clear();
  virtual void restoreExpandedState();

signals:
  void expand(const QModelIndex&);

protected:
  std::map<std::string, QModelIndex> m_IndexMap;
  std::set<std::string>              m_ExpandedList;
  QList<MidasCommunityTreeItem*>     m_TopLevelCommunities;
  bool AlterList;
};

#endif
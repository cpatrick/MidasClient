#ifndef __Midas3TreeModel_H
#define __Midas3TreeModel_H

#include "midasStandardIncludes.h"
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QList>
#include <QVariant>

class Midas3FolderTreeItem;
class Midas3TreeItem;
class midasSynchronizer;

class Midas3TreeModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  Midas3TreeModel(QObject *parent);
  ~Midas3TreeModel();

  void registerResource(std::string uuid, QModelIndex index);
  void unregisterResource(std::string uuid);
  QModelIndex getIndexByUuid(std::string uuid);
  void clearExpandedList();
  void expandAllResources();
  
  virtual void SetSynchronizer(midasSynchronizer* synch);
  virtual void Populate() = 0;

  virtual QVariant headerData(int section, Qt::Orientation orientation,
                              int role = Qt::DisplayRole) const;
  virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
  virtual QModelIndex parent(const QModelIndex& index) const;
  virtual Qt::ItemFlags flags(const QModelIndex& index) const;
  virtual QVariant data(const QModelIndex& index, int role) const;
  virtual int rowCount(const QModelIndex& parent) const;
  virtual int columnCount(const QModelIndex& parent) const;
  virtual bool canFetchMore(const QModelIndex& parent) const;
  virtual bool hasChildren(const QModelIndex& parent = QModelIndex()) const;

  virtual void fetchMore(const QModelIndex& parent);

  virtual void clear();
  virtual void restoreExpandedState();

  const inline Midas3TreeItem* midasTreeItem(const QModelIndex& index) const
    {
    return index.isValid() ? reinterpret_cast<const Midas3TreeItem*>(index.internalPointer()): NULL;
    }

signals:
  void expand(const QModelIndex&);

public slots:
  virtual void itemExpanded(const QModelIndex& index);
  virtual void itemCollapsed(const QModelIndex& index);
  virtual void decorateByUuid(std::string uuid);

  virtual void addResource(mdo::Object*) {}
  virtual void updateResource(mdo::Object*) {}
  virtual void deleteResource(mdo::Object*) {}

protected:
  std::map<std::string, QModelIndex> m_IndexMap;
  std::set<std::string>              m_ExpandedList;
  QList<Midas3FolderTreeItem*>       m_TopLevelFolders;
  midasSynchronizer*                 m_Synch;
  bool AlterList;
};

#endif

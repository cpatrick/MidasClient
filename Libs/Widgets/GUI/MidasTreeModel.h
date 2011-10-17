/******************************************************************************
 * Copyright 2011 Kitware Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
#ifndef __MidasTreeModel_H
#define __MidasTreeModel_H

#include "midasStandardIncludes.h"
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QList>
#include <QVariant>

class MidasCommunityTreeItem;
class MidasTreeItem;
class midasSynchronizer;

class MidasTreeModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  MidasTreeModel(QObject* parent);
  ~MidasTreeModel();

  void RegisterResource(const std::string& uuid, QModelIndex index);

  void UnregisterResource(const std::string& uuid);

  QModelIndex GetIndexByUuid(const std::string& uuid);

  void ClearExpandedList();

  void ExpandAllResources();

  virtual void SetSynchronizer(midasSynchronizer* synch);

  virtual void Populate() = 0;

  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex() ) const;

  virtual QModelIndex parent(const QModelIndex & index) const;

  virtual Qt::ItemFlags flags(const QModelIndex & index) const;

  virtual QVariant data(const QModelIndex & index, int role) const;

  virtual int rowCount(const QModelIndex & parent) const;

  virtual int columnCount(const QModelIndex & parent) const;

  virtual bool canFetchMore( const QModelIndex & parent ) const;

  virtual bool hasChildren( const QModelIndex & parent = QModelIndex() ) const;

  virtual void fetchMore( const QModelIndex & parent );

  virtual void Clear();

  virtual void RestoreExpandedState();

  const inline MidasTreeItem* GetMidasTreeItem(const QModelIndex & index) const
  {
    return index.isValid() ? reinterpret_cast<const MidasTreeItem *>(index.internalPointer() ) : NULL;
  }

signals:
  void Expand(const QModelIndex &);

public slots:
  virtual void ItemExpanded( const QModelIndex & index );

  virtual void ItemCollapsed( const QModelIndex & index );

  virtual void DecorateByUuid( const std::string & uuid );

  virtual void AddResource(mdo::Object *) {}

  virtual void UpdateResource(mdo::Object *) {}

  virtual void DeleteResource(mdo::Object *) {}

protected:
  std::map<std::string, QModelIndex> m_IndexMap;
  std::set<std::string>              m_ExpandedList;
  QList<MidasCommunityTreeItem *>    m_TopLevelCommunities;
  midasSynchronizer*                 m_Synch;
  bool                               m_AlterList;
};

#endif

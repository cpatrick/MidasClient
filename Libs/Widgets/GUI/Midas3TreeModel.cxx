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
#include "Midas3TreeModel.h"
#include "Midas3TreeItem.h"
#include "Midas3FolderTreeItem.h"
#include "midasSynchronizer.h"
#include "m3doFolder.h"
#include <QPixmap>

Midas3TreeModel::Midas3TreeModel(QObject* parent)
  : QAbstractItemModel(parent), m_Synch(NULL)
{
  m_AlterList = true;
}

Midas3TreeModel::~Midas3TreeModel()
{
}

void Midas3TreeModel::SetSynchronizer(midasSynchronizer* synch)
{
  m_Synch = synch;
}

// -------------------------------------------------------------------------
void Midas3TreeModel::Clear()
{
  this->beginResetModel();
  for( QList<Midas3FolderTreeItem *>::iterator i = m_TopLevelFolders.begin();
       i != m_TopLevelFolders.end(); ++i )
    {
    delete (*i)->GetFolder();
    delete *i;
    }
  m_TopLevelFolders.clear();
  m_IndexMap.clear();
  this->reset();
  this->endResetModel();
}

// -------------------------------------------------------------------------
void Midas3TreeModel::RegisterResource(const std::string& uuid, QModelIndex index)
{
  m_IndexMap[uuid] = index;
}

void Midas3TreeModel::UnregisterResource(const std::string& uuid)
{
  m_IndexMap.erase(uuid);
}

// -------------------------------------------------------------------------
QModelIndex Midas3TreeModel::GetIndexByUuid(const std::string& uuid)
{
  if( m_IndexMap.find(uuid) != m_IndexMap.end() )
    {
    return m_IndexMap[uuid];
    }
  else
    {
    return QModelIndex();
    }
}

// -------------------------------------------------------------------------
QModelIndex Midas3TreeModel::index(int row,
                                   int column, const
                                   QModelIndex & parent) const
{
  if( !hasIndex(row, column, parent) )
    {
    return QModelIndex();
    }

  if( !parent.isValid() )
    {
    return createIndex(row, column, m_TopLevelFolders[row]);
    }

  Midas3TreeItem* parentItem = static_cast<Midas3TreeItem *>(parent.internalPointer() );
  Midas3TreeItem* childItem = parentItem->GetChild(row);
  if( childItem )
    {
    return this->createIndex(row, column, childItem);
    }
  else
    {
    return QModelIndex();
    }
}

// -------------------------------------------------------------------------
void Midas3TreeModel::RestoreExpandedState()
{ /* don't do this for now
  //copy list so we can remove while iterating
  std::set<std::string> copy = m_ExpandedList;
  for(std::set<std::string>::iterator i = copy.begin();
      i != copy.end(); ++i)
    {
    QModelIndex index = this->getIndexByUuid(*i);
    if(index.isValid())
      {
      this->AlterList = false;
      emit expand(index);
      this->AlterList = true;
      }
    else
      {
      //If we can't resolve the list item, we should remove it from the list.
      //(Probably no longer exists due to deletion)
      m_ExpandedList.erase(*i);
      }
    }*/
}

// -------------------------------------------------------------------------
int Midas3TreeModel::rowCount(const QModelIndex& parent) const
{
  Midas3TreeItem* parentItem;

  if( parent.column() > 0 )
    {
    return 0;
    }

  if( !parent.isValid() )
    {
    return m_TopLevelFolders.size();
    }
  parentItem = static_cast<Midas3TreeItem *>(parent.internalPointer() );
  return parentItem->ChildCount();
}

// -------------------------------------------------------------------------
int Midas3TreeModel::columnCount(const QModelIndex& parent) const
{
  if( parent.isValid() )
    {
    return static_cast<Midas3TreeItem *>(parent.internalPointer() )->ColumnCount();
    }
  return 1;
}

// -------------------------------------------------------------------------
QVariant Midas3TreeModel::data(const QModelIndex& index, int role) const
{
  if( !index.isValid() )
    {
    return QVariant();
    }
  Midas3TreeItem* item = static_cast<Midas3TreeItem *>(index.internalPointer() );
  if( role == Qt::DisplayRole )
    {
    return item->GetData(index.column() );
    }
  else if( role == Qt::DecorationRole )
    {
    return item->GetDecoration();
    }
  else
    {
    return QVariant();
    }
}

// -------------------------------------------------------------------------
Qt::ItemFlags Midas3TreeModel::flags(const QModelIndex& index) const
{
  if( !index.isValid() )
    {
    return 0;
    }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

// -------------------------------------------------------------------------
QModelIndex Midas3TreeModel::parent(const QModelIndex& index) const
{
  if( !index.isValid() )
    {
    return QModelIndex();
    }

  Midas3TreeItem* childItem = static_cast<Midas3TreeItem *>(index.internalPointer() );
  Midas3TreeItem* parentItem = childItem->GetParent();

  if( parentItem == NULL )
    {
    return QModelIndex();
    }
  return createIndex(parentItem->GetRow(), 0, parentItem);
}

bool Midas3TreeModel::hasChildren(const QModelIndex& parent) const
{
  if( !parent.isValid() )
    {
    return true;
    }
  const Midas3TreeItem* item = this->GetMidasTreeItem(parent);
  if( item->IsFetchedChildren() )
    {
    return item->ChildCount() > 0;
    }
  else
    {
    return true;
    }
}

// -------------------------------------------------------------------------
QVariant Midas3TreeModel::headerData(int section,
                                     Qt::Orientation orientation,
                                     int role) const
{
  (void)section;
  (void)role;
  (void)orientation;
  /*if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
    return m_TopLevelFolders.data(section);
    }
    */
  return QVariant();
}

// -------------------------------------------------------------------------
bool Midas3TreeModel::canFetchMore(const QModelIndex& parent) const
{
  if( !parent.isValid() )
    {
    return false;
    }
  const Midas3TreeItem* item = this->GetMidasTreeItem(parent);
  return !item->IsFetchedChildren();
}

// -------------------------------------------------------------------------
void Midas3TreeModel::fetchMore(const QModelIndex& parent)
{
  (void)parent;
}

// -------------------------------------------------------------------------
void Midas3TreeModel::ClearExpandedList()
{
  m_ExpandedList.clear();
}

// -------------------------------------------------------------------------
void Midas3TreeModel::ExpandAllResources()
{
  m_ExpandedList.clear();
  for( std::map<std::string, QModelIndex>::iterator i = m_IndexMap.begin();
       i != m_IndexMap.end(); ++i )
    {
    m_ExpandedList.insert(i->first);
    }
}

// -------------------------------------------------------------------------
void Midas3TreeModel::ItemExpanded(const QModelIndex& index)
{
  Midas3TreeItem* item = const_cast<Midas3TreeItem *>(this->GetMidasTreeItem(index) );

  item->SetDecorationRole(Midas3TreeItem::Expanded);

  if( m_AlterList )
    {
    m_ExpandedList.insert(item->GetUuid() );
    }
}

// -------------------------------------------------------------------------
void Midas3TreeModel::ItemCollapsed(const QModelIndex& index)
{
  Midas3TreeItem* item = const_cast<Midas3TreeItem *>(this->GetMidasTreeItem(index) );

  item->SetDecorationRole(Midas3TreeItem::Collapsed);

  m_ExpandedList.erase(item->GetUuid() );
}

// -------------------------------------------------------------------------
void Midas3TreeModel::DecorateByUuid(std::string uuid)
{
  QModelIndex index = this->GetIndexByUuid(uuid);

  if( index.isValid() )
    {
    Midas3TreeItem* node = const_cast<Midas3TreeItem *>(
        this->GetMidasTreeItem(index) );
    node->SetDecorationRole(Midas3TreeItem::Dirty);

    Midas3TreeItem* item =
      const_cast<Midas3TreeItem *>(this->GetMidasTreeItem(index) );
    item->UpdateDisplayName();

    emit dataChanged(index, index);
    }
}


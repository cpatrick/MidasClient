#include "MidasTreeModelClient.h"

#include <QPixmap>
#include <QApplication>


#include "MidasTreeItem.h"
#include "MidasCommunityTreeItem.h"
#include "MidasCollectionTreeItem.h"
#include "MidasItemTreeItem.h"

#include "Logger.h"
#include "Utils.h"
#include <iostream>
#include "midasDatabaseProxy.h"
#include "midasLog.h"
#include "mdoCommunity.h"

MidasTreeModelClient::MidasTreeModelClient(QObject *parent) : MidasTreeModel(parent)
{
  this->AlterList = true;
}

MidasTreeModelClient::~MidasTreeModelClient()
{
}

void MidasTreeModelClient::SetLog(midasLog* log)
{
  this->m_Log = log;
}

/** Populate the tree */
void MidasTreeModelClient::Populate()
{
  this->m_Database->Open();
  std::vector<mdo::Community*> topLevelCommunities =
    this->m_Database->GetTopLevelCommunities(true);
  
  this->beginInsertRows(QModelIndex(), 0, topLevelCommunities.size());
  int row = 0;
  for(std::vector<mdo::Community*>::iterator i = topLevelCommunities.begin();
      i != topLevelCommunities.end(); ++i)
    {
    QList<QVariant> columnData;
    columnData << (*i)->GetName().c_str();

    MidasCommunityTreeItem* communityItem = new MidasCommunityTreeItem(columnData, this, NULL);
    communityItem->setCommunity(*i);
    m_TopLevelCommunities.append(communityItem);

    QModelIndex index = this->index(row, 0);
    registerResource((*i)->GetUuid(), index);

    communityItem->populate(index);
    communityItem->setTopLevelCommunities(&m_TopLevelCommunities);
    if((*i)->IsDirty())
      {
      communityItem->setDecorationRole(MidasTreeItem::Dirty);
      }
    row++;
    }
  this->m_Database->Close();
  this->endInsertRows();
  emit layoutChanged();
}

bool MidasTreeModelClient::hasChildren( const QModelIndex & parent ) const
{
  if (!parent.isValid())
    {
    return true;
    }
    
  const MidasTreeItem * item = (MidasTreeItem*)midasTreeItem(parent);
  return item->childCount();
}

int MidasTreeModelClient::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    {
    return static_cast<MidasTreeItem*>(parent.internalPointer())->columnCount();
    }
  return 1;
}

QVariant MidasTreeModelClient::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    {
    return QVariant();
    }
  MidasTreeItem *item = static_cast<MidasTreeItem*>(index.internalPointer());
  if ( role == Qt::DisplayRole )
    {
    return item->data(index.column());
    }
  else if ( role == Qt::DecorationRole )
    {
    return item->getDecoration();
    }
  else 
    {
    return QVariant();
    }
}

Qt::ItemFlags MidasTreeModelClient::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    {
    return 0;
    }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant MidasTreeModelClient::headerData(int section, 
                                    Qt::Orientation orientation,
                                    int role) const
{
  /*if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
    return m_TopLevelCommunities.data(section);
    }
    */
  return QVariant();
}

QModelIndex MidasTreeModelClient::parent(const QModelIndex &index) const
  {
  if (!index.isValid())
    {
    return QModelIndex();
    }

  MidasTreeItem *childItem = static_cast<MidasTreeItem*>(index.internalPointer());
  MidasTreeItem *parentItem = childItem->parent();

  if (parentItem == NULL)
    {
    return QModelIndex();
    }
  return createIndex(parentItem->row(), 0, parentItem);
}

bool MidasTreeModelClient::canFetchMore ( const QModelIndex & parent ) const
  {
  if ( !parent.isValid() )
    {
    return false;
    }
  const MidasTreeItem * item = (this->midasTreeItem(parent)); 
  return !item->isFetchedChildren();
  }

/** Fetch more data */
void MidasTreeModelClient::fetchMore ( const QModelIndex & parent )
{
  if (!parent.isValid())
    {
    return;
    }
  MidasTreeItem * item = const_cast<MidasTreeItem *>(midasTreeItem(parent)); 

  int row_count = this->rowCount(parent); 
  if (row_count > 0)
    {
    /*this->beginInsertRows(parent, 0, row_count - 1);
    item->populate(); 
    this->endInsertRows();
    */
    }
}

void MidasTreeModelClient::itemExpanded ( const QModelIndex & index )
{
  MidasTreeItem * item = const_cast<MidasTreeItem *>(this->midasTreeItem(index));
  item->setDecorationRole(MidasTreeItem::Expanded);
  
  if(this->AlterList)
    {
    m_ExpandedList.insert(item->getUuid());
    }
}

void MidasTreeModelClient::itemCollapsed ( const QModelIndex & index )
{
  MidasTreeItem * item = const_cast<MidasTreeItem *>(this->midasTreeItem(index));
  item->setDecorationRole(MidasTreeItem::Collapsed);
  
  m_ExpandedList.erase(item->getUuid());
}

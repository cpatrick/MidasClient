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

MidasTreeModelClient::MidasTreeModelClient(QObject *parent) : QAbstractItemModel(parent)
{
}

MidasTreeModelClient::~MidasTreeModelClient()
{
}

void MidasTreeModelClient::SetLog(midasLog* log)
{
  this->m_Log = log;
}

void MidasTreeModelClient::clear(const QModelIndex &index)
{
  for(QList<MidasCommunityTreeItem*>::iterator i = m_TopLevelCommunities.begin();
      i != m_TopLevelCommunities.end(); ++i)
    {
    delete (*i)->getCommunity();
    }
  this->m_TopLevelCommunities.clear();
}

/** Populate the tree */
void MidasTreeModelClient::Populate()
{
  this->m_Database->Open();
  std::vector<mdo::Community*> topLevelCommunities =
    this->m_Database->GetTopLevelCommunities(true);
  
  this->beginInsertRows(QModelIndex(), 0, topLevelCommunities.size());
  for(std::vector<mdo::Community*>::iterator i = topLevelCommunities.begin();
      i != topLevelCommunities.end(); ++i)
    {
    QList<QVariant> columnData;
    columnData << (*i)->GetName().c_str();

    MidasCommunityTreeItem* communityItem = new MidasCommunityTreeItem(columnData, NULL);
    communityItem->setCommunity(*i);
    communityItem->populate();
    communityItem->setTopLevelCommunities(&m_TopLevelCommunities);
    if((*i)->IsDirty())
      {
      communityItem->setDecorationRole(MidasTreeItem::Dirty);
      }
    m_TopLevelCommunities.append(communityItem);
    }
  this->m_Database->Close();
  this->endInsertRows();
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

QModelIndex MidasTreeModelClient::index(int row, 
                                  int column, const 
                                  QModelIndex &parent) const
{
  if (!hasIndex(row, column, parent))
    {
    return QModelIndex();
    }

  if (!parent.isValid())
    {
    return createIndex(row, column, m_TopLevelCommunities[row]);
    }
    
  MidasTreeItem* parentItem = static_cast<MidasTreeItem*>(parent.internalPointer());
  MidasTreeItem* childItem = parentItem->child(row);
  if (childItem)
    {
    return createIndex(row, column, childItem);
    }
  else
    {
    return QModelIndex();
    }
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

int MidasTreeModelClient::rowCount(const QModelIndex &parent) const
{
  MidasTreeItem *parentItem;
  if (parent.column() > 0)
    {
    return 0;
    }
    
  if (!parent.isValid())
    {
    return m_TopLevelCommunities.size();
    }
  parentItem = static_cast<MidasTreeItem*>(parent.internalPointer());
  return parentItem->childCount();
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
}

void MidasTreeModelClient::itemCollapsed ( const QModelIndex & index )
{
  MidasTreeItem * item = const_cast<MidasTreeItem *>(this->midasTreeItem(index));
  item->setDecorationRole(MidasTreeItem::Collapsed);
}

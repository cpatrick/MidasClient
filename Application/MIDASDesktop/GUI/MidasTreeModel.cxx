#include "MidasTreeModel.h"

#include <QPixmap>
#include <QApplication>

#include "MidasTreeItem.h"
#include "MidasCommunityTreeItem.h"
#include "MidasCollectionTreeItem.h"
#include "MidasItemTreeItem.h"
#include "MidasBitstreamTreeItem.h"

#include "Logger.h"
#include "Utils.h"

#include "mdoCollection.h"
#include "mwsCollection.h"
#include "mdoItem.h"
#include "mwsItem.h"
#include "mdoBitstream.h"

#include <iostream>
#include <mdoCommunity.h>
#include <mwsCommunity.h>

MidasTreeModel::MidasTreeModel(QObject *parent) : QAbstractItemModel(parent)
{
}

void MidasTreeModel::clear(const QModelIndex &index)
{
  for(QList<MidasCommunityTreeItem*>::iterator i = m_TopLevelCommunities.begin();
      i != m_TopLevelCommunities.end(); ++i)
    {
    delete (*i)->getCommunity();
    }
  this->m_TopLevelCommunities.clear();
}

MidasTreeModel::~MidasTreeModel()
{
}

/** Set the web API */
void MidasTreeModel::SetWebAPI(mws::WebAPI* api)
{
  m_WebAPI = api;
}

/** Populate the tree */
void MidasTreeModel::Populate()
{
  mws::Community remote;
  mdo::Community* community = new mdo::Community;
  community->SetId(0);
  remote.SetWebAPI(this->m_WebAPI);
  remote.SetObject(community);

  if(!remote.FetchTree())
    {
    return;
    }

  std::vector<mdo::Community*> communities = community->GetCommunities();

  this->beginInsertRows(QModelIndex(), 0, communities.size());

  std::vector<mdo::Community*>::const_iterator itCom = communities.begin();
  while(itCom != communities.end())
    {
    // Set the name of the community
    QList<QVariant> columnData;
    columnData << (*itCom)->GetName().c_str();
    
    // Add the community
    MidasCommunityTreeItem* communityItem = new MidasCommunityTreeItem(columnData, NULL);
    communityItem->setCommunity(*itCom);
    communityItem->setDynamicFetch(true);
    communityItem->populate();
    communityItem->setTopLevelCommunities(&m_TopLevelCommunities);
    
    m_TopLevelCommunities.append(communityItem);
    itCom++;
    }
  
  this->endInsertRows();
  emit layoutChanged();
  emit serverPolled();
}

bool MidasTreeModel::hasChildren( const QModelIndex & parent ) const
{
  if (!parent.isValid())
    {
    return true;
    }
  const MidasTreeItem * item = (MidasTreeItem*)midasTreeItem(parent);
  if(item->isFetchedChildren())
    {
    return item->childCount();
    }
  else
    {
    return true;
    }
}

int MidasTreeModel::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    {
    return static_cast<MidasTreeItem*>(parent.internalPointer())->columnCount();
    }
  return 1;
}

QVariant MidasTreeModel::data(const QModelIndex &index, int role) const
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

Qt::ItemFlags MidasTreeModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    {
    return 0;
    }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant MidasTreeModel::headerData(int section, 
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

QModelIndex MidasTreeModel::index(int row, 
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

QModelIndex MidasTreeModel::parent(const QModelIndex &index) const
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

int MidasTreeModel::rowCount(const QModelIndex &parent) const
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

bool MidasTreeModel::canFetchMore ( const QModelIndex & parent ) const
  {
  if ( !parent.isValid() )
    {
    return false;
    }
  const MidasTreeItem * item = (this->midasTreeItem(parent)); 
  return !item->isFetchedChildren(); 
  }

/** Fetch more data */
void MidasTreeModel::fetchMore ( const QModelIndex & parent )
{
  if (!parent.isValid() || !canFetchMore(parent))
    {
    return;
    }
  MidasTreeItem * item = const_cast<MidasTreeItem *>(midasTreeItem(parent)); 
  MidasCollectionTreeItem * collectionTreeItem = NULL;
  MidasItemTreeItem * itemTreeItem = NULL;

  if ((collectionTreeItem = dynamic_cast<MidasCollectionTreeItem*>( const_cast<MidasTreeItem*>( item ) ) ) != NULL )
    {
    this->fetchCollection(collectionTreeItem);
    }
  else if ((itemTreeItem = dynamic_cast<MidasItemTreeItem*>( const_cast<MidasTreeItem*>( item ) ) ) != NULL )
    {
    this->fetchItem(itemTreeItem);
    }
  item->setFetchedChildren(true);
  emit layoutChanged();
  emit fetchedMore();
}

//-------------------------------------------------------------------------
void MidasTreeModel::fetchCollection(MidasCollectionTreeItem* parent)
{
  mws::Collection remote;
  mdo::Collection* collection = parent->getCollection();
  remote.SetWebAPI(mws::WebAPI::Instance());
  remote.SetObject(collection);

  if(!remote.Fetch())
    {
    //emit fetchError();
    return;
    }

  for(std::vector<mdo::Item*>::const_iterator i = collection->GetItems().begin();
      i != collection->GetItems().end(); ++i)
    {
    QList<QVariant> name;
    name << (*i)->GetTitle().c_str();
    MidasItemTreeItem* item = new MidasItemTreeItem(name, parent);
    item->setItem(*i);
    item->setDynamicFetch(true);
    item->setFetchedChildren(false);
    parent->appendChild(item);
    }
  emit layoutChanged();
}

//-------------------------------------------------------------------------
void MidasTreeModel::fetchItem(MidasItemTreeItem* parent)
{
  mws::Item remote;
  mdo::Item* item = parent->getItem();
  remote.SetWebAPI(mws::WebAPI::Instance());
  remote.SetObject(item);

  if(!remote.Fetch())
    {
    //emit fetchError();
    return;
    }

  for(std::vector<mdo::Bitstream*>::const_iterator i = item->GetBitstreams().begin();
      i != item->GetBitstreams().end(); ++i)
    {
    QList<QVariant> name;
    name << (*i)->GetName().c_str();
    MidasBitstreamTreeItem* bitstream = new MidasBitstreamTreeItem(name, parent);
    bitstream->setBitstream(*i);
    parent->appendChild(bitstream);
    }
  emit layoutChanged();
}

//-------------------------------------------------------------------------
void MidasTreeModel::itemExpanded ( const QModelIndex & index )
{
  MidasTreeItem * item = const_cast<MidasTreeItem *>(this->midasTreeItem(index));
  item->setDecorationRole(MidasTreeItem::Expanded);
}

//-------------------------------------------------------------------------
void MidasTreeModel::itemCollapsed ( const QModelIndex & index )
{
  MidasTreeItem * item = const_cast<MidasTreeItem *>(this->midasTreeItem(index));
  item->setDecorationRole(MidasTreeItem::Collapsed);
}

//-------------------------------------------------------------------------
void MidasTreeModel::decorateByUuid(std::string uuid)
{
  for(QList<MidasCommunityTreeItem*>::iterator i = m_TopLevelCommunities.begin();
      i != this->m_TopLevelCommunities.end(); ++i)
    {
    decorateRecurse(reinterpret_cast<MidasTreeItem*>(*i), uuid);
    }
}

//-------------------------------------------------------------------------
void MidasTreeModel::decorateRecurse(MidasTreeItem* node, std::string uuid)
{
  if(node->getUuid() == uuid)
    {
    node->setDecorationRole(MidasTreeItem::Dirty);
    return;
    }
  for(int i = 0; i < node->childCount(); i++)
    {
    decorateRecurse(node->child(i), uuid);
    }
}

QModelIndex MidasTreeModel::getIndexByUuid(std::string uuid)
{
  QModelIndex value;
  for(QList<MidasCommunityTreeItem*>::iterator i = m_TopLevelCommunities.begin();
      i != this->m_TopLevelCommunities.end(); ++i)
    {
    MidasTreeItem* node = reinterpret_cast<MidasTreeItem*>(*i);
    value = getIndexRecurse(node, uuid, QModelIndex());
    if(value.isValid()) break;
    }
  return value;
}

QModelIndex MidasTreeModel::getIndexRecurse(MidasTreeItem* node,
                                            std::string uuid,
                                            QModelIndex parent)
{
  if(node->getUuid() == uuid)
    {
    return parent.isValid() ? index(node->row(), 0, parent) :
      index(node->row(), 0);
    }
  QModelIndex value;
  for(int i = 0; i < node->childCount(); i++)
    {
    value = getIndexRecurse(node->child(i), uuid,
      index(node->row(), 0, parent));
    if(value.isValid()) break;
    }
  return value;
}

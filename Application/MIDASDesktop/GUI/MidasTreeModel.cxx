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
  if (!parent.isValid())
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
void MidasTreeModel::insertMidasCommunity(const QModelIndex & parent, 
                                          const std::string& name, 
                                          const std::string& short_description, 
                                          const std::string& links_text)
  {
  /*MidasTreeItem * parentItem = const_cast<MidasTreeItem *>(this->midasTreeItem(parent)); 
  
  STRUCT_COMMUNITY * new_resource = new STRUCT_COMMUNITY; 
  new_resource->setParentId( parentItem->getId() ); 
  new_resource->setName( name ); 
  new_resource->getInfo()->short_description = short_description; 
  new_resource->getInfo()->links_text = links_text; 

  ModelManager::instance()->createCommunity(new_resource, parentItem->getId()); 

  int child_count = parentItem->childCount();

  this->beginInsertRows(parent, child_count, child_count); 

  if (parentItem->isFetchedChildren())
    {
    QList<QVariant> columnData;
    columnData << STR2QSTR(name); 

    parentItem->appendChild(
      new MidasCommunityTreeItem( columnData, new_resource, parentItem ) ); 
    }

  if (parentItem->isFetchedChildCount())
    {
    parentItem->setChildCount(child_count + 1);
    }

  this->endInsertRows();
*/
  }

//-------------------------------------------------------------------------
void MidasTreeModel::deleteMidasCommunity(const QModelIndex & current)
  {
 /* MidasTreeItem * currentItem = const_cast<MidasTreeItem *>(this->midasTreeItem(current)); 

  MidasTreeItem * parentItem = currentItem->parent(); 

  ModelManager::instance()->deleteCommunityRecursive(currentItem->getId(), parentItem->getId()); 

  this->beginRemoveRows(current.parent(), current.row(), current.row()); 

  parentItem->removeChild(currentItem); 

  delete currentItem; 

  parentItem->setChildCount(parentItem->childCount() - 1);

  this->endRemoveRows(); 
*/
  }

//-------------------------------------------------------------------------
void MidasTreeModel::updateCommunityProperty(const QModelIndex & index, const std::string& name, const std::string& value)
  {
  MidasCommunityTreeItem * item = dynamic_cast<MidasCommunityTreeItem *>( const_cast<MidasTreeItem *>( this->midasTreeItem(index) ) ); 
/*  ModelManager::instance()->updateCommunityProperty(item->getModelData(), name, value ); 
  if ( name.compare( STRUCT_COMMUNITY_INFO::propertyNameTitle ) == 0 ) { 
    item->setData( QVariant( STR2QSTR(value) ) ); 
    }*/
  }

//-------------------------------------------------------------------------
void MidasTreeModel::insertMidasCollection(const QModelIndex & parent, 
                                          const std::string& name, 
                                          const std::string& short_description, 
                                          const std::string& links_text, 
                                          bool createDefaultGroup)
  {
  MidasTreeItem * parentItem = const_cast<MidasTreeItem *>(this->midasTreeItem(parent));

  /*
  STRUCT_COLLECTION * new_resource = new STRUCT_COLLECTION; 
  new_resource->setParentId( parentItem->getId() ); 
  new_resource->setName( name ); 
  new_resource->getInfo()->short_description = short_description; 
  new_resource->getInfo()->links_text = links_text; 

  ModelManager::instance()->createCollection(new_resource, parentItem->getId(), createDefaultGroup); 

  int child_count = parentItem->childCount();

  this->beginInsertRows(parent, child_count, child_count); 

  if (parentItem->isFetchedChildren())
    {
    QList<QVariant> columnData;
    columnData << STR2QSTR(name); 

    parentItem->appendChild(
      new MidasCollectionTreeItem( columnData, new_resource, parentItem ) ); 
    }

  if (parentItem->isFetchedChildCount())
    {
    parentItem->setChildCount(child_count + 1); 
    }

  this->endInsertRows();
*/
 }

//-------------------------------------------------------------------------
void MidasTreeModel::deleteMidasCollection(const QModelIndex & current)
  {
  MidasTreeItem * currentItem = const_cast<MidasTreeItem *>(this->midasTreeItem(current)); 

  MidasTreeItem * parentItem = currentItem->parent(); 
/*
  ModelManager::instance()->deleteCollectionRecursive(currentItem->getId(), parentItem->getId(), currentItem->getModelData()->getName()); 

  this->beginRemoveRows(current.parent(), current.row(), current.row()); 

  parentItem->removeChild(currentItem); 
*/
  delete currentItem;

//  parentItem->setChildCount(parentItem->childCount() - 1);

  this->endRemoveRows(); 
  }

//-------------------------------------------------------------------------
void MidasTreeModel::updateCollectionProperty(const QModelIndex & index, const std::string& name, const std::string& value)
  {
  MidasCollectionTreeItem * item = dynamic_cast<MidasCollectionTreeItem *>( const_cast<MidasTreeItem *>( this->midasTreeItem(index) ) ); 
/*  ModelManager::instance()->updateCollectionProperty(item->getModelData(), name, value ); 
  if ( name.compare( STRUCT_COLLECTION_INFO::propertyNameTitle ) == 0 ) { 
    item->setData( QVariant( STR2QSTR(value) ) ); 
    }*/
  }

//-------------------------------------------------------------------------
void MidasTreeModel::insertMidasItem(const QModelIndex & parent,
                                        const std::string & name,
                                        const std::vector<std::string> &authors,
                                        const std::vector<std::string> &keywords,
                                        const std::string & abstract,
                                        const std::string & description,
                                        const std::string & issuedate,
                                        const std::string & handle)
  {
  if (authors.empty())
    {
    Logger::debug("empty authors!");
    }

  if (keywords.empty())
    {
    Logger::debug("empty keywords!");
    }

  MidasTreeItem * parentItem = const_cast<MidasTreeItem *>(this->midasTreeItem(parent));

/*  
  STRUCT_ITEM * new_resource = new STRUCT_ITEM; 
  new_resource->setParentId( parentItem->getId() ); 
  new_resource->setName( name ); 
  new_resource->getInfo()->authors = authors; 
  new_resource->getInfo()->keywords = keywords; 
  new_resource->getInfo()->abstract = abstract; 
  new_resource->getInfo()->description = description; 
  new_resource->getInfo()->issuedate = issuedate; 
  new_resource->getInfo()->handle = handle; 
  ModelManager::instance()->createItem(new_resource, parentItem->getId()); 

  int child_count = parentItem->childCount();

  this->beginInsertRows(parent, child_count, child_count); 

  if (parentItem->isFetchedChildren())
    {
    QList<QVariant> columnData;
    columnData << STR2QSTR(name); 

    parentItem->appendChild(
      new MidasItemTreeItem( columnData, new_resource, parentItem ) ); 
    }

  if (parentItem->isFetchedChildCount())
    {
    parentItem->setChildCount(child_count + 1);
    }

  this->endInsertRows();
*/
  }

//-------------------------------------------------------------------------
void MidasTreeModel::deleteMidasItem(const QModelIndex & current)
  {
  MidasTreeItem * currentItem = const_cast<MidasTreeItem *>(this->midasTreeItem(current)); 

  MidasTreeItem * parentItem = currentItem->parent(); 

/*
  ModelManager::instance()->deleteItemRecursive(currentItem->getId(), parentItem->getId(), parentItem->parent()->getId()); 

  this->beginRemoveRows(current.parent(), current.row(), current.row()); 

  parentItem->removeChild(currentItem); 

  delete currentItem;

  parentItem->setChildCount(parentItem->childCount() - 1);

  this->endRemoveRows(); 
*/
  }

//-------------------------------------------------------------------------
void MidasTreeModel::updateItemProperty(const QModelIndex & index, const std::string& name, const std::string& value)
  {
  MidasItemTreeItem * item = dynamic_cast<MidasItemTreeItem *>( const_cast<MidasTreeItem *>( this->midasTreeItem(index) ) ); 
/*  ModelManager::instance()->updateItemProperty(item->getModelData(), name, value ); 
  if ( name.compare( STRUCT_ITEM_INFO::propertyNameTitle ) == 0 ) { 
    item->setData( QVariant( STR2QSTR(value) ) ); 
    }*/
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
  for(int i = 0; i < node->childCount(); i++)
    {
    return getIndexRecurse(node->child(i), uuid,
      index(node->row(), 0, parent));
    }
  return QModelIndex();
}

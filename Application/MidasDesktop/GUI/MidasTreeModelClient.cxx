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

void MidasTreeModelClient::insertMidasCommunity(const QModelIndex & parent, 
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

void MidasTreeModelClient::deleteMidasCommunity(const QModelIndex & current)
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

void MidasTreeModelClient::updateCommunityProperty(const QModelIndex & index, const std::string& name, const std::string& value)
  {
  MidasCommunityTreeItem * item = dynamic_cast<MidasCommunityTreeItem *>( const_cast<MidasTreeItem *>( this->midasTreeItem(index) ) ); 
/*  ModelManager::instance()->updateCommunityProperty(item->getModelData(), name, value ); 
  if ( name.compare( STRUCT_COMMUNITY_INFO::propertyNameTitle ) == 0 ) { 
    item->setData( QVariant( STR2QSTR(value) ) ); 
    }*/
  }

void MidasTreeModelClient::insertMidasCollection(const QModelIndex & parent, 
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

void MidasTreeModelClient::deleteMidasCollection(const QModelIndex & current)
  {
  MidasTreeItem * currentItem = const_cast<MidasTreeItem *>(this->midasTreeItem(current)); 

  MidasTreeItem * parentItem = currentItem->parent(); 
/*
  ModelManager::instance()->deleteCollectionRecursive(currentItem->getId(), parentItem->getId(), currentItem->getModelData()->getName()); 

  this->beginRemoveRows(current.parent(), current.row(), current.row()); 

  parentItem->removeChild(currentItem); 
*/
  delete currentItem;

  //parentItem->setChildCount(parentItem->childCount() - 1);

  this->endRemoveRows(); 
  }

void MidasTreeModelClient::updateCollectionProperty(const QModelIndex & index, const std::string& name, const std::string& value)
  {
  MidasCollectionTreeItem * item = dynamic_cast<MidasCollectionTreeItem *>( const_cast<MidasTreeItem *>( this->midasTreeItem(index) ) ); 
/*  ModelManager::instance()->updateCollectionProperty(item->getModelData(), name, value ); 
  if ( name.compare( STRUCT_COLLECTION_INFO::propertyNameTitle ) == 0 ) { 
    item->setData( QVariant( STR2QSTR(value) ) ); 
    }*/
  }

void MidasTreeModelClient::insertMidasItem(const QModelIndex & parent, 
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

void MidasTreeModelClient::deleteMidasItem(const QModelIndex & current)
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

void MidasTreeModelClient::updateItemProperty(const QModelIndex & index, const std::string& name, const std::string& value)
  {
  MidasItemTreeItem * item = dynamic_cast<MidasItemTreeItem *>( const_cast<MidasTreeItem *>( this->midasTreeItem(index) ) ); 
/*  ModelManager::instance()->updateItemProperty(item->getModelData(), name, value ); 
  if ( name.compare( STRUCT_ITEM_INFO::propertyNameTitle ) == 0 ) { 
    item->setData( QVariant( STR2QSTR(value) ) ); 
    }*/
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

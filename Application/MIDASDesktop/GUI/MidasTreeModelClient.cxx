#include "MidasTreeModelClient.h"

#include <QPixmap>
#include <QApplication>

#include "MidasTreeItem.h"
#include "MidasCommunityTreeItem.h"
#include "MidasCollectionTreeItem.h"
#include "MidasItemTreeItem.h"
#include "MidasBitstreamTreeItem.h"

#include "Utils.h"
#include <iostream>
#include "mdsDatabaseAPI.h"
#include "mdoCommunity.h"
#include "mdoCollection.h"
#include "mdoItem.h"
#include "mdoBitstream.h"
#include "mdoObject.h"

MidasTreeModelClient::MidasTreeModelClient(QObject *parent) : MidasTreeModel(parent)
{
  this->AlterList = true;
}

MidasTreeModelClient::~MidasTreeModelClient()
{
}

/** Populate the tree */
void MidasTreeModelClient::Populate()
{
  mds::DatabaseAPI db;
  std::vector<mdo::Community*> topLevelCommunities =
    db.GetTopLevelCommunities(true);

  this->beginInsertRows(QModelIndex(), 0, topLevelCommunities.size());
  int row = 0;
  for(std::vector<mdo::Community*>::iterator i = topLevelCommunities.begin();
      i != topLevelCommunities.end(); ++i)
    {
    QList<QVariant> columnData;
    columnData << (*i)->GetName().c_str();

    MidasCommunityTreeItem* communityItem = new MidasCommunityTreeItem(columnData, this, NULL);
    communityItem->setClientResource(true);
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

void MidasTreeModelClient::addResource(mdo::Object* object)
{
  mdo::Community* comm = NULL;
  mdo::Collection* coll = NULL;
  mdo::Item* item = NULL;
  mdo::Bitstream* bitstream = NULL;

  if((comm = dynamic_cast<mdo::Community*>(object)) != NULL)
    {
    QList<QVariant> columnData;
    columnData << comm->GetName().c_str();
    MidasCommunityTreeItem* communityItem;

    QModelIndex index;
    if(comm->GetParentCommunity() == NULL)
      {
      communityItem = new MidasCommunityTreeItem(columnData, this, NULL);
      communityItem->setTopLevelCommunities(&m_TopLevelCommunities);
      int rows = this->rowCount(QModelIndex());
      this->beginInsertRows(QModelIndex(), rows, rows);
      m_TopLevelCommunities.append(communityItem);
      this->endInsertRows();
      index = this->index(rows, 0);
      }
    else
      {
      QModelIndex parentIndex = this->getIndexByUuid(comm->GetParentCommunity()->GetUuid());
      MidasCommunityTreeItem* parent = dynamic_cast<MidasCommunityTreeItem*>(const_cast<MidasTreeItem*>(this->midasTreeItem(parentIndex)));
      communityItem = new MidasCommunityTreeItem(columnData, this, parent);
      this->beginInsertRows(parentIndex, parent->childCount(), parent->childCount());
      parent->appendChild(communityItem);
      this->endInsertRows();

      index = this->index(parent->childCount()-1, 0, parentIndex);
      }
    communityItem->setClientResource(true);
    communityItem->setCommunity(comm);
    this->registerResource(object->GetUuid(), index);
    }
  else if((coll = dynamic_cast<mdo::Collection*>(object)) != NULL)
    {
    QList<QVariant> columnData;
    columnData << coll->GetName().c_str();
    
    QModelIndex parentIndex = this->getIndexByUuid(coll->GetParentCommunity()->GetUuid());
    MidasCommunityTreeItem* parent = dynamic_cast<MidasCommunityTreeItem*>(const_cast<MidasTreeItem*>(this->midasTreeItem(parentIndex)));
    MidasCollectionTreeItem* collectionItem = new MidasCollectionTreeItem(columnData, this, parent);
    this->beginInsertRows(parentIndex, parent->childCount(), parent->childCount());
    parent->appendChild(collectionItem);
    this->endInsertRows();
    QModelIndex index = this->index(parent->childCount()-1, 0, parentIndex);

    collectionItem->setClientResource(true);
    collectionItem->setCollection(coll);
    this->registerResource(object->GetUuid(), index);
    }
  else if((item = dynamic_cast<mdo::Item*>(object)) != NULL)
    {
    QList<QVariant> columnData;
    columnData << item->GetTitle().c_str();

    QModelIndex parentIndex = this->getIndexByUuid(item->GetParentCollection()->GetUuid());
    MidasCollectionTreeItem* parent = dynamic_cast<MidasCollectionTreeItem*>(const_cast<MidasTreeItem*>(this->midasTreeItem(parentIndex)));
    MidasItemTreeItem* itemTreeItem = new MidasItemTreeItem(columnData, this, parent);
    this->beginInsertRows(parentIndex, parent->childCount(), parent->childCount());
    parent->appendChild(itemTreeItem);
    this->endInsertRows();
    QModelIndex index = this->index(parent->childCount()-1, 0, parentIndex);

    itemTreeItem->setClientResource(true);
    itemTreeItem->setItem(item);
    this->registerResource(object->GetUuid(), index);
    }
  else if((bitstream = dynamic_cast<mdo::Bitstream*>(object)) != NULL)
    {
    QList<QVariant> columnData;
    columnData << bitstream->GetName().c_str();

    QModelIndex parentIndex = this->getIndexByUuid(bitstream->GetParentItem()->GetUuid());
    MidasItemTreeItem* parent = dynamic_cast<MidasItemTreeItem*>(const_cast<MidasTreeItem*>(this->midasTreeItem(parentIndex)));
    MidasBitstreamTreeItem* bitstreamItem = new MidasBitstreamTreeItem(columnData, this, parent);
    bitstreamItem->setClientResource(true);
    bitstreamItem->setBitstream(bitstream);
    bitstreamItem->setDecorationRole(MidasTreeItem::Dirty);

    this->beginInsertRows(parentIndex, parent->childCount(), parent->childCount());
    parent->appendChild(bitstreamItem);
    this->endInsertRows();
    QModelIndex index = this->index(parent->childCount()-1, 0, parentIndex);

    this->registerResource(object->GetUuid(), index);
    }
  else return;

  emit layoutChanged();
}

void MidasTreeModelClient::updateResource(mdo::Object* object)
{
  //TODO
}

void MidasTreeModelClient::deleteResource(mdo::Object* object)
{
  QModelIndex index = this->getIndexByUuid(object->GetUuid());
  MidasTreeItem* treeItem = const_cast<MidasTreeItem*>(this->midasTreeItem(index));
  treeItem->removeFromTree();
  emit layoutChanged();
}

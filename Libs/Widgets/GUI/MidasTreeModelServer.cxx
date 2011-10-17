#include "MidasTreeModelServer.h"

#include <QPixmap>
#include <QApplication>

#include "MidasTreeItem.h"
#include "MidasCommunityTreeItem.h"
#include "MidasCollectionTreeItem.h"
#include "MidasItemTreeItem.h"
#include "MidasBitstreamTreeItem.h"

#include "mdoCollection.h"
#include "mwsCollection.h"
#include "mdoItem.h"
#include "mwsItem.h"
#include "mdoBitstream.h"

#include <iostream>
#include <mdoCommunity.h>
#include <mwsCommunity.h>
#include "midasSynchronizer.h"

MidasTreeModelServer::MidasTreeModelServer(QObject *parent)
: MidasTreeModel(parent)
{
}

MidasTreeModelServer::~MidasTreeModelServer()
{
}

/** Populate the tree */
void MidasTreeModelServer::Populate()
{
  mws::Community remote;
  mdo::Community* community = new mdo::Community;
  community->SetId(0);
  remote.SetAuthenticator(m_Synch->GetAuthenticator());
  remote.SetObject(community);

  if(!remote.FetchTree())
    {
    return;
    }

  std::vector<mdo::Community*> communities = community->GetCommunities();

  int row = 0;
  for(std::vector<mdo::Community*>::const_iterator itCom = communities.begin();
      itCom != communities.end(); ++itCom)
    {
    // Set the name of the community
    QList<QVariant> columnData;
    columnData << (*itCom)->GetName().c_str();
    
    // Add the community
    MidasCommunityTreeItem* communityItem = new MidasCommunityTreeItem(columnData, this, NULL);
    communityItem->setCommunity(*itCom);
    communityItem->setDynamicFetch(true);
    m_TopLevelCommunities.append(communityItem);

    QModelIndex index = this->index(row, 0);
    registerResource((*itCom)->GetUuid(), index);

    communityItem->populate(index);
    communityItem->setTopLevelCommunities(&m_TopLevelCommunities);
    
    row++;
    }
  emit layoutChanged();
  emit serverPolled();
}

/** Fetch more data */
void MidasTreeModelServer::fetchMore ( const QModelIndex & parent )
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
void MidasTreeModelServer::fetchCollection(MidasCollectionTreeItem* parent)
{
  mws::Collection remote;
  mdo::Collection* collection = parent->getCollection();
  remote.SetAuthenticator(m_Synch->GetAuthenticator());
  remote.SetObject(collection);

  if(!remote.Fetch())
    {
    //emit fetchError();
    return;
    }

  int row = 0;
  for(std::vector<mdo::Item*>::const_iterator i = collection->GetItems().begin();
      i != collection->GetItems().end(); ++i)
    {
    QList<QVariant> name;
    name << (*i)->GetTitle().c_str();
    MidasItemTreeItem* item = new MidasItemTreeItem(name, this, parent);
    item->setItem(*i);
    item->setDynamicFetch(true);
    item->setFetchedChildren(false);
    parent->appendChild(item);

    QModelIndex index = this->index(row, 0, getIndexByUuid(parent->getUuid()));
    registerResource((*i)->GetUuid(), index);
    row++;
    }
  emit layoutChanged();
}

//-------------------------------------------------------------------------
void MidasTreeModelServer::fetchItem(MidasItemTreeItem* parent)
{
  mws::Item remote;
  mdo::Item* item = parent->getItem();
  remote.SetAuthenticator(m_Synch->GetAuthenticator());
  remote.SetObject(item);

  if(!remote.Fetch())
    {
    //emit fetchError();
    return;
    }

  int row = 0;
  for(std::vector<mdo::Bitstream*>::const_iterator i = item->GetBitstreams().begin();
      i != item->GetBitstreams().end(); ++i)
    {
    QList<QVariant> name;
    name << (*i)->GetName().c_str();
    MidasBitstreamTreeItem* bitstream = new MidasBitstreamTreeItem(name, this, parent);
    bitstream->setBitstream(*i);
    parent->appendChild(bitstream);
    QModelIndex index = this->index(row, 0, getIndexByUuid(parent->getUuid()));
    registerResource((*i)->GetUuid(), index);
    row++;
    }
  emit layoutChanged();
}

//-------------------------------------------------------------------------
void MidasTreeModelServer::itemExpanded ( const QModelIndex & index )
{
  MidasTreeItem * item = const_cast<MidasTreeItem *>(this->midasTreeItem(index));
  item->setDecorationRole(MidasTreeItem::Expanded);

  if(this->AlterList && item->getType() == midasResourceType::COMMUNITY)
    {
    m_ExpandedList.insert(item->getUuid());
    }
}

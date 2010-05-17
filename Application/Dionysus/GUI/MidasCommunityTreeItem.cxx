#include "MidasCommunityTreeItem.h"
#include "MidasCollectionTreeItem.h"

#include <mdoCommunity.h>
#include <iostream>
#include <QPixmap>
#include <QStyle>

MidasCommunityTreeItem::MidasCommunityTreeItem(const QList<QVariant> &itemData, MidasTreeItem *parent):
MidasTreeItem(itemData, parent)
{
  m_Community = NULL;
}

MidasCommunityTreeItem::~MidasCommunityTreeItem()
{
}

void MidasCommunityTreeItem::setCommunity(mdo::Community* community)
{
  m_Community = community;
}

int MidasCommunityTreeItem::getId() const
{
  return this->m_Community->GetId();
}

std::string MidasCommunityTreeItem::getUuid() const
{
  return m_Community->GetUuid();
}


void MidasCommunityTreeItem::populate()
{
  if(!m_Community)
    {
    std::cerr << "Community not set" << std::endl;
    return;
    } 
  
  // Add the collections for the community
  std::vector<mdo::Collection*>::const_iterator itCol = m_Community->GetCollections().begin();
  while(itCol != m_Community->GetCollections().end())
    {
    QList<QVariant> name;
    name << (*itCol)->GetName().c_str();
    MidasCollectionTreeItem* collection = new MidasCollectionTreeItem(name, this);
    collection->setCollection(*itCol);

    if(this->isDynamicFetch())
      {
      collection->setFetchedChildren(false);
      collection->setDynamicFetch(true);
      }
    else
      {
      collection->populate();
      }

    if((*itCol)->IsDirty())
      {
      collection->setDecorationRole(MidasTreeItem::Dirty);
      }
    this->appendChild(collection);
    itCol++;
    }

  // Add the subcommunities for the community
  std::vector<mdo::Community*>::const_iterator itCom = m_Community->GetCommunities().begin();
  while(itCom != m_Community->GetCommunities().end())
    {
    QList<QVariant> name;
    name << (*itCom)->GetName().c_str();
    MidasCommunityTreeItem* community = new MidasCommunityTreeItem(name, this);
    community->setCommunity(*itCom);

    if(this->isDynamicFetch())
      {
      community->setDynamicFetch(true);
      }
    community->populate();
    if((*itCom)->IsDirty())
      {
      community->setDecorationRole(MidasTreeItem::Dirty);
      }
    this->appendChild(community);
    itCom++;
    }
  this->setFetchedChildren( true );
}

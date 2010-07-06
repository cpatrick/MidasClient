#include "MidasCommunityTreeItem.h"
#include "MidasCollectionTreeItem.h"
#include "midasStandardIncludes.h"

#include <mdoCommunity.h>
#include <iostream>
#include <QPixmap>
#include <QStyle>
#include <QModelIndex>

MidasCommunityTreeItem::MidasCommunityTreeItem(const QList<QVariant> &itemData, MidasTreeItem *parent, MidasTreeModel* model):
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

int MidasCommunityTreeItem::getType() const
{
  return midasResourceType::COMMUNITY;
}

int MidasCommunityTreeItem::getId() const
{
  return this->m_Community->GetId();
}

std::string MidasCommunityTreeItem::getUuid() const
{
  return m_Community->GetUuid();
}


void MidasCommunityTreeItem::populate(QModelIndex parent)
{
  if(!m_Community)
    {
    std::cerr << "Community not set" << std::endl;
    return;
    }
  
  // Add the collections for the community
  std::vector<mdo::Collection*>::const_iterator itCol = m_Community->GetCollections().begin();
  int row = 0;
  while(itCol != m_Community->GetCollections().end())
    {
    QList<QVariant> name;
    name << (*itCol)->GetName().c_str();
    MidasCollectionTreeItem* collection = new MidasCollectionTreeItem(name, this);
    collection->setCollection(*itCol);

    QModelIndex index = index(
    

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
    row++;
    }

  // Add the subcommunities for the community
  std::vector<mdo::Community*>::const_iterator itCom = m_Community->GetCommunities().begin();
  while(itCom != m_Community->GetCommunities().end())
    {
    QList<QVariant> name;
    name << (*itCom)->GetName().c_str();
    MidasCommunityTreeItem* community = new MidasCommunityTreeItem(name, this);
    community->setCommunity(*itCom);

    QModelIndex index(row, 0, parent);

    if(this->isDynamicFetch())
      {
      community->setDynamicFetch(true);
      }
    community->populate(index);
    if((*itCom)->IsDirty())
      {
      community->setDecorationRole(MidasTreeItem::Dirty);
      }
    this->appendChild(community);
    itCom++;
    row++;
    }
  this->setFetchedChildren( true );
}

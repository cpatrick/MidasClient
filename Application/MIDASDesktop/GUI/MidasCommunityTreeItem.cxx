#include "MidasCommunityTreeItem.h"
#include "MidasCollectionTreeItem.h"
#include "midasStandardIncludes.h"
#include "MidasTreeModel.h"

#include <mdoCommunity.h>
#include <iostream>
#include <QPixmap>
#include <QStyle>
#include <QModelIndex>

MidasCommunityTreeItem::MidasCommunityTreeItem(const QList<QVariant> &itemData, MidasTreeModel* model, MidasTreeItem* parent):
MidasTreeItem(itemData, model, parent)
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
    MidasCollectionTreeItem* collection = new MidasCollectionTreeItem(name, m_Model, this);
    collection->setClientResource(m_ClientResource);
    collection->setCollection(*itCol);

    this->appendChild(collection);
    QModelIndex index = m_Model->index(row, 0, parent);
    m_Model->registerResource((*itCol)->GetUuid(), index);

    if(this->isDynamicFetch())
      {
      collection->setFetchedChildren(false);
      collection->setDynamicFetch(true);
      }
    else
      {
      collection->populate(index);
      }

    if((*itCol)->IsDirty())
      {
      collection->setDecorationRole(MidasTreeItem::Dirty);
      }
    itCol++;
    row++;
    }

  // Add the subcommunities for the community
  std::vector<mdo::Community*>::const_iterator itCom = m_Community->GetCommunities().begin();
  while(itCom != m_Community->GetCommunities().end())
    {
    QList<QVariant> name;
    name << (*itCom)->GetName().c_str();
    MidasCommunityTreeItem* community = new MidasCommunityTreeItem(name, m_Model, this);
    community->setClientResource(m_ClientResource);
    community->setCommunity(*itCom);

    this->appendChild(community);
    QModelIndex index = m_Model->index(row, 0, parent);
    m_Model->registerResource((*itCom)->GetUuid(), index);

    if(this->isDynamicFetch())
      {
      community->setDynamicFetch(true);
      }
    if(!m_ClientResource) //server gives us subcommunities and collections; client only gives top level
      {
      community->populate(index);
      }
    if((*itCom)->IsDirty())
      {
      community->setDecorationRole(MidasTreeItem::Dirty);
      }
    itCom++;
    row++;
    }
  if(!this->isDynamicFetch())
    {
    this->setFetchedChildren( true );
    }
}

void MidasCommunityTreeItem::updateDisplayName()
{
  QVariant name = this->getCommunity()->GetName().c_str();
  this->setData(name,0);
}

void MidasCommunityTreeItem::removeFromTree()
{
}

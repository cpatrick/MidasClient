#include "MidasCommunityTreeItem.h"
#include "MidasCollectionTreeItem.h"
#include "midasStandardIncludes.h"
#include "MidasTreeModel.h"

#include <mdoCommunity.h>
#include <iostream>
#include <QPixmap>
#include <QStyle>
#include <QModelIndex>

MidasCommunityTreeItem::MidasCommunityTreeItem(const QList<QVariant> &itemData,
                                               MidasTreeModel* model, MidasTreeItem* parent)
: MidasTreeItem(itemData, model, parent)
{
  m_Community = NULL;
}

MidasCommunityTreeItem::~MidasCommunityTreeItem()
{
}

void MidasCommunityTreeItem::SetCommunity(mdo::Community* community)
{
  m_Community = community;
}

int MidasCommunityTreeItem::GetType() const
{
  return midasResourceType::COMMUNITY;
}

int MidasCommunityTreeItem::GetId() const
{
  return this->m_Community->GetId();
}

std::string MidasCommunityTreeItem::GetUuid() const
{
  return m_Community->GetUuid();
}


void MidasCommunityTreeItem::Populate(QModelIndex parent)
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
    collection->SetClientResource(m_ClientResource);
    collection->SetCollection(*itCol);

    this->AppendChild(collection);
    QModelIndex index = m_Model->index(row, 0, parent);
    m_Model->registerResource((*itCol)->GetUuid(), index);

    if(this->IsDynamicFetch())
      {
      collection->SetFetchedChildren(false);
      collection->SetDynamicFetch(true);
      }
    else
      {
      collection->Populate(index);
      }

    if((*itCol)->IsDirty())
      {
      collection->SetDecorationRole(MidasTreeItem::Dirty);
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
    community->SetClientResource(m_ClientResource);
    community->SetCommunity(*itCom);

    this->AppendChild(community);
    QModelIndex index = m_Model->index(row, 0, parent);
    m_Model->registerResource((*itCom)->GetUuid(), index);

    if(this->IsDynamicFetch())
      {
      community->SetDynamicFetch(true);
      }
    if(!m_ClientResource) //server gives us subcommunities and collections; client only gives top level
      {
      community->Populate(index);
      }
    if((*itCom)->IsDirty())
      {
      community->SetDecorationRole(MidasTreeItem::Dirty);
      }
    itCom++;
    row++;
    }
  if(!this->IsDynamicFetch())
    {
    this->SetFetchedChildren( true );
    }
}

void MidasCommunityTreeItem::UpdateDisplayName()
{
  QVariant name = this->GetCommunity()->GetName().c_str();
  this->SetData(name,0);
}

void MidasCommunityTreeItem::RemoveFromTree()
{
}

mdo::Community* MidasCommunityTreeItem::GetCommunity() const
{
  return m_Community;
}

mdo::Object* MidasCommunityTreeItem::GetObject() const
{
  return m_Community;
}

bool MidasCommunityTreeItem::ResourceIsFetched() const
{
  return m_Community->IsFetched();
}

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

MidasTreeModelClient::MidasTreeModelClient(QObject *parent) : MidasTreeModel(parent)
{
  this->AlterList = true;
}

MidasTreeModelClient::~MidasTreeModelClient()
{
}

void MidasTreeModelClient::SetLog(midasLog* log)
{
  this->m_Log = log;
}

/** Populate the tree */
void MidasTreeModelClient::Populate()
{
  this->m_Database->Open();
  std::vector<mdo::Community*> topLevelCommunities =
    this->m_Database->GetTopLevelCommunities(true);
  
  this->beginInsertRows(QModelIndex(), 0, topLevelCommunities.size());
  int row = 0;
  for(std::vector<mdo::Community*>::iterator i = topLevelCommunities.begin();
      i != topLevelCommunities.end(); ++i)
    {
    QList<QVariant> columnData;
    columnData << (*i)->GetName().c_str();

    MidasCommunityTreeItem* communityItem = new MidasCommunityTreeItem(columnData, this, NULL);
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
  this->m_Database->Close();
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

void MidasTreeModelClient::itemExpanded ( const QModelIndex & index )
{
  MidasTreeItem * item = const_cast<MidasTreeItem *>(this->midasTreeItem(index));
  item->setDecorationRole(MidasTreeItem::Expanded);
  
  if(this->AlterList)
    {
    m_ExpandedList.insert(item->getUuid());
    }
}

void MidasTreeModelClient::itemCollapsed ( const QModelIndex & index )
{
  MidasTreeItem * item = const_cast<MidasTreeItem *>(this->midasTreeItem(index));
  item->setDecorationRole(MidasTreeItem::Collapsed);
  
  m_ExpandedList.erase(item->getUuid());
}

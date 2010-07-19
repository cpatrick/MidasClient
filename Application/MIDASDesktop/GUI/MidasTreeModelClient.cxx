#include "MidasTreeModelClient.h"

#include <QPixmap>
#include <QApplication>


#include "MidasTreeItem.h"
#include "MidasCommunityTreeItem.h"
#include "MidasCollectionTreeItem.h"
#include "MidasItemTreeItem.h"

#include "Utils.h"
#include <iostream>
#include "midasDatabaseProxy.h"
#include "mdoCommunity.h"

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

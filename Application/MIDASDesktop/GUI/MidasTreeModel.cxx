#include "MidasTreeModel.h"
#include "MidasTreeItem.h"
#include "MidasCommunityTreeItem.h"
#include "mdoCommunity.h"

MidasTreeModel::MidasTreeModel(QObject *parent) : QAbstractItemModel(parent)
{
  this->AlterList = true;
}

MidasTreeModel::~MidasTreeModel()
{
}

void MidasTreeModel::clear()
{
  for(QList<MidasCommunityTreeItem*>::iterator i = m_TopLevelCommunities.begin();
      i != m_TopLevelCommunities.end(); ++i)
    {
    delete (*i)->getCommunity();
    }
  m_TopLevelCommunities.clear();
  m_IndexMap.clear();
}

void MidasTreeModel::registerResource(std::string uuid, QModelIndex index)
{
  m_IndexMap[uuid] = index;
}

QModelIndex MidasTreeModel::getIndexByUuid(std::string uuid)
{
  if(m_IndexMap.find(uuid) != m_IndexMap.end())
    {
    return m_IndexMap[uuid];
    }
  else
    {
    return QModelIndex();
    }
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

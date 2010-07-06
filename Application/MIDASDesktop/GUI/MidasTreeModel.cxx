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


void MidasTreeModel::restoreExpandedState()
{
  //copy list so we can remove while iterating
  std::set<std::string> copy = m_ExpandedList;
  for(std::set<std::string>::iterator i = copy.begin();
      i != copy.end(); ++i)
    {
    QModelIndex index = this->getIndexByUuid(*i);
    if(index.isValid())
      {
      this->AlterList = false;
      emit expand(index);
      this->AlterList = true;
      }
    else
      {
      //If we can't resolve the list item, we should remove it from the list.
      //(Probably no longer exists due to deletion)
      m_ExpandedList.erase(*i);
      }
    }
}

int MidasTreeModel::rowCount(const QModelIndex &parent) const
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
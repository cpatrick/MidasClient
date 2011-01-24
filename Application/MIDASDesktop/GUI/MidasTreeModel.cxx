#include "MidasTreeModel.h"
#include "MidasTreeItem.h"
#include "MidasCommunityTreeItem.h"
#include "mdoCommunity.h"
#include <QPixmap>

MidasTreeModel::MidasTreeModel(QObject *parent) : QAbstractItemModel(parent)
{
  this->AlterList = true;
}

MidasTreeModel::~MidasTreeModel()
{
}

//-------------------------------------------------------------------------
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

//-------------------------------------------------------------------------
void MidasTreeModel::registerResource(std::string uuid, QModelIndex index)
{
  m_IndexMap[uuid] = index;
}

void MidasTreeModel::unregisterResource(std::string uuid)
{
  m_IndexMap.erase(uuid);
}

//-------------------------------------------------------------------------
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

//-------------------------------------------------------------------------
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

//-------------------------------------------------------------------------
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

//-------------------------------------------------------------------------
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

//-------------------------------------------------------------------------
int MidasTreeModel::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    {
    return static_cast<MidasTreeItem*>(parent.internalPointer())->columnCount();
    }
  return 1;
}

//-------------------------------------------------------------------------
QVariant MidasTreeModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    {
    return QVariant();
    }
  MidasTreeItem *item = static_cast<MidasTreeItem*>(index.internalPointer());
  if ( role == Qt::DisplayRole )
    {
    return item->data(index.column());
    }
  else if ( role == Qt::DecorationRole )
    {
    return item->getDecoration();
    }
  else 
    {
    return QVariant();
    }
}

//-------------------------------------------------------------------------
Qt::ItemFlags MidasTreeModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    {
    return 0;
    }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//-------------------------------------------------------------------------
QModelIndex MidasTreeModel::parent(const QModelIndex &index) const
  {
  if (!index.isValid())
    {
    return QModelIndex();
    }

  MidasTreeItem *childItem = static_cast<MidasTreeItem*>(index.internalPointer());
  MidasTreeItem *parentItem = childItem->parent();

  if (parentItem == NULL)
    {
    return QModelIndex();
    }
  return createIndex(parentItem->row(), 0, parentItem);
}

//-------------------------------------------------------------------------
QVariant MidasTreeModel::headerData(int section, 
                                    Qt::Orientation orientation,
                                    int role) const
{
  /*if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
    return m_TopLevelCommunities.data(section);
    }
    */
  return QVariant();
}

//-------------------------------------------------------------------------
bool MidasTreeModel::canFetchMore ( const QModelIndex & parent ) const
{
  if ( !parent.isValid() )
    {
    return false;
    }
  const MidasTreeItem* item = (this->midasTreeItem(parent)); 
  return !item->isFetchedChildren(); 
}

//-------------------------------------------------------------------------
void MidasTreeModel::fetchMore ( const QModelIndex & parent )
{
}

//-------------------------------------------------------------------------
void MidasTreeModel::clearExpandedList()
{
  m_ExpandedList.clear();
}

//-------------------------------------------------------------------------
void MidasTreeModel::expandAllResources()
{
  m_ExpandedList.clear();
  
  for(std::map<std::string, QModelIndex>::iterator i = m_IndexMap.begin();
      i != m_IndexMap.end(); ++i)
    {
    m_ExpandedList.insert(i->first);
    }
}

//-------------------------------------------------------------------------
void MidasTreeModel::itemExpanded ( const QModelIndex & index )
{
  MidasTreeItem * item = const_cast<MidasTreeItem *>(this->midasTreeItem(index));
  item->setDecorationRole(MidasTreeItem::Expanded);

  if(this->AlterList)
    {
    m_ExpandedList.insert(item->getUuid());
    }
}

//-------------------------------------------------------------------------
void MidasTreeModel::itemCollapsed ( const QModelIndex & index )
{
  MidasTreeItem* item = const_cast<MidasTreeItem*>(this->midasTreeItem(index));
  item->setDecorationRole(MidasTreeItem::Collapsed);

  m_ExpandedList.erase(item->getUuid());
}

//-------------------------------------------------------------------------
void MidasTreeModel::decorateByUuid(std::string uuid)
{
  QModelIndex index = getIndexByUuid(uuid);

  if(index.isValid())
    {
    MidasTreeItem* node = const_cast<MidasTreeItem*>(
      this->midasTreeItem(index));
    node->setDecorationRole(MidasTreeItem::Dirty);
    
    MidasTreeItem* item =
      const_cast<MidasTreeItem*>(this->midasTreeItem(index));
    item->updateDisplayName();

    emit dataChanged(index, index);
    }
}

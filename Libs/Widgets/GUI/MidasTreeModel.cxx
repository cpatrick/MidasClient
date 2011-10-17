#include "MidasTreeModel.h"
#include "MidasTreeItem.h"
#include "MidasCommunityTreeItem.h"
#include "midasSynchronizer.h"
#include <QPixmap>

MidasTreeModel::MidasTreeModel(QObject *parent)
: QAbstractItemModel(parent), m_Synch(NULL)
{
  this->AlterList = true;
}

MidasTreeModel::~MidasTreeModel()
{
}

void MidasTreeModel::SetSynchronizer(midasSynchronizer* synch)
{
  m_Synch = synch;
}

//-------------------------------------------------------------------------
void MidasTreeModel::clear()
{
  this->beginResetModel();
  for(QList<MidasCommunityTreeItem*>::iterator i = m_TopLevelCommunities.begin();
      i != m_TopLevelCommunities.end(); ++i)
    {
    delete (*i)->GetCommunity();
    delete *i;
    }
  m_TopLevelCommunities.clear();
  m_IndexMap.clear();
  this->reset();
  this->endResetModel();
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
  MidasTreeItem* childItem = parentItem->GetChild(row);
  if (childItem)
    {
    return this->createIndex(row, column, childItem);
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
  return parentItem->ChildCount();
}

//-------------------------------------------------------------------------
int MidasTreeModel::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    {
    return static_cast<MidasTreeItem*>(parent.internalPointer())->ColumnCount();
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
    return item->GetData(index.column());
    }
  else if ( role == Qt::DecorationRole )
    {
    return item->GetDecoration();
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
  MidasTreeItem *parentItem = childItem->GetParent();

  if (parentItem == NULL)
    {
    return QModelIndex();
    }
  return createIndex(parentItem->GetRow(), 0, parentItem);
}

bool MidasTreeModel::hasChildren( const QModelIndex & parent ) const
{
  if (!parent.isValid())
    {
    return true;
    }
  const MidasTreeItem* item = midasTreeItem(parent);
  if(item->IsFetchedChildren())
    {
    return item->ChildCount() > 0;
    }
  else
    {
    return true;
    }
}

//-------------------------------------------------------------------------
QVariant MidasTreeModel::headerData(int section, 
                                    Qt::Orientation orientation,
                                    int role) const
{
  (void)section;
  (void)role;
  (void)orientation;
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
  return !item->IsFetchedChildren(); 
}

//-------------------------------------------------------------------------
void MidasTreeModel::fetchMore ( const QModelIndex & parent )
{
  (void)parent;
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
  item->SetDecorationRole(MidasTreeItem::Expanded);

  if(this->AlterList)
    {
    m_ExpandedList.insert(item->GetUuid());
    }
}

//-------------------------------------------------------------------------
void MidasTreeModel::itemCollapsed ( const QModelIndex & index )
{
  MidasTreeItem* item = const_cast<MidasTreeItem*>(this->midasTreeItem(index));
  item->SetDecorationRole(MidasTreeItem::Collapsed);

  m_ExpandedList.erase(item->GetUuid());
}

//-------------------------------------------------------------------------
void MidasTreeModel::decorateByUuid(std::string uuid)
{
  QModelIndex index = getIndexByUuid(uuid);

  if(index.isValid())
    {
    MidasTreeItem* node = const_cast<MidasTreeItem*>(
      this->midasTreeItem(index));
    node->SetDecorationRole(MidasTreeItem::Dirty);
    
    MidasTreeItem* item =
      const_cast<MidasTreeItem*>(this->midasTreeItem(index));
    item->UpdateDisplayName();

    emit dataChanged(index, index);
    }
}

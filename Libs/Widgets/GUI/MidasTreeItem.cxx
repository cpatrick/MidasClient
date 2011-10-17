#include "MidasTreeItem.h"
#include "MidasCommunityTreeItem.h"

#include <QStringList>
#include <QPixmap>
#include <QStyle>
#include <QTime>

MidasTreeItem::MidasTreeItem(const QList<QVariant> &itemData, MidasTreeModel* model, MidasTreeItem *parent):
  decorationRole(Collapsed), m_Model(model), itemData(itemData), parentItem(parent), lifespan(600)
{
  timestamp = QTime::currentTime().second();
  this->fetchedChildren = true;
  this->dynamicFetch = false;
  m_ClientResource = false;
}

MidasTreeItem::~MidasTreeItem()
{
  qDeleteAll(this->childItems);
}

void MidasTreeItem::setDynamicFetch(bool value)
{
  this->dynamicFetch = value;
}

bool MidasTreeItem::isDynamicFetch() const
{
  return this->dynamicFetch;
}

bool MidasTreeItem::operator==(const MidasTreeItem* other) const 
{
  return (this->getId() == other->getId());
}

bool MidasTreeItem::isValid() const
{
  uint current = QTime::currentTime().second();
  return (this->timestamp + this->lifespan > current); 
}

void MidasTreeItem::appendChild(MidasTreeItem *item)
{
  this->childItems.append(item);
}

void MidasTreeItem::removeChild(MidasTreeItem *item)
{
  this->childItems.removeAt(this->childItems.indexOf(item));
}

void MidasTreeItem::removeAllChild()
{
  MidasTreeItem * midasTreeItem = NULL; 
  foreach(midasTreeItem, childItems)
    {
    removeChild(midasTreeItem);
    }
}

MidasTreeItem *MidasTreeItem::child(int row)
{
  return this->childItems.value(row);
}

void MidasTreeItem::setFetchedChildren(bool value)
{
  this->fetchedChildren = value;
}

bool MidasTreeItem::isFetchedChildren() const
{
  return this->fetchedChildren;
}

int MidasTreeItem::childCount() const
{
  return this->childItems.count();
}

int MidasTreeItem::columnCount() const
{
  return this->itemData.count();
}

void MidasTreeItem::setData(const QVariant& value, int column)
{
  this->itemData.replace(column, value); 
}

QVariant MidasTreeItem::data(int column) const
{
  return this->itemData.value(column);
}

MidasTreeItem *MidasTreeItem::parent()
{
  return this->parentItem;
}

const MidasTreeItem *MidasTreeItem::parent() const
{
  return this->parentItem;
}

int MidasTreeItem::row() const
{
  if (this->parentItem)
    {
    return this->parentItem->childItems.indexOf(
      const_cast<MidasTreeItem*>(this));
    }
  return this->m_TopLevelCommunities->indexOf(
    reinterpret_cast<MidasCommunityTreeItem*>(
    const_cast<MidasTreeItem*>(this)));
}

QPixmap MidasTreeItem::getDecoration()
{
  std::string role = ":icons/gpl_folder";
  if ( this->decorationRole & Expanded )
    {
    role += "_open";
    }
  if ( this->decorationRole & Dirty )
    {
    role += "_red";
    }
  role += ".png";
  return QPixmap(role.c_str());
}

void MidasTreeItem::setDecorationRole(DecorationRoles role)
{
  if(this->decorationRole & Dirty)
    {
    this->decorationRole = role | Dirty;
    }
  else
    {
    this->decorationRole = role; 
    }
}

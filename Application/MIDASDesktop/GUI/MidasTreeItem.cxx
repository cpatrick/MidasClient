#include <QStringList>

#include "MidasTreeItem.h"
#include "MidasCommunityTreeItem.h"
#include "MidasCollectionTreeItem.h"
#include "MidasItemTreeItem.h"
#include "MidasBitstreamTreeItem.h"
#include "Utils.h"
#include "MidasClientGlobal.h"
#include <QPixmap>
#include <QStyle>
#include <QTime>

MidasTreeItem::MidasTreeItem(const QList<QVariant> &itemData, MidasTreeItem *parent):
  lifespan(600),  
  parentItem(parent), itemData(itemData),
  decorationRole(Collapsed)
{
  m_WebAPI = NULL;
  timestamp = QTime::currentTime().second();
  this->fetchedChildren = true;
  this->dynamicFetch = false;
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

/** Set the web API */
void MidasTreeItem::SetWebAPI(mws::WebAPI* api)
{
  m_WebAPI = api;
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

void MidasTreeItem::populate()
{
}

void MidasTreeItem::appendChild(MidasTreeItem *item)
{
  this->childItems.append(item);
}

void MidasTreeItem::removeChild(MidasTreeItem *item)
{
  this->childItems.removeOne(item);
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
  return QPixmap(role.c_str()).scaledToHeight(TREE_ITEM_ICON_HEIGHT);
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

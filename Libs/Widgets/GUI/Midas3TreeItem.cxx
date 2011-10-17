#include "Midas3TreeItem.h"
#include "Midas3FolderTreeItem.h"

#include <QStringList>
#include <QPixmap>
#include <QStyle>
#include <QTime>

Midas3TreeItem::Midas3TreeItem(const QList<QVariant> &itemData, Midas3TreeModel* model, Midas3TreeItem *parent):
  decorationRole(Collapsed), m_Model(model), itemData(itemData), parentItem(parent), lifespan(600)
{
  timestamp = QTime::currentTime().second();
  this->fetchedChildren = true;
  this->dynamicFetch = false;
  m_ClientResource = false;
}

Midas3TreeItem::~Midas3TreeItem()
{
  qDeleteAll(this->childItems);
}

void Midas3TreeItem::setDynamicFetch(bool value)
{
  this->dynamicFetch = value;
}

bool Midas3TreeItem::isDynamicFetch() const
{
  return this->dynamicFetch;
}

bool Midas3TreeItem::operator==(const Midas3TreeItem* other) const 
{
  return (this->getId() == other->getId());
}

bool Midas3TreeItem::isValid() const
{
  uint current = QTime::currentTime().second();
  return (this->timestamp + this->lifespan > current); 
}

void Midas3TreeItem::appendChild(Midas3TreeItem* item)
{
  this->childItems.append(item);
}

void Midas3TreeItem::removeChild(Midas3TreeItem *item)
{
  this->childItems.removeAt(this->childItems.indexOf(item));
}

void Midas3TreeItem::removeAllChild()
{
  Midas3TreeItem* midasTreeItem = NULL; 
  foreach(midasTreeItem, childItems)
    {
    removeChild(midasTreeItem);
    }
}

Midas3TreeItem* Midas3TreeItem::child(int row)
{
  return this->childItems.value(row);
}

void Midas3TreeItem::setFetchedChildren(bool value)
{
  this->fetchedChildren = value;
}

bool Midas3TreeItem::isFetchedChildren() const
{
  return this->fetchedChildren;
}

int Midas3TreeItem::childCount() const
{
  return this->childItems.count();
}

int Midas3TreeItem::columnCount() const
{
  return this->itemData.count();
}

void Midas3TreeItem::setData(const QVariant& value, int column)
{
  this->itemData.replace(column, value); 
}

QVariant Midas3TreeItem::data(int column) const
{
  return this->itemData.value(column);
}

Midas3TreeItem* Midas3TreeItem::parent()
{
  return this->parentItem;
}

const Midas3TreeItem* Midas3TreeItem::parent() const
{
  return this->parentItem;
}

int Midas3TreeItem::row() const
{
  if (this->parentItem)
    {
    return this->parentItem->childItems.indexOf(
      const_cast<Midas3TreeItem*>(this));
    }
  return this->m_TopLevelFolders->indexOf(
    reinterpret_cast<Midas3FolderTreeItem*>(
    const_cast<Midas3TreeItem*>(this)));
}

QPixmap Midas3TreeItem::getDecoration()
{
  std::string role = ":icons/gpl_folder";
  if (this->decorationRole & Expanded)
    {
    role += "_open";
    }
  if (this->decorationRole & Dirty)
    {
    role += "_red";
    }
  role += ".png";
  return QPixmap(role.c_str());
}

void Midas3TreeItem::setDecorationRole(DecorationRoles role)
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

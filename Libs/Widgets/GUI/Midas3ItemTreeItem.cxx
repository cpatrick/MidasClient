#include "Midas3ItemTreeItem.h"
#include "midasStandardIncludes.h"
#include "Midas3TreeModel.h"
#include "m3doItem.h"

#include <iostream>
#include <QPixmap>
#include <QStyle>
#include <QModelIndex>

Midas3ItemTreeItem::Midas3ItemTreeItem(const QList<QVariant> &itemData,
                                           Midas3TreeModel* model,
                                           Midas3TreeItem* parent)
: Midas3TreeItem(itemData, model, parent)
{
  m_Item = NULL;
}

Midas3ItemTreeItem::~Midas3ItemTreeItem()
{
}

void Midas3ItemTreeItem::setItem(m3do::Item* item)
{
  m_Item = item;
}

int Midas3ItemTreeItem::getType() const
{
  return midas3ResourceType::ITEM;
}

int Midas3ItemTreeItem::getId() const
{
  return this->m_Item->GetId();
}

std::string Midas3ItemTreeItem::getUuid() const
{
  return m_Item->GetUuid();
}

std::string Midas3ItemTreeItem::getPath() const
{
  return m_Item->GetPath();
}

void Midas3ItemTreeItem::populate(QModelIndex parent)
{
  if(!m_Item)
    {
    std::cerr << "Item not set" << std::endl;
    return;
    }
  
  (void)parent;
  // TODO Add the revisions
  /*std::vector<m3do::Item*>::const_iterator f = m_Item->GetItems().begin();
  int row = 0;
  while(f != m_Item->GetItems().end())
    {
    QList<QVariant> name;
    name << (*f)->GetName().c_str();
    Midas3ItemTreeItem* Item = new Midas3ItemTreeItem(name, m_Model, this);
    Item->setClientResource(m_ClientResource);
    Item->setItem(*f);

    this->appendChild(Item);
    QModelIndex index = m_Model->index(row, 0, parent);
    m_Model->registerResource((*f)->GetUuid(), index);

    if(this->isDynamicFetch())
      {
      Item->setFetchedChildren(false);
      Item->setDynamicFetch(true);
      }
    else
      {
      Item->populate(index);
      }

    if((*f)->IsDirty())
      {
      Item->setDecorationRole(Midas3TreeItem::Dirty);
      }
    f++;
    row++;
    }
*/
  //if(!this->isDynamicFetch())
    {
    this->setFetchedChildren(false);
    }
}

void Midas3ItemTreeItem::updateDisplayName()
{
  QVariant name = m_Item->GetName().c_str();
  this->setData(name,0);
}

void Midas3ItemTreeItem::removeFromTree()
{
}

m3do::Item* Midas3ItemTreeItem::getItem() const
{
  return m_Item;
}

mdo::Object* Midas3ItemTreeItem::getObject() const
{
  return m_Item;
}

bool Midas3ItemTreeItem::resourceIsFetched() const
{
  return m_Item->IsFetched();
}

QPixmap Midas3ItemTreeItem::getDecoration()
{
  std::string role = ":icons/gpl_document";

  /*if(this->decorationRole & Expanded)
    {
    role += "_open";
    }*/
  if(this->decorationRole & Dirty)
    {
    role += "_red";
    }
  role += ".png";
  return QPixmap(role.c_str());
}

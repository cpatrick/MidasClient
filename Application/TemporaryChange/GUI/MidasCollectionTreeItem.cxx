#include "MidasCollectionTreeItem.h"
#include "MidasItemTreeItem.h"
#include "mdoItem.h"
#include "mdoCollection.h"
#include <QPixmap>
#include <QStyle>

MidasCollectionTreeItem::MidasCollectionTreeItem(const QList<QVariant> &itemData, MidasTreeItem *parent): 
MidasTreeItem(itemData, parent)
{
}

MidasCollectionTreeItem::~MidasCollectionTreeItem()
{
}

int MidasCollectionTreeItem::getId() const
{
  return m_Collection->GetId();
}

std::string MidasCollectionTreeItem::getUuid() const
{
  return m_Collection->GetUuid();
}

void MidasCollectionTreeItem::populate()
{
  if(!m_Collection)
    {
    //error
    return;
    }

  // Add the items for the collection
  std::vector<mdo::Item*>::const_iterator i = m_Collection->GetItems().begin();
  while(i != m_Collection->GetItems().end())
    {
    QList<QVariant> name;
    name << (*i)->GetTitle().c_str();
    MidasItemTreeItem* item = new MidasItemTreeItem(name, this);
    item->setItem(*i);
    item->populate();
    if((*i)->IsDirty())
      {
      item->setDecorationRole(MidasTreeItem::Dirty);
      }
    this->appendChild(item);
    i++;
    }
}

#include "MidasCollectionTreeItem.h"
#include "MidasItemTreeItem.h"
#include "mdoItem.h"
#include "mdoCollection.h"
#include "midasStandardIncludes.h"
#include <QPixmap>
#include <QStyle>

MidasCollectionTreeItem::MidasCollectionTreeItem(const QList<QVariant> &itemData, MidasTreeModel* model, MidasTreeItem *parent): 
MidasTreeItem(itemData, model, parent)
{
}

MidasCollectionTreeItem::~MidasCollectionTreeItem()
{
}

int MidasCollectionTreeItem::getType() const
{
  return midasResourceType::COLLECTION;
}

int MidasCollectionTreeItem::getId() const
{
  return m_Collection->GetId();
}

std::string MidasCollectionTreeItem::getUuid() const
{
  return m_Collection->GetUuid();
}

void MidasCollectionTreeItem::populate(QModelIndex parent)
{
  if(!m_Collection)
    {
    //error
    return;
    }

  // Add the items for the collection
  int row = 0;
  std::vector<mdo::Item*>::const_iterator i = m_Collection->GetItems().begin();
  while(i != m_Collection->GetItems().end())
    {
    QList<QVariant> name;
    name << (*i)->GetTitle().c_str();
    MidasItemTreeItem* item = new MidasItemTreeItem(name, m_Model, this);
    item->setClientResource(m_ClientResource);
    item->setItem(*i);
    this->appendChild(item);

    QModelIndex index = m_Model->index(row, 0, parent);
    m_Model->registerResource((*i)->GetUuid(), index);

    item->populate(index);

    if((*i)->IsDirty())
      {
      item->setDecorationRole(MidasTreeItem::Dirty);
      }
    i++;
    row++;
    }
}

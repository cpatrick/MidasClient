#include "MidasCollectionTreeItem.h"
#include "MidasItemTreeItem.h"
#include "mdoItem.h"
#include "mdoCollection.h"
#include "midasStandardIncludes.h"
#include <QPixmap>
#include <QStyle>

MidasCollectionTreeItem::MidasCollectionTreeItem(const QList<QVariant> & itemData, MidasTreeModel* model,
                                                 MidasTreeItem *parent) :
  MidasTreeItem(itemData, model, parent)
{
}

MidasCollectionTreeItem::~MidasCollectionTreeItem()
{
}

int MidasCollectionTreeItem::GetType() const
{
  return midasResourceType::COLLECTION;
}

int MidasCollectionTreeItem::GetId() const
{
  return m_Collection->GetId();
}

std::string MidasCollectionTreeItem::GetUuid() const
{
  return m_Collection->GetUuid();
}

void MidasCollectionTreeItem::Populate(QModelIndex parent)
{
  if( !m_Collection )
    {
    // error
    return;
    }

  // Add the items for the collection
  int                                      row = 0;
  std::vector<mdo::Item *>::const_iterator i = m_Collection->GetItems().begin();
  while( i != m_Collection->GetItems().end() )
    {
    QList<QVariant> name;
    name << (*i)->GetTitle().c_str();
    MidasItemTreeItem* item = new MidasItemTreeItem(name, m_Model, this);
    item->SetClientResource(m_ClientResource);
    item->SetItem(*i);
    this->AppendChild(item);

    QModelIndex index = m_Model->index(row, 0, parent);
    m_Model->registerResource( (*i)->GetUuid(), index);

    item->Populate(index);

    if( (*i)->IsDirty() )
      {
      item->SetDecorationRole(MidasTreeItem::Dirty);
      }
    i++;
    row++;
    }
}

void MidasCollectionTreeItem::UpdateDisplayName()
{
  QVariant name = this->GetCollection()->GetName().c_str();

  this->SetData(name, 0);
}

void MidasCollectionTreeItem::RemoveFromTree()
{

}

void MidasCollectionTreeItem::SetCollection(mdo::Collection* collection)
{
  m_Collection = collection;
}

mdo::Collection * MidasCollectionTreeItem::GetCollection() const
{
  return m_Collection;
}

mdo::Object * MidasCollectionTreeItem::GetObject() const
{
  return m_Collection;
}

bool MidasCollectionTreeItem::ResourceIsFetched() const
{
  return m_Collection->IsFetched();
}


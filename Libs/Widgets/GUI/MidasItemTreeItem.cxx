#include "MidasItemTreeItem.h"
#include "MidasBitstreamTreeItem.h"
#include "midasStandardIncludes.h"
#include "mdoBitstream.h"
#include "mdoItem.h"
#include <QPixmap>
#include <QStyle>

MidasItemTreeItem::MidasItemTreeItem(const QList<QVariant> &itemData, MidasTreeModel* model, MidasTreeItem *parent):
MidasTreeItem(itemData, model, parent)
{
  //this->fetchedChildren = true; 
}

MidasItemTreeItem::~MidasItemTreeItem()
{
}

int MidasItemTreeItem::getType() const
{
  return midasResourceType::ITEM;
}

int MidasItemTreeItem::getId() const
{
  return m_Item->GetId();
}

std::string MidasItemTreeItem::getUuid() const
{
  return m_Item->GetUuid();
}

void MidasItemTreeItem::populate(QModelIndex parent)
{
  if(!m_Item)
    {
    //error
    return;
    }

  // Add the bitstreams for the item
  int row = 0;
  std::vector<mdo::Bitstream*>::const_iterator i = m_Item->GetBitstreams().begin();
  while(i != m_Item->GetBitstreams().end())
    {
    QList<QVariant> name;
    name << (*i)->GetName().c_str();
    MidasBitstreamTreeItem* bitstream = new MidasBitstreamTreeItem(name, m_Model, this);
    bitstream->setClientResource(m_ClientResource);
    bitstream->setBitstream(*i);
    this->appendChild(bitstream);
    QModelIndex index = m_Model->index(row, 0, parent);
    m_Model->registerResource((*i)->GetUuid(), index);

    if((*i)->IsDirty())
      {
      bitstream->setDecorationRole(MidasTreeItem::Dirty);
      }
    bitstream->populate(index);
    i++;
    row++;
    }
  this->setFetchedChildren( true );
}

void MidasItemTreeItem::updateDisplayName()
{
  QVariant name = this->getItem()->GetTitle().c_str();
  this->setData(name,0);
}

void MidasItemTreeItem::removeFromTree()
{
}

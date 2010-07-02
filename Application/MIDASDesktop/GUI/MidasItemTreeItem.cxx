#include "MidasItemTreeItem.h"
#include "MidasBitstreamTreeItem.h"
#include "MidasClientGlobal.h"
#include "midasStandardIncludes.h"
#include "mdoBitstream.h"
#include "mdoItem.h"
#include <QPixmap>
#include <QStyle>

MidasItemTreeItem::MidasItemTreeItem(const QList<QVariant> &itemData, MidasTreeItem *parent):
MidasTreeItem(itemData, parent)
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

void MidasItemTreeItem::populate()
{
  if(!m_Item)
    {
    //error
    return;
    }

  // Add the bitstreams for the item
  std::vector<mdo::Bitstream*>::const_iterator i = m_Item->GetBitstreams().begin();
  while(i != m_Item->GetBitstreams().end())
    {
    QList<QVariant> name;
    name << (*i)->GetName().c_str();
    MidasBitstreamTreeItem* bitstream = new MidasBitstreamTreeItem(name, this);
    bitstream->setBitstream(*i);
    if((*i)->IsDirty())
      {
      bitstream->setDecorationRole(MidasTreeItem::Dirty);
      }
    this->appendChild(bitstream);
    i++;
    }
  this->setFetchedChildren( true );
}

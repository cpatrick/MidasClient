#include "MidasBitstreamTreeItem.h"
#include "MidasClientGlobal.h"
#include "mdoBitstream.h"
#include <QPixmap>
#include <QStyle>

MidasBitstreamTreeItem::MidasBitstreamTreeItem(const QList<QVariant> &itemData, MidasTreeItem *parent):
MidasTreeItem(itemData, parent)
{
  //this->fetchedChildren = true; 
}

MidasBitstreamTreeItem::~MidasBitstreamTreeItem()
{
}

int MidasBitstreamTreeItem::getId() const
{
  return m_Bitstream->GetId();
}

std::string MidasBitstreamTreeItem::getUuid() const
{
  return m_Bitstream->GetUuid();
}

QPixmap MidasBitstreamTreeItem::getDecoration()
{
  std::string role = ":icons/gpl_document";
  if ( this->decorationRole & Dirty )
    {
    role += "_red";
    }
  role += ".png";
  return QPixmap(role.c_str()).scaledToHeight(TREE_ITEM_ICON_HEIGHT); 
}

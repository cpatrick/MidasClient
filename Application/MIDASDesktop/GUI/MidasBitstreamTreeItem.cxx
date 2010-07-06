#include "MidasBitstreamTreeItem.h"
#include "MidasClientGlobal.h"
#include "MidasTreeModel.h"
#include "mdoBitstream.h"
#include "midasStandardIncludes.h"
#include <QPixmap>
#include <QStyle>

MidasBitstreamTreeItem::MidasBitstreamTreeItem(const QList<QVariant> &itemData, MidasTreeModel* model, MidasTreeItem *parent):
MidasTreeItem(itemData, model, parent)
{
  //this->fetchedChildren = true; 
}

MidasBitstreamTreeItem::~MidasBitstreamTreeItem()
{
}

int MidasBitstreamTreeItem::getType() const
{
  return midasResourceType::BITSTREAM;
}

int MidasBitstreamTreeItem::getId() const
{
  return m_Bitstream->GetId();
}

std::string MidasBitstreamTreeItem::getUuid() const
{
  return m_Bitstream->GetUuid();
}

void MidasBitstreamTreeItem::populate(QModelIndex parent)
{
}

QPixmap MidasBitstreamTreeItem::getDecoration()
{
  std::string role = ":icons/gpl_document";
  if ( this->decorationRole & Dirty )
    {
    role += "_red";
    }
  role += ".png";
  return QPixmap(role.c_str()); 
}

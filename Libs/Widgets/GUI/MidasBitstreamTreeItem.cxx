#include "MidasBitstreamTreeItem.h"
#include "MidasItemTreeItem.h"
#include "MidasTreeModel.h"
#include "mdoBitstream.h"
#include "midasStandardIncludes.h"
#include "mdoItem.h"
#include <QPixmap>
#include <QStyle>

MidasBitstreamTreeItem::MidasBitstreamTreeItem(const QList<QVariant> &itemData,
                                               MidasTreeModel* model, MidasTreeItem *parent)
: MidasTreeItem(itemData, model, parent)
{
  //this->fetchedChildren = true; 
}

MidasBitstreamTreeItem::~MidasBitstreamTreeItem()
{
}

int MidasBitstreamTreeItem::GetType() const
{
  return midasResourceType::BITSTREAM;
}

int MidasBitstreamTreeItem::GetId() const
{
  return m_Bitstream->GetId();
}

std::string MidasBitstreamTreeItem::GetUuid() const
{
  return m_Bitstream->GetUuid();
}

void MidasBitstreamTreeItem::Populate(QModelIndex parent)
{
  (void)parent;
}

QPixmap MidasBitstreamTreeItem::GetDecoration()
{
  std::string role = ":icons/gpl_document";
  if ( m_DecorationRole & Dirty )
    {
    role += "_red";
    }
  role += ".png";
  return QPixmap(role.c_str());
}

void MidasBitstreamTreeItem::UpdateDisplayName()
{
  QVariant name = this->GetBitstream()->GetName().c_str();
  this->SetData(name, 0);
}

void MidasBitstreamTreeItem::RemoveFromTree()
{
}

void MidasBitstreamTreeItem::SetBitstream(mdo::Bitstream* bitstream)
{
  m_Bitstream = bitstream;
}

mdo::Bitstream* MidasBitstreamTreeItem::GetBitstream() const
{
  return m_Bitstream;
}

mdo::Object* MidasBitstreamTreeItem::GetObject() const
{
  return m_Bitstream;
}

bool MidasBitstreamTreeItem::ResourceIsFetched() const
{
  return m_Bitstream->IsFetched();
}

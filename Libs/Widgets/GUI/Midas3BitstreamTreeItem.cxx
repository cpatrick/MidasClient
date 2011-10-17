#include "Midas3BitstreamTreeItem.h"
#include "Midas3ItemTreeItem.h"
#include "Midas3TreeModel.h"
#include "m3doBitstream.h"
#include "m3doItem.h"
#include "midasStandardIncludes.h"

#include <QPixmap>
#include <QStyle>

Midas3BitstreamTreeItem::Midas3BitstreamTreeItem(
  const QList<QVariant>& itemData, Midas3TreeModel* model,
  Midas3TreeItem* parent)
: Midas3TreeItem(itemData, model, parent)
{
  //this->fetchedChildren = true; 
}

Midas3BitstreamTreeItem::~Midas3BitstreamTreeItem()
{
}

int Midas3BitstreamTreeItem::getType() const
{
  return midas3ResourceType::BITSTREAM;
}

int Midas3BitstreamTreeItem::getId() const
{
  return m_Bitstream->GetId();
}

std::string Midas3BitstreamTreeItem::getUuid() const
{
  std::stringstream uuid;
  uuid << m_Bitstream->GetChecksum() << m_Bitstream->GetId();
  return uuid.str();
}

std::string Midas3BitstreamTreeItem::getPath() const
{
  return m_Bitstream->GetPath();
}

void Midas3BitstreamTreeItem::populate(QModelIndex parent)
{
  (void)parent;
}

QPixmap Midas3BitstreamTreeItem::getDecoration()
{
  std::string role = ":icons/page";
  if(this->decorationRole & Dirty)
    {
    role += "_red";
    }
  role += ".png";
  return QPixmap(role.c_str());
}

void Midas3BitstreamTreeItem::updateDisplayName()
{
  QVariant name = this->getBitstream()->GetName().c_str();
  this->setData(name,0);
}

void Midas3BitstreamTreeItem::removeFromTree()
{
}

mdo::Object* Midas3BitstreamTreeItem::getObject() const
{
  return m_Bitstream;
}

m3do::Bitstream* Midas3BitstreamTreeItem::getBitstream() const
{
  return m_Bitstream;
}

void Midas3BitstreamTreeItem::setBitstream(m3do::Bitstream* bitstream)
{
  m_Bitstream = bitstream;
}

bool Midas3BitstreamTreeItem::resourceIsFetched() const
{
  return true;
}



/******************************************************************************
 * Copyright 2011 Kitware Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
#include "Midas3BitstreamTreeItem.h"
#include "Midas3ItemTreeItem.h"
#include "Midas3TreeModel.h"
#include "m3doBitstream.h"
#include "m3doItem.h"
#include "midasStandardIncludes.h"

#include <QPixmap>
#include <QStyle>

Midas3BitstreamTreeItem::Midas3BitstreamTreeItem(const QList<QVariant>& itemData, Midas3TreeModel* model,
                                                 Midas3TreeItem* parent)
  : Midas3TreeItem(itemData, model, parent)
{
  // this->fetchedChildren = true;
}

Midas3BitstreamTreeItem::~Midas3BitstreamTreeItem()
{
}

int Midas3BitstreamTreeItem::GetType() const
{
  return midas3ResourceType::BITSTREAM;
}

int Midas3BitstreamTreeItem::GetId() const
{
  return m_Bitstream->GetId();
}

std::string Midas3BitstreamTreeItem::GetUuid() const
{
  std::stringstream uuid;

  uuid << m_Bitstream->GetChecksum() << m_Bitstream->GetId();
  return uuid.str();
}

std::string Midas3BitstreamTreeItem::GetPath() const
{
  return m_Bitstream->GetPath();
}

void Midas3BitstreamTreeItem::Populate(QModelIndex parent)
{
  (void)parent;
}

QPixmap Midas3BitstreamTreeItem::GetDecoration()
{
  std::string role = ":icons/page";

  if( m_DecorationRole & Dirty )
    {
    role += "_red";
    }
  role += ".png";
  return QPixmap(role.c_str() );
}

void Midas3BitstreamTreeItem::UpdateDisplayName()
{
  QVariant name = this->GetBitstream()->GetName().c_str();

  this->SetData(name, 0);
}

void Midas3BitstreamTreeItem::RemoveFromTree()
{
}

mdo::Object * Midas3BitstreamTreeItem::GetObject() const
{
  return m_Bitstream;
}

m3do::Bitstream * Midas3BitstreamTreeItem::GetBitstream() const
{
  return m_Bitstream;
}

void Midas3BitstreamTreeItem::SetBitstream(m3do::Bitstream* bitstream)
{
  m_Bitstream = bitstream;
}

bool Midas3BitstreamTreeItem::ResourceIsFetched() const
{
  return true;
}


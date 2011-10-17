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
#include "MidasItemTreeItem.h"
#include "MidasBitstreamTreeItem.h"
#include "midasStandardIncludes.h"
#include "mdoBitstream.h"
#include "mdoItem.h"
#include <QPixmap>
#include <QStyle>

MidasItemTreeItem::MidasItemTreeItem(const QList<QVariant> & itemData, MidasTreeModel* model, MidasTreeItem *parent)
  : MidasTreeItem(itemData, model, parent)
{
  // this->fetchedChildren = true;
}

MidasItemTreeItem::~MidasItemTreeItem()
{
}

int MidasItemTreeItem::GetType() const
{
  return midasResourceType::ITEM;
}

int MidasItemTreeItem::GetId() const
{
  return m_Item->GetId();
}

std::string MidasItemTreeItem::GetUuid() const
{
  return m_Item->GetUuid();
}

void MidasItemTreeItem::Populate(QModelIndex parent)
{
  if( !m_Item )
    {
    // error
    return;
    }

  // Add the bitstreams for the item
  int                                           row = 0;
  std::vector<mdo::Bitstream *>::const_iterator i = m_Item->GetBitstreams().begin();
  while( i != m_Item->GetBitstreams().end() )
    {
    QList<QVariant> name;
    name << (*i)->GetName().c_str();
    MidasBitstreamTreeItem* bitstream = new MidasBitstreamTreeItem(name, m_Model, this);
    bitstream->SetClientResource(m_ClientResource);
    bitstream->SetBitstream(*i);
    this->AppendChild(bitstream);
    QModelIndex index = m_Model->index(row, 0, parent);
    m_Model->RegisterResource( (*i)->GetUuid(), index);

    if( (*i)->IsDirty() )
      {
      bitstream->SetDecorationRole(MidasTreeItem::Dirty);
      }
    bitstream->Populate(index);
    i++;
    row++;
    }

  this->SetFetchedChildren( true );
}

void MidasItemTreeItem::UpdateDisplayName()
{
  QVariant name = this->GetItem()->GetTitle().c_str();

  this->SetData(name, 0);
}

void MidasItemTreeItem::RemoveFromTree()
{
}

void MidasItemTreeItem::SetItem(mdo::Item* item)
{
  m_Item = item;
}

mdo::Item * MidasItemTreeItem::GetItem() const
{
  return m_Item;
}

mdo::Object * MidasItemTreeItem::GetObject() const
{
  return m_Item;
}

bool MidasItemTreeItem::ResourceIsFetched() const
{
  return m_Item->IsFetched();
}


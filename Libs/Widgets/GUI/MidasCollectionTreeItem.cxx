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


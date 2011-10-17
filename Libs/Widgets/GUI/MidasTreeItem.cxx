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
#include "MidasTreeItem.h"
#include "MidasCommunityTreeItem.h"

#include <QStringList>
#include <QPixmap>
#include <QStyle>
#include <QTime>

MidasTreeItem::MidasTreeItem(const QList<QVariant> & itemData, MidasTreeModel* model, MidasTreeItem *parent)
  : m_DecorationRole(Collapsed), m_Model(model), m_ItemData(itemData), m_ParentItem(parent), m_Lifespan(600)
{
  m_Timestamp = QTime::currentTime().second();
  m_FetchedChildren = true;
  m_DynamicFetch = false;
  m_ClientResource = false;
}

MidasTreeItem::~MidasTreeItem()
{
  qDeleteAll(m_ChildItems);
}

void MidasTreeItem::SetDynamicFetch(bool value)
{
  m_DynamicFetch = value;
}

bool MidasTreeItem::IsDynamicFetch() const
{
  return m_DynamicFetch;
}

bool MidasTreeItem::operator==(const MidasTreeItem* other) const
{
  return this->GetId() == other->GetId();
}

bool MidasTreeItem::IsValid() const
{
  uint current = QTime::currentTime().second();

  return m_Timestamp + m_Lifespan > current;
}

void MidasTreeItem::AppendChild(MidasTreeItem *item)
{
  m_ChildItems.append(item);
}

void MidasTreeItem::RemoveChild(MidasTreeItem *item)
{
  m_ChildItems.removeAt(m_ChildItems.indexOf(item) );
}

void MidasTreeItem::RemoveAllChildren()
{
  MidasTreeItem* midasTreeItem = NULL;

  foreach(midasTreeItem, m_ChildItems)
    {
    this->RemoveChild(midasTreeItem);
    }
}

MidasTreeItem * MidasTreeItem::GetChild(int row)
{
  return m_ChildItems.value(row);
}

void MidasTreeItem::SetFetchedChildren(bool value)
{
  m_FetchedChildren = value;
}

bool MidasTreeItem::IsFetchedChildren() const
{
  return m_FetchedChildren;
}

int MidasTreeItem::ChildCount() const
{
  return m_ChildItems.count();
}

int MidasTreeItem::ColumnCount() const
{
  return m_ItemData.count();
}

void MidasTreeItem::SetData(const QVariant& value, int column)
{
  m_ItemData.replace(column, value);
}

QVariant MidasTreeItem::GetData(int column) const
{
  return m_ItemData.value(column);
}

MidasTreeItem * MidasTreeItem::GetParent()
{
  return m_ParentItem;
}

const MidasTreeItem * MidasTreeItem::GetParent() const
{
  return m_ParentItem;
}

int MidasTreeItem::GetRow() const
{
  if( m_ParentItem )
    {
    return m_ParentItem->m_ChildItems.indexOf(
             const_cast<MidasTreeItem *>(this) );
    }
  return m_TopLevelCommunities->indexOf(
           reinterpret_cast<MidasCommunityTreeItem *>(
             const_cast<MidasTreeItem *>(this) ) );
}

QPixmap MidasTreeItem::GetDecoration()
{
  std::string role = ":icons/gpl_folder";

  if( m_DecorationRole & Expanded )
    {
    role += "_open";
    }
  if( m_DecorationRole & Dirty )
    {
    role += "_red";
    }
  role += ".png";
  return QPixmap(role.c_str() );
}

void MidasTreeItem::SetDecorationRole(DecorationRoles role)
{
  if( m_DecorationRole & Dirty )
    {
    m_DecorationRole = role | Dirty;
    }
  else
    {
    m_DecorationRole = role;
    }
}

QList<MidasTreeItem *> MidasTreeItem::GetChildren()
{
  return m_ChildItems;
}

void MidasTreeItem::SetTopLevelCommunities(
  QList<MidasCommunityTreeItem *>* tlc)
{
  m_TopLevelCommunities = tlc;
}

bool MidasTreeItem::IsClientResource() const
{
  return m_ClientResource;
}

void MidasTreeItem::SetClientResource(bool val)
{
  m_ClientResource = val;
}


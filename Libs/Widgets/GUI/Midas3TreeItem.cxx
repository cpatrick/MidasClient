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
#include "Midas3TreeItem.h"
#include "Midas3FolderTreeItem.h"

#include <QStringList>
#include <QPixmap>
#include <QStyle>
#include <QTime>

Midas3TreeItem::Midas3TreeItem(const QList<QVariant> & itemData, Midas3TreeModel* model, Midas3TreeItem *parent)
  : m_DecorationRole(Collapsed), m_Model(model), m_ItemData(itemData), m_ParentItem(parent), m_Lifespan(600)
{
  m_Timestamp = QTime::currentTime().second();
  m_FetchedChildren = true;
  m_DynamicFetch = false;
  m_ClientResource = false;
}

Midas3TreeItem::~Midas3TreeItem()
{
  qDeleteAll(m_ChildItems);
}

void Midas3TreeItem::SetDynamicFetch(bool value)
{
  m_DynamicFetch = value;
}

bool Midas3TreeItem::IsDynamicFetch() const
{
  return m_DynamicFetch;
}

bool Midas3TreeItem::operator==(const Midas3TreeItem* other) const
{
  return this->GetId() == other->GetId();
}

bool Midas3TreeItem::IsValid() const
{
  uint current = QTime::currentTime().second();

  return m_Timestamp + m_Lifespan > current;
}

void Midas3TreeItem::AppendChild(Midas3TreeItem* item)
{
  m_ChildItems.append(item);
}

void Midas3TreeItem::RemoveChild(Midas3TreeItem *item)
{
  m_ChildItems.removeAt(m_ChildItems.indexOf(item) );
}

void Midas3TreeItem::RemoveAllChildren()
{
  Midas3TreeItem* midasTreeItem = NULL;

  foreach(midasTreeItem, m_ChildItems)
    {
    this->RemoveChild(midasTreeItem);
    }
}

Midas3TreeItem * Midas3TreeItem::GetChild(int row)
{
  return m_ChildItems.value(row);
}

void Midas3TreeItem::SetFetchedChildren(bool value)
{
  m_FetchedChildren = value;
}

bool Midas3TreeItem::IsFetchedChildren() const
{
  return m_FetchedChildren;
}

int Midas3TreeItem::ChildCount() const
{
  return m_ChildItems.count();
}

int Midas3TreeItem::ColumnCount() const
{
  return m_ItemData.count();
}

void Midas3TreeItem::SetData(const QVariant& value, int column)
{
  m_ItemData.replace(column, value);
}

QVariant Midas3TreeItem::GetData(int column) const
{
  return m_ItemData.value(column);
}

Midas3TreeItem * Midas3TreeItem::GetParent()
{
  return m_ParentItem;
}

const Midas3TreeItem * Midas3TreeItem::GetParent() const
{
  return m_ParentItem;
}

int Midas3TreeItem::GetRow() const
{
  if( m_ParentItem )
    {
    return m_ParentItem->m_ChildItems.indexOf(
             const_cast<Midas3TreeItem *>(this) );
    }
  return m_TopLevelFolders->indexOf(
           reinterpret_cast<Midas3FolderTreeItem *>(
             const_cast<Midas3TreeItem *>(this) ) );
}

QPixmap Midas3TreeItem::GetDecoration()
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

void Midas3TreeItem::SetDecorationRole(DecorationRoles role)
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

void Midas3TreeItem::SetTopLevelFolders(QList<Midas3FolderTreeItem *>* tlf)
{
  m_TopLevelFolders = tlf;
}

bool Midas3TreeItem::IsClientResource() const
{
  return m_ClientResource;
}

void Midas3TreeItem::SetClientResource(bool val)
{
  m_ClientResource = val;
}

QList<Midas3TreeItem *> Midas3TreeItem::GetChildren()
{
  return m_ChildItems;
}


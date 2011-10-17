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
#include "Midas3TreeModelClient.h"

#include <QPixmap>
#include <QApplication>
#include <iostream>

#include "Midas3TreeItem.h"
#include "Midas3FolderTreeItem.h"
#include "Midas3ItemTreeItem.h"
#include "Midas3BitstreamTreeItem.h"

#include "mdsDatabaseAPI.h"
#include "m3doCommunity.h"
#include "m3doFolder.h"
#include "m3dsFolder.h"
#include "m3doItem.h"
#include "m3dsItem.h"
#include "m3doBitstream.h"
#include "m3dsBitstream.h"
#include "mdoObject.h"

Midas3TreeModelClient::Midas3TreeModelClient(QObject *parent)
  : Midas3TreeModel(parent)
{
  this->AlterList = true;
}

Midas3TreeModelClient::~Midas3TreeModelClient()
{
}

/** Populate the tree */
void Midas3TreeModelClient::Populate()
{
  mds::DatabaseAPI db;

  std::vector<m3do::Folder *> topLevelFolders = db.GetTopLevelFolders();

  this->beginInsertRows(QModelIndex(), 0, topLevelFolders.size() );
  int row = 0;
  for( std::vector<m3do::Folder *>::iterator i = topLevelFolders.begin();
       i != topLevelFolders.end(); ++i )
    {
    QList<QVariant> columnData;
    columnData << (*i)->GetName().c_str();

    Midas3FolderTreeItem* folderItem = new Midas3FolderTreeItem(columnData, this, NULL);
    folderItem->SetClientResource(true);
    folderItem->SetFolder(*i);
    folderItem->SetDynamicFetch(true);
    folderItem->SetFetchedChildren(false);
    m_TopLevelFolders.append(folderItem);

    QModelIndex index = this->index(row, 0);
    registerResource( (*i)->GetUuid(), index);

    folderItem->Populate(index);
    folderItem->SetTopLevelFolders(&m_TopLevelFolders);
    if( (*i)->IsDirty() )
      {
      folderItem->SetDecorationRole(Midas3TreeItem::Dirty);
      }
    row++;
    }
  this->endInsertRows();
  emit layoutChanged();
}

void Midas3TreeModelClient::addResource(mdo::Object* object)
{
  m3do::Folder*    folderPtr = NULL;
  m3do::Item*      itemPtr = NULL;
  m3do::Bitstream* bitstreamPtr = NULL;

  if( (folderPtr = dynamic_cast<m3do::Folder *>(object) ) != NULL )
    {
    m3do::Folder* folder = // preserve this pointer for use in the tree
      folderPtr->GetResourceType() == midas3ResourceType::COMMUNITY ?
      new m3do::Community(dynamic_cast<m3do::Community *>(folderPtr) ) :
      new m3do::Folder(folderPtr);
    QList<QVariant> columnData;
    columnData << folder->GetName().c_str();
    Midas3FolderTreeItem* folderItem;

    QModelIndex index;
    if( folder->GetParentFolder() == NULL )
      {
      folderItem = new Midas3FolderTreeItem(columnData, this, NULL);
      folderItem->SetTopLevelFolders(&m_TopLevelFolders);
      folderItem->SetFolder(folder);

      int rows = this->rowCount(QModelIndex() );
      this->beginInsertRows(QModelIndex(), rows, rows);
      m_TopLevelFolders.append(folderItem);
      this->endInsertRows();
      index = this->index(rows, 0);
      }
    else
      {
      QModelIndex parentIndex = this->getIndexByUuid(folder->GetParentFolder()->GetUuid() );
      if( !parentIndex.isValid() )
        {
        return;
        }
      Midas3FolderTreeItem* parent =
        dynamic_cast<Midas3FolderTreeItem *>(const_cast<Midas3TreeItem *>(this->midasTreeItem(parentIndex) ) );
      folderItem = new Midas3FolderTreeItem(columnData, this, parent);
      folderItem->SetFolder(folder);

      this->beginInsertRows(parentIndex, parent->ChildCount(), parent->ChildCount() );
      parent->AppendChild(folderItem);
      this->endInsertRows();

      index = this->index(parent->ChildCount() - 1, 0, parentIndex);
      }
    folderItem->SetClientResource(true);

    this->registerResource(object->GetUuid(), index);
    }
  else if( (itemPtr = dynamic_cast<m3do::Item *>(object) ) != NULL )
    {
    m3do::Item*     item = new m3do::Item(itemPtr); // preserve this pointer for
                                                    // use in the tree
    QList<QVariant> columnData;
    columnData << item->GetName().c_str();

    QModelIndex parentIndex = this->getIndexByUuid(item->GetParentFolder()->GetUuid() );
    if( !parentIndex.isValid() )
      {
      return;
      }
    Midas3FolderTreeItem* parent =
      dynamic_cast<Midas3FolderTreeItem *>(const_cast<Midas3TreeItem *>(this->midasTreeItem(parentIndex) ) );
    Midas3ItemTreeItem* itemTreeItem = new Midas3ItemTreeItem(columnData, this, parent);
    this->beginInsertRows(parentIndex, parent->ChildCount(), parent->ChildCount() );
    parent->AppendChild(itemTreeItem);
    this->endInsertRows();
    QModelIndex index = this->index(parent->ChildCount() - 1, 0, parentIndex);

    itemTreeItem->SetClientResource(true);
    itemTreeItem->SetItem(item);
    this->registerResource(object->GetUuid(), index);
    }
  else if( (bitstreamPtr = dynamic_cast<m3do::Bitstream *>(object) ) != NULL )
    {
    m3do::Bitstream* bitstream = new m3do::Bitstream(bitstreamPtr);
    QList<QVariant>  columnData;
    columnData << bitstream->GetName().c_str();

    QModelIndex parentIndex = this->getIndexByUuid(bitstream->GetParentItem()->GetUuid() );
    if( !parentIndex.isValid() )
      {
      return;
      }
    Midas3ItemTreeItem* parent =
      dynamic_cast<Midas3ItemTreeItem *>(const_cast<Midas3TreeItem *>(this->midasTreeItem(parentIndex) ) );
    Midas3BitstreamTreeItem* bitstreamTreeItem = new Midas3BitstreamTreeItem(columnData, this, parent);
    this->beginInsertRows(parentIndex, parent->ChildCount(), parent->ChildCount() );
    parent->AppendChild(bitstreamTreeItem);
    this->endInsertRows();
    QModelIndex index = this->index(parent->ChildCount() - 1, 0, parentIndex);

    bitstreamTreeItem->SetClientResource(true);
    bitstreamTreeItem->SetBitstream(bitstream);
    std::stringstream uuid;
    uuid << bitstream->GetChecksum() << bitstream->GetId();
    this->registerResource(uuid.str(), index);
    }
  else
    {
    return;
    }

  emit layoutChanged();
}

void Midas3TreeModelClient::updateResource(mdo::Object* object)
{
  // TODO
  (void)object;
}

void Midas3TreeModelClient::deleteResource(mdo::Object* object)
{
  /*QModelIndex index = this->getIndexByUuid(object->GetUuid());
  MidasTreeItem* treeItem = const_cast<MidasTreeItem*>(this->midasTreeItem(index));
  treeItem->removeFromTree();
  emit layoutChanged();*/
  (void)object;
}

void Midas3TreeModelClient::fetchMore(const QModelIndex& parent)
{
  if( !parent.isValid() || !canFetchMore(parent) )
    {
    return;
    }
  Midas3TreeItem*       item = const_cast<Midas3TreeItem *>(midasTreeItem(parent) );
  Midas3FolderTreeItem* folderTreeItem = NULL;
  Midas3ItemTreeItem*   itemTreeItem = NULL;

  if( (folderTreeItem = dynamic_cast<Midas3FolderTreeItem *>(const_cast<Midas3TreeItem *>(item) ) ) != NULL )
    {
    this->fetchFolder(folderTreeItem);
    }
  else if( (itemTreeItem = dynamic_cast<Midas3ItemTreeItem *>(const_cast<Midas3TreeItem *>(item) ) ) != NULL )
    {
    this->fetchItem(itemTreeItem);
    }
  item->SetFetchedChildren(true);
  emit layoutChanged();
}

// -------------------------------------------------------------------------
void Midas3TreeModelClient::fetchFolder(Midas3FolderTreeItem* parent)
{
  m3ds::Folder  mdsFolder;
  m3do::Folder* folder = parent->GetFolder();

  mdsFolder.SetObject(folder);
  mdsFolder.SetRecursive(false);

  if( !mdsFolder.FetchTree() )
    {
    // emit fetchError();
    return;
    }

  int row = 0;
  for( std::vector<m3do::Folder *>::const_iterator i = folder->GetFolders().begin();
       i != folder->GetFolders().end(); ++i )
    {
    QList<QVariant> name;
    name << (*i)->GetName().c_str();
    Midas3FolderTreeItem* item = new Midas3FolderTreeItem(name, this, parent);
    item->SetFolder(*i);
    item->SetDynamicFetch(true);
    item->SetFetchedChildren(false);
    if( (*i)->IsDirty() )
      {
      item->SetDecorationRole(Midas3TreeItem::Dirty);
      }
    parent->AppendChild(item);

    QModelIndex index = this->index(row, 0, this->getIndexByUuid(parent->GetUuid() ) );
    this->registerResource( (*i)->GetUuid(), index);
    row++;
    }
  for( std::vector<m3do::Item *>::const_iterator i = folder->GetItems().begin();
       i != folder->GetItems().end(); ++i )
    {
    QList<QVariant> name;
    name << (*i)->GetName().c_str();
    Midas3ItemTreeItem* item = new Midas3ItemTreeItem(name, this, parent);
    item->SetItem(*i);
    item->SetDynamicFetch(true);
    item->SetFetchedChildren(false);
    if( (*i)->IsDirty() )
      {
      item->SetDecorationRole(Midas3TreeItem::Dirty);
      }
    parent->AppendChild(item);

    QModelIndex index = this->index(row, 0, this->getIndexByUuid(parent->GetUuid() ) );
    this->registerResource( (*i)->GetUuid(), index);
    row++;
    }
  emit layoutChanged();
}

// -------------------------------------------------------------------------
void Midas3TreeModelClient::fetchItem(Midas3ItemTreeItem* parent)
{
  m3ds::Item  mdsItem;
  m3do::Item* item = parent->GetItem();

  mdsItem.SetObject(item);

  if( !mdsItem.FetchTree() )
    {
    // emit fetchError();
    return;
    }

  int row = 0;
  for( std::vector<m3do::Bitstream *>::const_iterator i = item->GetBitstreams().begin();
       i != item->GetBitstreams().end(); ++i )
    {
    QList<QVariant> name;
    name << (*i)->GetName().c_str();
    Midas3BitstreamTreeItem* bitstream = new Midas3BitstreamTreeItem(name, this, parent);
    bitstream->SetBitstream(*i);
    bitstream->SetDynamicFetch(false);
    bitstream->SetFetchedChildren(true);
    if( (*i)->IsDirty() )
      {
      bitstream->SetDecorationRole(Midas3TreeItem::Dirty);
      }
    parent->AppendChild(bitstream);

    QModelIndex       index = this->index(row, 0, this->getIndexByUuid(parent->GetUuid() ) );
    std::stringstream uuid;
    uuid << (*i)->GetChecksum() << (*i)->GetId(); // bitstreams have no uuid, so
                                                  // we fudge one
    registerResource(uuid.str(), index);
    row++;
    }
  emit layoutChanged();
}


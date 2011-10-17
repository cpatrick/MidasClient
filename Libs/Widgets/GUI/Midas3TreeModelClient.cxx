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
  std::vector<m3do::Folder*> topLevelFolders = db.GetTopLevelFolders();

  this->beginInsertRows(QModelIndex(), 0, topLevelFolders.size());
  int row = 0;
  for(std::vector<m3do::Folder*>::iterator i = topLevelFolders.begin();
      i != topLevelFolders.end(); ++i)
    {
    QList<QVariant> columnData;
    columnData << (*i)->GetName().c_str();

    Midas3FolderTreeItem* folderItem = new Midas3FolderTreeItem(columnData, this, NULL);
    folderItem->setClientResource(true);
    folderItem->setFolder(*i);
    folderItem->setDynamicFetch(true);
    folderItem->setFetchedChildren(false);
    m_TopLevelFolders.append(folderItem);

    QModelIndex index = this->index(row, 0);
    registerResource((*i)->GetUuid(), index);

    folderItem->populate(index);
    folderItem->setTopLevelFolders(&m_TopLevelFolders);
    if((*i)->IsDirty())
      {
      folderItem->setDecorationRole(Midas3TreeItem::Dirty);
      }
    row++;
    }
  this->endInsertRows();
  emit layoutChanged();
}

void Midas3TreeModelClient::addResource(mdo::Object* object)
{
  m3do::Folder* folderPtr = NULL;
  m3do::Item* itemPtr = NULL;
  m3do::Bitstream* bitstreamPtr = NULL;

  if((folderPtr = dynamic_cast<m3do::Folder*>(object)) != NULL)
    {
    m3do::Folder* folder = //preserve this pointer for use in the tree
      folderPtr->GetResourceType() == midas3ResourceType::COMMUNITY ?
      new m3do::Community(dynamic_cast<m3do::Community*>(folderPtr)) :
      new m3do::Folder(folderPtr);
    QList<QVariant> columnData;
    columnData << folder->GetName().c_str();
    Midas3FolderTreeItem* folderItem;

    QModelIndex index;
    if(folder->GetParentFolder() == NULL)
      {
      folderItem = new Midas3FolderTreeItem(columnData, this, NULL);
      folderItem->setTopLevelFolders(&m_TopLevelFolders);
      folderItem->setFolder(folder);

      int rows = this->rowCount(QModelIndex());
      this->beginInsertRows(QModelIndex(), rows, rows);
      m_TopLevelFolders.append(folderItem);
      this->endInsertRows();
      index = this->index(rows, 0);
      }
    else
      {
      QModelIndex parentIndex = this->getIndexByUuid(folder->GetParentFolder()->GetUuid());
      if(!parentIndex.isValid())
        {
        return;
        }
      Midas3FolderTreeItem* parent = dynamic_cast<Midas3FolderTreeItem*>(const_cast<Midas3TreeItem*>(this->midasTreeItem(parentIndex)));
      folderItem = new Midas3FolderTreeItem(columnData, this, parent);
      folderItem->setFolder(folder);

      this->beginInsertRows(parentIndex, parent->childCount(), parent->childCount());
      parent->appendChild(folderItem);
      this->endInsertRows();

      index = this->index(parent->childCount() - 1, 0, parentIndex);
      }
    folderItem->setClientResource(true);

    this->registerResource(object->GetUuid(), index);
    }
  else if((itemPtr = dynamic_cast<m3do::Item*>(object)) != NULL)
    {
    m3do::Item* item = new m3do::Item(itemPtr); //preserve this pointer for use in the tree
    QList<QVariant> columnData;
    columnData << item->GetName().c_str();

    QModelIndex parentIndex = this->getIndexByUuid(item->GetParentFolder()->GetUuid());
    if(!parentIndex.isValid())
      {
      return;
      }
    Midas3FolderTreeItem* parent = dynamic_cast<Midas3FolderTreeItem*>(const_cast<Midas3TreeItem*>(this->midasTreeItem(parentIndex)));
    Midas3ItemTreeItem* itemTreeItem = new Midas3ItemTreeItem(columnData, this, parent);
    this->beginInsertRows(parentIndex, parent->childCount(), parent->childCount());
    parent->appendChild(itemTreeItem);
    this->endInsertRows();
    QModelIndex index = this->index(parent->childCount() - 1, 0, parentIndex);

    itemTreeItem->setClientResource(true);
    itemTreeItem->setItem(item);
    this->registerResource(object->GetUuid(), index);
    }
  else if((bitstreamPtr = dynamic_cast<m3do::Bitstream*>(object)) != NULL)
    {
    m3do::Bitstream* bitstream = new m3do::Bitstream(bitstreamPtr);
    QList<QVariant> columnData;
    columnData << bitstream->GetName().c_str();

    QModelIndex parentIndex = this->getIndexByUuid(bitstream->GetParentItem()->GetUuid());
    if(!parentIndex.isValid())
      {
      return;
      }
    Midas3ItemTreeItem* parent = dynamic_cast<Midas3ItemTreeItem*>(const_cast<Midas3TreeItem*>(this->midasTreeItem(parentIndex)));
    Midas3BitstreamTreeItem* bitstreamTreeItem = new Midas3BitstreamTreeItem(columnData, this, parent);
    this->beginInsertRows(parentIndex, parent->childCount(), parent->childCount());
    parent->appendChild(bitstreamTreeItem);
    this->endInsertRows();
    QModelIndex index = this->index(parent->childCount() - 1, 0, parentIndex);

    bitstreamTreeItem->setClientResource(true);
    bitstreamTreeItem->setBitstream(bitstream);
    std::stringstream uuid;
    uuid << bitstream->GetChecksum() << bitstream->GetId();
    this->registerResource(uuid.str(), index);
    }
  else return;

  emit layoutChanged();
}

void Midas3TreeModelClient::updateResource(mdo::Object* object)
{
  //TODO
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
  if(!parent.isValid() || !canFetchMore(parent))
    {
    return;
    }
  Midas3TreeItem* item = const_cast<Midas3TreeItem*>(midasTreeItem(parent));
  Midas3FolderTreeItem* folderTreeItem = NULL;
  Midas3ItemTreeItem* itemTreeItem = NULL;

  if((folderTreeItem = dynamic_cast<Midas3FolderTreeItem*>(const_cast<Midas3TreeItem*>(item))) != NULL)
    {
    this->fetchFolder(folderTreeItem);
    }
  else if((itemTreeItem = dynamic_cast<Midas3ItemTreeItem*>(const_cast<Midas3TreeItem*>(item))) != NULL)
    {
    this->fetchItem(itemTreeItem);
    }
  item->setFetchedChildren(true);
  emit layoutChanged();
}

//-------------------------------------------------------------------------
void Midas3TreeModelClient::fetchFolder(Midas3FolderTreeItem* parent)
{
  m3ds::Folder mdsFolder;
  m3do::Folder* folder = parent->getFolder();
  mdsFolder.SetObject(folder);
  mdsFolder.SetRecursive(false);

  if(!mdsFolder.FetchTree())
    {
    //emit fetchError();
    return;
    }

  int row = 0;
  for(std::vector<m3do::Folder*>::const_iterator i = folder->GetFolders().begin();
      i != folder->GetFolders().end(); ++i)
    {
    QList<QVariant> name;
    name << (*i)->GetName().c_str();
    Midas3FolderTreeItem* item = new Midas3FolderTreeItem(name, this, parent);
    item->setFolder(*i);
    item->setDynamicFetch(true);
    item->setFetchedChildren(false);
    if((*i)->IsDirty())
      {
      item->setDecorationRole(Midas3TreeItem::Dirty);
      }
    parent->appendChild(item);

    QModelIndex index = this->index(row, 0, getIndexByUuid(parent->getUuid()));
    registerResource((*i)->GetUuid(), index);
    row++;
    }

  for(std::vector<m3do::Item*>::const_iterator i = folder->GetItems().begin();
      i != folder->GetItems().end(); ++i)
    {
    QList<QVariant> name;
    name << (*i)->GetName().c_str();
    Midas3ItemTreeItem* item = new Midas3ItemTreeItem(name, this, parent);
    item->setItem(*i);
    item->setDynamicFetch(true);
    item->setFetchedChildren(false);
    if((*i)->IsDirty())
      {
      item->setDecorationRole(Midas3TreeItem::Dirty);
      }
    parent->appendChild(item);

    QModelIndex index = this->index(row, 0, getIndexByUuid(parent->getUuid()));
    registerResource((*i)->GetUuid(), index);
    row++;
    }
  emit layoutChanged();
}

//-------------------------------------------------------------------------
void Midas3TreeModelClient::fetchItem(Midas3ItemTreeItem* parent)
{
  m3ds::Item mdsItem;
  m3do::Item* item = parent->getItem();
  mdsItem.SetObject(item);

  if(!mdsItem.FetchTree())
    {
    //emit fetchError();
    return;
    }

  int row = 0;
  for(std::vector<m3do::Bitstream*>::const_iterator i = item->GetBitstreams().begin();
      i != item->GetBitstreams().end(); ++i)
    {
    QList<QVariant> name;
    name << (*i)->GetName().c_str();
    Midas3BitstreamTreeItem* bitstream = new Midas3BitstreamTreeItem(name, this, parent);
    bitstream->setBitstream(*i);
    bitstream->setDynamicFetch(false);
    bitstream->setFetchedChildren(true);
    if((*i)->IsDirty())
      {
      bitstream->setDecorationRole(Midas3TreeItem::Dirty);
      }
    parent->appendChild(bitstream);

    QModelIndex index = this->index(row, 0, getIndexByUuid(parent->getUuid()));
    std::stringstream uuid;
    uuid << (*i)->GetChecksum() << (*i)->GetId(); //bitstreams have no uuid, so we fudge one
    registerResource(uuid.str(), index);
    row++;
    }
  emit layoutChanged();
}

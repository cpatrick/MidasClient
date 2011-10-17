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
#include "Midas3TreeViewClient.h"

#include <QtGui>
#include <QItemSelection>
#include <QContextMenuEvent>
#include <QModelIndex>
#include <iostream>

#include "Midas3TreeItem.h"
#include "Midas3TreeModelClient.h"
#include "Midas3FolderTreeItem.h"
#include "Midas3ItemTreeItem.h"
#include "Midas3BitstreamTreeItem.h"
#include "m3dsFolder.h"
#include "m3doFolder.h"
#include "m3dsItem.h"
#include "m3doItem.h"
#include "m3doBitstream.h"
#include "m3dsBitstream.h"
#include "mdoObject.h"

Midas3TreeViewClient::Midas3TreeViewClient(QWidget* parent)
  : Midas3TreeView(parent)
{
  m_Model = new Midas3TreeModelClient;

  this->setModel(m_Model);
  this->setAcceptDrops(true);
  this->setSelectionMode(QTreeView::SingleSelection);

  connect(this, SIGNAL(collapsed(const QModelIndex &) ),
          this->model(), SLOT(itemCollapsed(const QModelIndex &) ) );
  connect(this, SIGNAL(expanded(const QModelIndex &) ),
          this->model(), SLOT(itemExpanded(const QModelIndex &) ) );
  connect(m_Model, SIGNAL(expand(const QModelIndex &) ),
          this, SLOT(expand(const QModelIndex &) ) );

  // define action to be triggered when tree item is selected
  QItemSelectionModel* itemSelectionModel = this->selectionModel();
  connect(itemSelectionModel,
          SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &) ),
          this, SLOT(updateSelection(const QItemSelection &, const QItemSelection &) ) );

  m_MimeType = "MIDAS/client_resource";
  m_AcceptMimeType = "MIDAS/server_resource";
}

Midas3TreeViewClient::~Midas3TreeViewClient()
{
  delete m_Model;
}

void Midas3TreeViewClient::mouseDoubleClickEvent(QMouseEvent* event)
{
  Midas3BitstreamTreeItem* bitstream = NULL;
  Midas3TreeItem*          node = const_cast<Midas3TreeItem *>(
      m_Model->midasTreeItem(this->indexAt(event->pos() ) ) );

  if( (bitstream = dynamic_cast<Midas3BitstreamTreeItem *>(node) ) != NULL )
    {
    emit bitstreamOpenRequest();
    }
  else
    {
    QTreeView::mouseDoubleClickEvent(event);
    }
}

void Midas3TreeViewClient::dragMoveEvent(QDragMoveEvent* event)
{
  selectionModel()->clearSelection();
  selectionModel()->select(this->indexAt(event->pos() ), QItemSelectionModel::Select | QItemSelectionModel::Rows);

  if( event->mimeData()->hasUrls() )
    {
    Midas3ItemTreeItem* item = NULL;
    Midas3TreeItem*     node = const_cast<Midas3TreeItem *>(
        m_Model->midasTreeItem(this->indexAt(event->pos() ) ) );

    if( (item = dynamic_cast<Midas3ItemTreeItem *>(node) ) != NULL )
      {
      event->acceptProposedAction();
      }
    else
      {
      event->setAccepted(false);
      }
    }
  else if( event->mimeData()->hasFormat("MIDAS/server_resource") )
    {
    event->setAccepted(!indexAt(event->pos() ).isValid() );
    }
}

void Midas3TreeViewClient::dropEvent(QDropEvent* event)
{
  if( !event || !event->mimeData() )
    {
    event->setAccepted(false);
    return;
    }
  const QMimeData* md = event->mimeData();
  if( md->hasUrls() )
    {
    Midas3ItemTreeItem* item = NULL;
    Midas3TreeItem*     node = const_cast<Midas3TreeItem *>(
        m_Model->midasTreeItem(this->indexAt(event->pos() ) ) );

    if( (item = dynamic_cast<Midas3ItemTreeItem *>(node) ) != NULL )
      {
      QStringList files;
      foreach(QUrl url, md->urls() )
        {
        QFileInfo info(url.toLocalFile() );

        if( info.exists() && info.isFile() )
          {
          files << url.toLocalFile();
          }
        }
      emit bitstreamsDropped(item, files);
      event->acceptProposedAction();
      }
    }
  else if( md->hasFormat("MIDAS/server_resource") )
    {
    QString                  data = QString(md->data("MIDAS/server_resource") );
    std::vector<std::string> tokens;
    midasUtils::Tokenize(data.toStdString(), tokens);

    int  type = atoi(tokens[0].c_str() );
    int  id   = atoi(tokens[1].c_str() );
    emit resourceDropped(type, id);
    event->acceptProposedAction();
    }
}

void Midas3TreeViewClient::expandAll()
{
  m_Model->expandAllResources();
  Midas3TreeView::expandAll();
}

void Midas3TreeViewClient::collapseAll()
{
  m_Model->clearExpandedList();
  Midas3TreeView::collapseAll();
}

void Midas3TreeViewClient::fetchItemData(Midas3TreeItem* item)
{
  Midas3FolderTreeItem*    folderTreeItem = NULL;
  Midas3ItemTreeItem*      itemTreeItem = NULL;
  Midas3BitstreamTreeItem* bitstreamTreeItem = NULL;

  if( (folderTreeItem = dynamic_cast<Midas3FolderTreeItem *>(item) ) != NULL )
    {
    m3ds::Folder mdsFolder;
    mdsFolder.SetObject(folderTreeItem->GetFolder() );
    mdsFolder.Fetch();

    emit midas3FolderTreeItemSelected(folderTreeItem);
    }
  else if( (itemTreeItem = dynamic_cast<Midas3ItemTreeItem *>(item) ) != NULL )
    {
    m3ds::Item mdsItem;
    mdsItem.SetObject(itemTreeItem->GetItem() );
    mdsItem.Fetch();

    emit midas3ItemTreeItemSelected(itemTreeItem);
    }
  else if( (bitstreamTreeItem = dynamic_cast<Midas3BitstreamTreeItem *>(item) ) != NULL )
    {
    m3ds::Bitstream mdsBitstream;
    mdsBitstream.SetObject(bitstreamTreeItem->GetBitstream() );
    mdsBitstream.Fetch();

    emit midas3BitstreamTreeItemSelected(bitstreamTreeItem);
    }
}

void Midas3TreeViewClient::addResource(mdo::Object* object)
{
  m_Model->addResource(object);
}

void Midas3TreeViewClient::updateResource(mdo::Object *)
{
}

void Midas3TreeViewClient::deleteResource(mdo::Object* object)
{
  m_Model->deleteResource(object);
}


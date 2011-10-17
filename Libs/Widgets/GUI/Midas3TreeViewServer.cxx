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
#include "Midas3TreeViewServer.h"

#include <QtGui>
#include <QItemSelection>
#include <QContextMenuEvent>
#include <QModelIndex>
#include <QPixmap>
#include <QMimeData>

#include "Midas3TreeItem.h"
#include "Midas3TreeModelServer.h"
#include "Midas3FolderTreeItem.h"
#include "Midas3ItemTreeItem.h"
#include "Midas3BitstreamTreeItem.h"
#include "ExpandTreeThread.h"

#include "mwsWebAPI.h"
#include "m3wsFolder.h"
#include "m3doFolder.h"
#include "m3wsItem.h"
#include "m3doItem.h"
#include "m3wsBitstream.h"
#include "m3doBitstream.h"
#include "mdoObject.h"

#include "midasUtils.h"
#include "midasSynchronizer.h"
#include <iostream>

Midas3TreeViewServer::Midas3TreeViewServer(QWidget* parent)
  : Midas3TreeView(parent)
{
  m_Model = new Midas3TreeModelServer;
  this->setModel(m_Model);

  this->setSelectionMode(QTreeView::SingleSelection);

  connect(this, SIGNAL(collapsed(const QModelIndex &) ),
          this->model(), SLOT(itemCollapsed(const QModelIndex &) ) );
  connect(this, SIGNAL(expanded(const QModelIndex &) ),
          this->model(), SLOT(itemExpanded(const QModelIndex &) ) );
  connect(m_Model, SIGNAL(expand(const QModelIndex &) ),
          this, SLOT(expand(const QModelIndex &) ) );
  connect(this->model(), SIGNAL(fetchedMore() ),
          this, SLOT(alertFetchedMore() ) );

  m_ExpandTreeThread = NULL;

  // define action to be triggered when tree item is selected
  QItemSelectionModel* itemSelectionModel = this->selectionModel();
  connect(itemSelectionModel,
          SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &) ),
          this, SLOT(updateSelection(const QItemSelection &, const QItemSelection &) ) );

  m_MimeType = "MIDAS/server_resource";
  m_AcceptMimeType = "MIDAS/client_resource";
}

Midas3TreeViewServer::~Midas3TreeViewServer()
{
  delete m_Model;
  delete m_ExpandTreeThread;
}

void Midas3TreeViewServer::AlertFetchedMore()
{
  emit FetchedMore();
}

void Midas3TreeViewServer::SelectByUuid(std::string uuid, bool select)
{
  if( m_ExpandTreeThread )
    {
    disconnect(m_ExpandTreeThread);
    }
  delete m_ExpandTreeThread;

  m_ExpandTreeThread = new ExpandTreeThread(this,
                                            reinterpret_cast<Midas3TreeModelServer *>(m_Model), uuid, select);

  connect(m_ExpandTreeThread, SIGNAL(finished() ),
          this, SLOT(ExpansionDone() ) );
  connect(m_ExpandTreeThread, SIGNAL(expand(const QModelIndex &) ),
          this, SLOT(expand(const QModelIndex &) ) );
  connect(m_ExpandTreeThread, SIGNAL(select(const QModelIndex &) ),
          this, SLOT(selectByIndex(const QModelIndex &) ) );

  m_ExpandTreeThread->start();
  emit StartedExpandingTree();
}

void Midas3TreeViewServer::ExpansionDone()
{
  emit FinishedExpandingTree();
  emit EnableActions(true);
}

void Midas3TreeViewServer::SelectByIndex(const QModelIndex& index)
{
  selectionModel()->select(index,
                           QItemSelectionModel::Select | QItemSelectionModel::Clear);
  this->scrollTo(index);
}

void Midas3TreeViewServer::FetchItemData(Midas3TreeItem* item)
{
  Midas3FolderTreeItem*    folderTreeItem = NULL;
  Midas3ItemTreeItem*      itemTreeItem = NULL;
  Midas3BitstreamTreeItem* bitstreamTreeItem = NULL;

  if( (folderTreeItem = dynamic_cast<Midas3FolderTreeItem *>(item) ) != NULL )
    {
    m3do::Folder* folder = folderTreeItem->GetFolder();
    m3ws::Folder  remote;
    remote.SetObject(folder);
    remote.SetAuthenticator(m_Synch->GetAuthenticator() );
    remote.Fetch();

    emit Midas3FolderTreeItemSelected(folderTreeItem);
    }
  else if( (itemTreeItem = dynamic_cast<Midas3ItemTreeItem *>(item) ) != NULL )
    {
    // TODO fetch?
    emit Midas3ItemTreeItemSelected(itemTreeItem);
    }
  else if( (bitstreamTreeItem = dynamic_cast<Midas3BitstreamTreeItem *>(item) ) != NULL )
    {
    emit Midas3BitstreamTreeItemSelected(bitstreamTreeItem);
    }
}

void Midas3TreeViewServer::dragMoveEvent(QDragMoveEvent* event)
{
  selectionModel()->clearSelection();
  selectionModel()->select(this->indexAt(event->pos() ), QItemSelectionModel::Select | QItemSelectionModel::Rows);

  if( event->mimeData()->hasFormat("MIDAS/client_resource") )
    {
    event->setAccepted(!indexAt(event->pos() ).isValid() );
    }
}

void Midas3TreeViewServer::dropEvent(QDropEvent* event)
{
  if( !event || !event->mimeData() )
    {
    event->setAccepted(false);
    return;
    }
  const QMimeData* md = event->mimeData();
  if( md->hasFormat("MIDAS/client_resource") )
    {
    QString                  data = QString(md->data("MIDAS/client_resource") );
    std::vector<std::string> tokens;
    midasUtils::Tokenize(data.toStdString(), tokens);

    int  type = atoi( tokens[0].c_str() );
    int  id   = atoi( tokens[1].c_str() );
    emit ResourceDropped(type, id);
    event->acceptProposedAction();
    }
}


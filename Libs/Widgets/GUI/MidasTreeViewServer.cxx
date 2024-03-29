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
#include "MidasTreeViewServer.h"

#include <QtGui>
#include <QItemSelection>
#include <QContextMenuEvent>
#include <QModelIndex>
#include <QPixmap>
#include <QMimeData>

#include "MidasTreeItem.h"
#include "MidasTreeModelServer.h"
#include "MidasCommunityTreeItem.h"
#include "MidasCollectionTreeItem.h"
#include "MidasItemTreeItem.h"
#include "MidasBitstreamTreeItem.h"
#include "ExpandTreeThread.h"

#include "mwsWebAPI.h"
#include "mwsCommunity.h"
#include "mwsBitstream.h"
#include "mwsCollection.h"
#include "mwsItem.h"
#include "mdoObject.h"
#include "mdoCommunity.h"
#include "mdoCollection.h"
#include "mdoItem.h"
#include "mdoBitstream.h"
#include "midasUtils.h"
#include "midasSynchronizer.h"
#include <iostream>

MidasTreeViewServer::MidasTreeViewServer(QWidget* parent)
  : MidasTreeView(parent)
{
  m_Model = new MidasTreeModelServer;
  this->setModel(m_Model);

  this->setSelectionMode( QTreeView::SingleSelection );

  connect(this, SIGNAL( collapsed(const QModelIndex &) ),
          this->model(), SLOT(itemCollapsed(const QModelIndex &) ) );
  connect(this, SIGNAL( expanded(const QModelIndex &) ),
          this->model(), SLOT(itemExpanded(const QModelIndex &) ) );
  connect(m_Model, SIGNAL( expand(const QModelIndex &) ),
          this, SLOT( expand(const QModelIndex &) ) );
  connect(this->model(), SIGNAL( fetchedMore() ),
          this, SLOT( alertFetchedMore() ) );

  m_ExpandTreeThread = NULL;

  // define action to be triggered when tree item is selected
  QItemSelectionModel * itemSelectionModel = this->selectionModel();
  connect(itemSelectionModel,
          SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &) ),
          this, SLOT(updateSelection(const QItemSelection &, const QItemSelection & ) ) );

  m_MimeType = "MIDAS/server_resource";
  m_AcceptMimeType = "MIDAS/client_resource";
}

MidasTreeViewServer::~MidasTreeViewServer()
{
  delete m_Model;
  delete m_ExpandTreeThread;
}

void MidasTreeViewServer::AlertFetchedMore()
{
  emit FetchedMore();
}

void MidasTreeViewServer::SelectByUuid(std::string uuid, bool select)
{
  if( m_ExpandTreeThread )
    {
    disconnect(m_ExpandTreeThread);
    }
  delete m_ExpandTreeThread;

  m_ExpandTreeThread = new ExpandTreeThread(this,
                                            reinterpret_cast<MidasTreeModelServer *>(m_Model), uuid, select);

  connect(m_ExpandTreeThread, SIGNAL( finished() ),
          this, SLOT(ExpansionDone() ) );
  connect(m_ExpandTreeThread, SIGNAL(expand(const QModelIndex &) ),
          this, SLOT(expand(const QModelIndex &) ) );
  connect(m_ExpandTreeThread, SIGNAL(select(const QModelIndex &) ),
          this, SLOT(SelectByIndex(const QModelIndex &) ) );

  m_ExpandTreeThread->start();
  emit StartedExpandingTree();
}

void MidasTreeViewServer::ExpansionDone()
{
  emit FinishedExpandingTree();
  emit EnableActions(true);
}

void MidasTreeViewServer::SelectByIndex(const QModelIndex& index)
{
  selectionModel()->select(index,
                           QItemSelectionModel::Select | QItemSelectionModel::Clear);
  this->scrollTo(index);
}

void MidasTreeViewServer::FetchItemData(MidasTreeItem* item)
{
  MidasCommunityTreeItem*  communityTreeItem = NULL;
  MidasCollectionTreeItem* collectionTreeItem = NULL;
  MidasItemTreeItem*       itemTreeItem = NULL;
  MidasBitstreamTreeItem*  bitstreamTreeItem = NULL;

  if( (communityTreeItem = dynamic_cast<MidasCommunityTreeItem *>(item) ) != NULL )
    {
    mdo::Community* community = communityTreeItem->GetCommunity();
    mws::Community  remote;
    remote.SetObject(community);
    remote.SetAuthenticator(m_Synch->GetAuthenticator() );
    remote.Fetch();

    emit MidasCommunityTreeItemSelected(communityTreeItem);
    }
  else if( (collectionTreeItem = dynamic_cast<MidasCollectionTreeItem *>(item) ) != NULL )
    {
    mdo::Collection* collection = collectionTreeItem->GetCollection();
    mws::Collection  remote;
    remote.SetObject(collection);
    remote.SetAuthenticator(m_Synch->GetAuthenticator() );
    remote.Fetch();

    emit MidasCollectionTreeItemSelected(collectionTreeItem);
    }
  else if( (itemTreeItem = dynamic_cast<MidasItemTreeItem *>(item) ) != NULL )
    {
    mdo::Item* item = itemTreeItem->GetItem();
    mws::Item  remote;
    remote.SetObject(item);
    remote.SetAuthenticator(m_Synch->GetAuthenticator() );
    remote.Fetch();

    emit MidasItemTreeItemSelected(itemTreeItem);
    }
  else if( (bitstreamTreeItem = dynamic_cast<MidasBitstreamTreeItem *>(item) ) != NULL )
    {
    mdo::Bitstream* bitstream = bitstreamTreeItem->GetBitstream();
    mws::Bitstream  remote;
    remote.SetObject(bitstream);
    remote.SetAuthenticator(m_Synch->GetAuthenticator() );
    remote.Fetch();

    emit MidasBitstreamTreeItemSelected(bitstreamTreeItem);
    }
}

void MidasTreeViewServer::dragMoveEvent(QDragMoveEvent* event)
{
  selectionModel()->clearSelection();
  selectionModel()->select(this->indexAt(event->pos() ), QItemSelectionModel::Select | QItemSelectionModel::Rows);

  if( event->mimeData()->hasFormat("MIDAS/client_resource") )
    {
    event->setAccepted(!indexAt(event->pos() ).isValid() );
    }
}

void MidasTreeViewServer::dropEvent( QDropEvent * event )
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


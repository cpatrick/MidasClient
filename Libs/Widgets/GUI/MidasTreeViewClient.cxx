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
#include "MidasTreeViewClient.h"

#include <QtGui>
#include <QItemSelection>
#include <QContextMenuEvent>
#include <QModelIndex>

#include "MidasTreeItem.h"
#include "MidasTreeModelClient.h"
#include "MidasCommunityTreeItem.h"
#include "MidasCollectionTreeItem.h"
#include "MidasItemTreeItem.h"
#include "MidasBitstreamTreeItem.h"
#include "mdoCommunity.h"
#include "mdsCommunity.h"
#include "mdsCollection.h"
#include "mdoCollection.h"
#include "mdsItem.h"
#include "mdoItem.h"
#include "mdsBitstream.h"
#include "mdoBitstream.h"
#include "mdoObject.h"
#include "midasLog.h"
#include <iostream>

MidasTreeViewClient::MidasTreeViewClient(QWidget* parent)
  : MidasTreeView(parent)
{
  m_Model = new MidasTreeModelClient;

  this->setModel(m_Model);
  this->setAcceptDrops(true);
  this->setSelectionMode(QTreeView::SingleSelection);

  connect(this, SIGNAL( collapsed(const QModelIndex &) ),
          this->model(), SLOT(itemCollapsed(const QModelIndex &) ) );
  connect(this, SIGNAL( expanded(const QModelIndex &) ),
          this->model(), SLOT(itemExpanded(const QModelIndex &) ) );
  connect(m_Model, SIGNAL( expand(const QModelIndex &) ),
          this, SLOT( expand(const QModelIndex &) ) );

  // define action to be triggered when tree item is selected
  QItemSelectionModel* itemSelectionModel = this->selectionModel();
  connect(itemSelectionModel,
          SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &) ),
          this, SLOT(updateSelection(const QItemSelection &, const QItemSelection & ) ) );

  m_MimeType = "MIDAS/client_resource";
  m_AcceptMimeType = "MIDAS/server_resource";
}

MidasTreeViewClient::~MidasTreeViewClient()
{
  delete m_Model;
}

void MidasTreeViewClient::mouseDoubleClickEvent(QMouseEvent* event)
{
  MidasBitstreamTreeItem* bitstream = NULL;
  MidasTreeItem*          node = const_cast<MidasTreeItem *>(
      m_Model->GetMidasTreeItem(this->indexAt(event->pos() ) ) );

  if( (bitstream = dynamic_cast<MidasBitstreamTreeItem *>(node) ) != NULL )
    {
    emit BitstreamOpenRequest();
    }
  else
    {
    QTreeView::mouseDoubleClickEvent(event);
    }
}

void MidasTreeViewClient::dragMoveEvent(QDragMoveEvent* event)
{
  selectionModel()->clearSelection();
  selectionModel()->select(this->indexAt(event->pos() ), QItemSelectionModel::Select | QItemSelectionModel::Rows);

  if( event->mimeData()->hasUrls() )
    {
    MidasItemTreeItem* item = NULL;
    MidasTreeItem*     node = const_cast<MidasTreeItem *>(
        m_Model->GetMidasTreeItem(this->indexAt(event->pos() ) ) );

    if( (item = dynamic_cast<MidasItemTreeItem *>(node) ) != NULL )
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

void MidasTreeViewClient::dropEvent(QDropEvent* event)
{
  if( !event || !event->mimeData() )
    {
    event->setAccepted(false);
    return;
    }
  const QMimeData* md = event->mimeData();
  if( md->hasUrls() )
    {
    MidasItemTreeItem* item = NULL;
    MidasTreeItem*     node = const_cast<MidasTreeItem *>(
        m_Model->GetMidasTreeItem(this->indexAt(event->pos() ) ) );

    if( (item = dynamic_cast<MidasItemTreeItem *>(node) ) != NULL )
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
      emit BitstreamsDropped( item, files );
      event->acceptProposedAction();
      }
    }
  else if( md->hasFormat("MIDAS/server_resource") )
    {
    QString                  data = QString(md->data("MIDAS/server_resource") );
    std::vector<std::string> tokens;
    midasUtils::Tokenize(data.toStdString(), tokens);

    int  type = atoi( tokens[0].c_str() );
    int  id   = atoi( tokens[1].c_str() );
    emit ResourceDropped(type, id);
    event->acceptProposedAction();
    }
}

void MidasTreeViewClient::expandAll()
{
  m_Model->ExpandAllResources();
  MidasTreeView::expandAll();
}

void MidasTreeViewClient::collapseAll()
{
  m_Model->ClearExpandedList();
  MidasTreeView::collapseAll();
}

void MidasTreeViewClient::FetchItemData(MidasTreeItem* item)
{
  MidasCommunityTreeItem*  communityTreeItem = NULL;
  MidasCollectionTreeItem* collectionTreeItem = NULL;
  MidasItemTreeItem*       itemTreeItem = NULL;
  MidasBitstreamTreeItem*  bitstreamTreeItem = NULL;

  if( (communityTreeItem = dynamic_cast<MidasCommunityTreeItem *>(item) ) != NULL )
    {
    mds::Community mdsComm;
    mdsComm.SetObject(communityTreeItem->GetCommunity() );
    mdsComm.Fetch();

    emit MidasCommunityTreeItemSelected(communityTreeItem);
    }
  else if( (collectionTreeItem = dynamic_cast<MidasCollectionTreeItem *>(item) ) != NULL )
    {
    mds::Collection mdsColl;
    mdsColl.SetObject(collectionTreeItem->GetCollection() );
    mdsColl.Fetch();

    emit MidasCollectionTreeItemSelected(collectionTreeItem);
    }
  else if( (itemTreeItem = dynamic_cast<MidasItemTreeItem *>(item) ) != NULL )
    {
    mds::Item mdsItem;
    mdsItem.SetObject(itemTreeItem->GetItem() );
    mdsItem.Fetch();

    emit MidasItemTreeItemSelected(itemTreeItem);
    }
  else if( (bitstreamTreeItem = dynamic_cast<MidasBitstreamTreeItem *>(item) ) != NULL )
    {
    mds::Bitstream mdsBitstream;
    mdsBitstream.SetObject(bitstreamTreeItem->GetBitstream() );
    mdsBitstream.Fetch();

    emit MidasBitstreamTreeItemSelected(bitstreamTreeItem);
    }
}

void MidasTreeViewClient::AddResource(mdo::Object* object)
{
  this->m_Model->AddResource(object);
}

void MidasTreeViewClient::UpdateResource(mdo::Object *)
{
}

void MidasTreeViewClient::DeleteResource(mdo::Object* object)
{
  this->m_Model->DeleteResource(object);
}


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
#include "MidasTreeView.h"

#include "MidasTreeItem.h"
#include "MidasCommunityTreeItem.h"
#include "MidasCollectionTreeItem.h"
#include "MidasItemTreeItem.h"
#include "MidasBitstreamTreeItem.h"
#include "MidasTreeModel.h"
#include "midasSynchronizer.h"

#include <QModelIndex>
#include <QtGui>

MidasTreeView::MidasTreeView(QWidget* parent)
: MidasTreeViewBase(parent), m_Synch(NULL)
{
}

MidasTreeView::~MidasTreeView()
{
}

void MidasTreeView::SetSynchronizer(midasSynchronizer* synch)
{
  m_Synch = synch;
  m_Model->SetSynchronizer(synch);
}

void MidasTreeView::UpdateSelection(const QItemSelection & selected,
                                    const QItemSelection & deselected)
{
  (void)deselected;
  QModelIndexList items = selected.indexes();
  if( items.size() > 0 && items.first().isValid() )
    {
    MidasTreeItem* item = const_cast<MidasTreeItem *>(
      m_Model->GetMidasTreeItem(items.first() ) );
    this->FetchItemData(item);
    emit MidasTreeItemSelected(item);
    }
  else
    {
    emit MidasNoTreeItemSelected();
    }
  emit FetchedSelectedResource();
}

bool MidasTreeView::IsModelIndexSelected() const
{
  QItemSelectionModel* selectionModel = this->selectionModel();

  return selectionModel->selectedIndexes().size() > 0;
}

const QModelIndex MidasTreeView::GetSelectedModelIndex() const
{
  QModelIndexList selectedIndexes = selectionModel()->selectedIndexes();

  if( selectedIndexes.size() )
    {
    return selectedIndexes.first();
    }
  else
    {
    return QModelIndex();
    }
}

const MidasTreeItem* MidasTreeView::GetSelectedMidasTreeItem() const
{
  return m_Model->GetMidasTreeItem(this->GetSelectedModelIndex() );
}

void MidasTreeView::Update()
{
  this->Clear();
  this->Initialize();
}

void MidasTreeView::Clear()
{
  disconnect(this);
  m_Model->Clear();
  this->reset();
}

void MidasTreeView::Initialize()
{
  m_Model->Populate();
  m_Model->RestoreExpandedState();
}

void MidasTreeView::DecorateByUuid(std::string uuid)
{
  m_Model->DecorateByUuid(uuid);
}

void MidasTreeView::contextMenuEvent(QContextMenuEvent* event)
{
  emit MidasTreeViewContextMenu(event);

  event->accept();
}

void MidasTreeView::dragEnterEvent(QDragEnterEvent* event)
{
  if( event && event->mimeData() )
    {
    const QMimeData* md = event->mimeData();
    if( md->hasUrls() || md->hasFormat(m_AcceptMimeType) )
      {
      event->acceptProposedAction();
      }
    }
}

void MidasTreeView::dragLeaveEvent(QDragLeaveEvent *)
{
}

void MidasTreeView::dragMoveEvent(QDragMoveEvent *)
{
}

void MidasTreeView::dropEvent(QDropEvent *)
{
}

void MidasTreeView::mouseDoubleClickEvent(QMouseEvent *)
{
}

void MidasTreeView::mousePressEvent(QMouseEvent* event)
{
  if( event->button() == Qt::LeftButton )
    {
    m_DragStart = event->pos();
    }
  QTreeView::mousePressEvent(event);
}

void MidasTreeView::mouseMoveEvent(QMouseEvent* event)
{
  if( !(event->buttons() & Qt::LeftButton) )
    {
    return;
    }
  if( (event->pos() - m_DragStart).manhattanLength()
      < QApplication::startDragDistance() )
    {
    return;
    }

  QModelIndex index = this->indexAt(m_DragStart);
  if( !index.isValid() )
    {
    event->setAccepted(false);
    return;
    }

  MidasTreeItem* resource =
    const_cast<MidasTreeItem *>(m_Model->GetMidasTreeItem(index) );

  QDrag*            drag = new QDrag(this);
  QMimeData*        mimeData = new QMimeData;
  std::stringstream data;
  data << resource->GetType() << " " << resource->GetId();

  mimeData->setData(m_MimeType, QString(data.str().c_str() ).toAscii() );
  drag->setPixmap(resource->GetDecoration() );
  drag->setMimeData(mimeData);
  drag->start();
}

void MidasTreeView::AddResource(mdo::Object *)
{
}

void MidasTreeView::UpdateResource(mdo::Object *)
{
}

void MidasTreeView::DeleteResource(mdo::Object *)
{
}


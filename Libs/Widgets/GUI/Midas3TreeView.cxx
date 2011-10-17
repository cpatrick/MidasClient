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
#include "Midas3TreeView.h"

#include "Midas3TreeItem.h"
#include "Midas3FolderTreeItem.h"
#include "Midas3TreeModel.h"
#include "midasSynchronizer.h"

#include <QModelIndex>
#include <QtGui>

Midas3TreeView::Midas3TreeView(QWidget* parent)
  : MidasTreeViewBase(parent), m_Synch(NULL)
{
}

Midas3TreeView::~Midas3TreeView()
{
}

void Midas3TreeView::SetSynchronizer(midasSynchronizer* synch)
{
  m_Synch = synch;
  m_Model->SetSynchronizer(synch);
}

void Midas3TreeView::updateSelection(const QItemSelection & selected,
                                     const QItemSelection & deselected)
{
  (void)deselected;
  QModelIndexList items = selected.indexes();
  if( items.size() > 0 && items.first().isValid() )
    {
    Midas3TreeItem* item = const_cast<Midas3TreeItem *>(m_Model->midasTreeItem(items.first() ) );
    fetchItemData(item);
    emit midasTreeItemSelected(item);
    }
  else
    {
    emit midasNoTreeItemSelected();
    }
  emit fetchedSelectedResource();
}

bool Midas3TreeView::isModelIndexSelected() const
{
  QItemSelectionModel* selectionModel = this->selectionModel();

  return selectionModel->selectedIndexes().size() > 0;
}

const QModelIndex Midas3TreeView::getSelectedModelIndex() const
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

const Midas3TreeItem * Midas3TreeView::getSelectedMidasTreeItem() const
{
  return m_Model->midasTreeItem(this->getSelectedModelIndex() );
}

void Midas3TreeView::Update()
{
  this->Clear();
  this->Initialize();
}

void Midas3TreeView::Clear()
{
  disconnect(this);
  this->m_Model->clear();
  this->reset();
}

void Midas3TreeView::Initialize()
{
  m_Model->Populate();
  m_Model->restoreExpandedState();
}

void Midas3TreeView::decorateByUuid(std::string uuid)
{
  m_Model->decorateByUuid(uuid);
}

void Midas3TreeView::contextMenuEvent(QContextMenuEvent* event)
{
  emit midasTreeViewContextMenu(event);

  event->accept();
}

void Midas3TreeView::dragEnterEvent(QDragEnterEvent* event)
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

void Midas3TreeView::dragLeaveEvent(QDragLeaveEvent *)
{
}

void Midas3TreeView::dragMoveEvent(QDragMoveEvent *)
{
}

void Midas3TreeView::dropEvent(QDropEvent *)
{
}

void Midas3TreeView::mouseDoubleClickEvent(QMouseEvent *)
{
}

void Midas3TreeView::mousePressEvent(QMouseEvent* event)
{
  if( event->button() == Qt::LeftButton )
    {
    m_DragStart = event->pos();
    }
  QTreeView::mousePressEvent(event);
}

void Midas3TreeView::mouseMoveEvent(QMouseEvent* event)
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

  Midas3TreeItem* resource =
    const_cast<Midas3TreeItem *>(m_Model->midasTreeItem(index) );

  QDrag*            drag = new QDrag(this);
  QMimeData*        mimeData = new QMimeData;
  std::stringstream data;
  data << resource->GetType() << " " << resource->GetId();

  mimeData->setData(m_MimeType, QString(data.str().c_str() ).toAscii() );
  drag->setPixmap(resource->GetDecoration() );
  drag->setMimeData(mimeData);
  drag->start();
}

void Midas3TreeView::addResource(mdo::Object *)
{
}

void Midas3TreeView::updateResource(mdo::Object *)
{
}

void Midas3TreeView::deleteResource(mdo::Object *)
{
}


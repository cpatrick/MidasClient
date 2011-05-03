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
: QTreeView(parent), m_Synch(NULL)
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

void MidasTreeView::updateSelection(const QItemSelection &selected,
                                    const QItemSelection &deselected)
{
  QModelIndex index;
  QModelIndexList items = selected.indexes();
  if (items.size() > 0)
    {
    MidasTreeItem* item = const_cast<MidasTreeItem*>(m_Model->midasTreeItem(items.first()));
    emit midasTreeItemSelected(item);
    
    fetchItemData(item);
    }
  else 
    {
    emit midasNoTreeItemSelected(); 
    }
}

bool MidasTreeView::isModelIndexSelected() const
{
  QItemSelectionModel* selectionModel = this->selectionModel();
  return (selectionModel->selectedIndexes().size() > 0);
}

const QModelIndex MidasTreeView::getSelectedModelIndex() const
{
  QModelIndexList selectedIndexes = selectionModel()->selectedIndexes();
  if(selectedIndexes.size())
    {
    return selectedIndexes.first();
    }
  else
    {
    return QModelIndex();
    }
}

const MidasTreeItem* MidasTreeView::getSelectedMidasTreeItem() const
{
  return m_Model->midasTreeItem(this->getSelectedModelIndex());
}

void MidasTreeView::Update()
{
  this->Clear();
  this->Initialize();
}

void MidasTreeView::Clear()
{
  this->m_Model->clear();
  disconnect(this);
  this->reset();
}

void MidasTreeView::Initialize()
{
  m_Model->Populate();
  m_Model->restoreExpandedState();
}


void MidasTreeView::decorateByUuid(std::string uuid)
{
  m_Model->decorateByUuid(uuid);
}

void MidasTreeView::contextMenuEvent(QContextMenuEvent * e)
{
  emit midasTreeViewContextMenu( e );
}

void MidasTreeView::dragEnterEvent(QDragEnterEvent* event)
{
  if (event && event->mimeData())
    {
    const QMimeData* md = event->mimeData();
    if ( md->hasUrls() || md->hasFormat("MIDAS/resource"))
      {
      event->acceptProposedAction();
      }
    }
}

void MidasTreeView::dragLeaveEvent(QDragLeaveEvent* event)
{
}

void MidasTreeView::dragMoveEvent(QDragMoveEvent* event)
{
}

void MidasTreeView::dropEvent(QDropEvent* event)
{
}

void MidasTreeView::mouseDoubleClickEvent(QMouseEvent* event)
{
}

void MidasTreeView::mousePressEvent(QMouseEvent* event)
{
  if(event->button() == Qt::LeftButton)
    {
    m_DragStart = event->pos();
    }
  QTreeView::mousePressEvent(event);
}

void MidasTreeView::mouseMoveEvent(QMouseEvent* event)
{
  if (!(event->buttons() & Qt::LeftButton))
    {
    return;
    }
  if ((event->pos() - m_DragStart).manhattanLength()
       < QApplication::startDragDistance())
    {
    return;
    }

  QModelIndex index = this->indexAt(m_DragStart);
  if(!index.isValid())
    {
    event->setAccepted(false);
    return;
    }

  MidasTreeItem* resource =
    const_cast<MidasTreeItem*>(m_Model->midasTreeItem(index));

  QDrag* drag = new QDrag(this);
  QMimeData* mimeData = new QMimeData;
  std::stringstream data;
  data << resource->getType() << " " << resource->getId();

  mimeData->setData("MIDAS/resource", QString(data.str().c_str()).toAscii());
  drag->setPixmap(resource->getDecoration());
  drag->setMimeData(mimeData);
  Qt::DropAction dropAction = drag->start();
}

void MidasTreeView::addResource(mdo::Object*)
{
}

void MidasTreeView::updateResource(mdo::Object*)
{
}

void MidasTreeView::deleteResource(mdo::Object*)
{
}

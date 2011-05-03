#include "MidasTreeView.h"

#include "MidasTreeItem.h"
#include "MidasCommunityTreeItem.h"
#include "MidasCollectionTreeItem.h"
#include "MidasItemTreeItem.h"
#include "MidasBitstreamTreeItem.h"
#include "MidasTreeModel.h"
#include "midasSynchronizer.h"

#include <QModelIndex>

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
  QItemSelectionModel * selectionModel = this->selectionModel();
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

void MidasTreeView::contextMenuEvent(QContextMenuEvent * e)
{
  emit midasTreeViewContextMenu( e );
}

void MidasTreeView::decorateByUuid(std::string uuid)
{
  m_Model->decorateByUuid(uuid);
}

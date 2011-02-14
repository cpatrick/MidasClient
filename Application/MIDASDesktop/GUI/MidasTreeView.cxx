#include "MidasTreeView.h"

#include "MidasTreeItem.h"
#include "MidasCommunityTreeItem.h"
#include "MidasCollectionTreeItem.h"
#include "MidasItemTreeItem.h"
#include "MidasBitstreamTreeItem.h"
#include "MidasTreeModel.h"
#include "MIDASDesktopUI.h"

#include <QModelIndex>

MidasTreeView::MidasTreeView(QWidget* parent) : QTreeView(parent)
{
}

void MidasTreeView::SetParentUI(MIDASDesktopUI* parent)
{
  m_Parent = parent;
  m_Model->SetParentUI(parent);
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
  return selectedIndexes.first();
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

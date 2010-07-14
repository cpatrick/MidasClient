#include "MidasTreeView.h"

#include "MidasTreeItem.h"
#include "MidasCommunityTreeItem.h"
#include "MidasCollectionTreeItem.h"
#include "MidasItemTreeItem.h"
#include "MidasBitstreamTreeItem.h"
#include "MidasTreeModel.h"

#include <QModelIndex>

void MidasTreeView::updateSelection(const QItemSelection &selected,
                                    const QItemSelection &deselected)
{
  QModelIndex index;
  QModelIndexList items = selected.indexes();
  if (items.size() > 0)
    {
    MidasTreeItem* item = const_cast<MidasTreeItem*>(m_Model->midasTreeItem(items.first()));
    emit midasTreeItemSelected(item);

    MidasCommunityTreeItem * communityTreeItem = NULL;
    MidasCollectionTreeItem * collectionTreeItem = NULL;
    MidasItemTreeItem * itemTreeItem = NULL;
    MidasBitstreamTreeItem * bitstreamTreeItem = NULL;

    if ((communityTreeItem = dynamic_cast<MidasCommunityTreeItem*>(item)) != NULL)
      {
      emit midasCommunityTreeItemSelected(communityTreeItem);
      }
    else if ((collectionTreeItem = dynamic_cast<MidasCollectionTreeItem*>(item)) != NULL)
      {
      emit midasCollectionTreeItemSelected(collectionTreeItem);
      }
    else if ((itemTreeItem = dynamic_cast<MidasItemTreeItem*>(item)) != NULL)
      {
      emit midasItemTreeItemSelected(itemTreeItem);
      }
    else if ((bitstreamTreeItem = dynamic_cast<MidasBitstreamTreeItem*>(item)) != NULL)
      {
      emit midasBitstreamTreeItemSelected(bitstreamTreeItem);
      }
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

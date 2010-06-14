#include "MidasTreeView.h"

#include <QtGui>
#include <QItemSelection>
#include <QContextMenuEvent>
#include <QModelIndex>

#include "MIDASDesktopUI.h"
#include "MidasTreeItem.h"
#include "MidasTreeModel.h"
#include "MidasCommunityTreeItem.h"
#include "MidasCollectionTreeItem.h"
#include "MidasItemTreeItem.h"
#include "MidasBitstreamTreeItem.h"
#include "Logger.h"
#include "Utils.h"
#include "MidasClientGlobal.h"
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
#include <iostream>

/** Constructor */
MidasTreeView::MidasTreeView(QWidget * parent):QTreeView(parent)
{
  // The tree model
  m_Model = new MidasTreeModel;
  this->setModel(m_Model);

  m_WebAPI = NULL;

  this->setSelectionMode( QTreeView::SingleSelection );

  connect (this, SIGNAL( collapsed(const QModelIndex&)),
     this->model(), SLOT(itemCollapsed(const QModelIndex&)) );
  connect (this, SIGNAL( expanded(const QModelIndex&)),
     this->model(), SLOT(itemExpanded (const QModelIndex&)) );
  connect (this->model(), SIGNAL( fetchedMore() ),
     this, SLOT( alertFetchedMore() ) );

  m_ExpandTreeThread = NULL;

  // define action to be triggered when tree item is selected
  QItemSelectionModel * itemSelectionModel = this->selectionModel(); 
  connect(itemSelectionModel,
     SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
     this, SLOT(updateSelection(const QItemSelection&, const QItemSelection& )));
}

/** Destructor */
MidasTreeView::~MidasTreeView()
{
  delete m_Model;
  delete m_ExpandTreeThread;
}

/** Set the web API */
void MidasTreeView::SetWebAPI(mws::WebAPI* api)
{
  m_Model->SetWebAPI(api);
  m_WebAPI = api;
}

/** Initialize the tree */
bool MidasTreeView::Initialize()
{
  if(!m_WebAPI)
    {
    std::cerr << "WebAPI not set" << std::endl;
    return false;
    } 
  
  m_Model->Populate();
  return true;
}

void MidasTreeView::Update()
{
  this->Clear();
  this->Initialize();
}

/** Clear */
void MidasTreeView::Clear()
{
  this->m_Model->clear(QModelIndex());
  disconnect(this);
  this->reset();
}

bool MidasTreeView::isModelIndexSelected() const
{
  QItemSelectionModel * selectionModel = this->selectionModel(); 
  assert(selectionModel != NULL); 
  return (selectionModel->selectedIndexes().size() > 0); 
}

const QModelIndex MidasTreeView::getSelectedModelIndex() const
{
  QItemSelectionModel * selectionModel = this->selectionModel(); 
  assert(selectionModel != NULL); 
  QModelIndexList selectedIndexes = selectionModel->selectedIndexes(); 
  assert(selectedIndexes.size() == 1); 
  return selectedIndexes.first();
}

const MidasTreeItem * MidasTreeView::getSelectedMidasTreeItem() const
{
  return reinterpret_cast<MidasTreeModel*>(
    this->model())->midasTreeItem(this->getSelectedModelIndex());
}

void MidasTreeView::updateSelection(const QItemSelection &selected,
                                    const QItemSelection &deselected)
{
  assert(this->selectionMode() == QTreeView::SingleSelection); 

  QModelIndex index;
  QModelIndexList items = selected.indexes();
  if (items.size() > 0)
    {
    MidasTreeItem * item = const_cast<MidasTreeItem*>( m_Model->midasTreeItem(items.first()) ); 
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

void MidasTreeView::contextMenuEvent(QContextMenuEvent* e)
{
  emit midasTreeViewContextMenu(e);
}

void MidasTreeView::decorateByUuid(std::string uuid)
{
  this->m_Model->decorateByUuid(uuid);
}

void MidasTreeView::alertFetchedMore()
{
  emit fetchedMore();
}

void MidasTreeView::selectByObject(mdo::Object* object)
{
  //m_Mutex.lock();
  if(m_ExpandTreeThread)
    {
    disconnect(m_ExpandTreeThread);
    }
  delete m_ExpandTreeThread;

  m_ExpandTreeThread = new ExpandTreeThread;
  m_ExpandTreeThread->SetParentUI(this);
  m_ExpandTreeThread->SetParentModel(m_Model);
  m_ExpandTreeThread->SetObject(object);

  connect(m_ExpandTreeThread, SIGNAL(threadComplete()),
    this, SLOT(expansionDone()));
  connect(m_ExpandTreeThread, SIGNAL(expand(const QModelIndex&)),
    this, SLOT(expand(const QModelIndex&)));
  connect(m_ExpandTreeThread, SIGNAL(select(const QModelIndex&)),
    this, SLOT(selectByIndex(const QModelIndex&)));

  m_ExpandTreeThread->start();
  emit startedExpandingTree();
}

void MidasTreeView::expansionDone()
{
  emit finishedExpandingTree();
  //m_Mutex.unlock();
}

void MidasTreeView::selectByIndex(const QModelIndex& index)
{
  selectionModel()->select(index,
    QItemSelectionModel::Select | QItemSelectionModel::Clear);
  scrollTo(index);
}

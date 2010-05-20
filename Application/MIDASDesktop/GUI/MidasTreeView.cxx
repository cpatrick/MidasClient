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
#include "Logger.h"
#include "Utils.h"
#include "MidasClientGlobal.h"
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
  return NULL;
  //return this->model()->midasTreeItem(this->getSelectedModelIndex()); 
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

MidasItemTreeItem * MidasTreeView::selectedMidasItemTreeItem()
{
  MidasItemTreeItem * itemTreeItem = NULL; 
  QModelIndexList selected = this->selectedIndexes(); 
  if(selected.size() > 0)
    {
//    MidasTreeItem * item = const_cast<MidasTreeItem*>( this->model()->midasTreeItem(selected.first()) ); 
//    itemTreeItem = dynamic_cast<MidasItemTreeItem*>(item); 
    }
  return itemTreeItem; 
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
  std::vector<std::string> path; //path of uuids to the root

  mdo::Community* comm = NULL;
  mdo::Collection* coll = NULL;
  mdo::Item* item = NULL;
  mdo::Bitstream* bitstream = NULL;

  while(true)
    {
    if((comm = dynamic_cast<mdo::Community*>(object)) != NULL)
      {
      mws::Community remote;
      remote.SetWebAPI(mws::WebAPI::Instance());
      remote.SetObject(comm);
      if(path.size() == 0)
        {
        remote.Fetch();
        }
      remote.FetchParent();
      object = comm->GetParentCommunity();
      path.push_back(comm->GetUuid());
      }
    else if((coll = dynamic_cast<mdo::Collection*>(object)) != NULL)
      {
      mws::Collection remote;
      remote.SetWebAPI(mws::WebAPI::Instance());
      remote.SetObject(coll);
      if(path.size() == 0)
        {
        remote.Fetch();
        }
      remote.FetchParent();
      object = coll->GetParentCommunity();
      path.push_back(coll->GetUuid());
      }
    else if((item = dynamic_cast<mdo::Item*>(object)) != NULL)
      {
      mws::Item remote;
      remote.SetWebAPI(mws::WebAPI::Instance());
      remote.SetObject(item);
      if(path.size() == 0)
        {
        remote.Fetch();
        }
      remote.FetchParent();
      object = item->GetParentCollection();
      path.push_back(item->GetUuid());
      }
    else if((bitstream = dynamic_cast<mdo::Bitstream*>(object)) != NULL)
      {
      mws::Bitstream remote;
      remote.SetWebAPI(mws::WebAPI::Instance());
      remote.SetObject(bitstream);
      if(path.size() == 0)
        {
        remote.Fetch();
        }
      remote.FetchParent();
      object = bitstream->GetParentItem();
      path.push_back(bitstream->GetUuid());
      }
    if(object == NULL)
      {
      break;
      }
    }

  for(std::vector<std::string>::reverse_iterator i = path.rbegin();
      i != path.rend(); ++i) //TODO rend +/- 1?
    {
    expand(m_Model->getIndexByUuid(*i));
    }
  selectionModel()->select(m_Model->getIndexByUuid(*(path.begin())),
    QItemSelectionModel::Select | QItemSelectionModel::Clear);
  
}

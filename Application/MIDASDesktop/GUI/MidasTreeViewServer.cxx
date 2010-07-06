#include "MidasTreeViewServer.h"

#include <QtGui>
#include <QItemSelection>
#include <QContextMenuEvent>
#include <QModelIndex>
#include <QPixmap>
#include <QMimeData>

#include "MIDASDesktopUI.h"
#include "MidasTreeItem.h"
#include "MidasTreeModelServer.h"
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
MidasTreeViewServer::MidasTreeViewServer(QWidget * parent):MidasTreeView(parent)
{
  // The tree model
  m_Model = new MidasTreeModelServer;
  this->setModel(m_Model);

  m_WebAPI = NULL;

  this->setSelectionMode( QTreeView::SingleSelection );

  connect (this, SIGNAL( collapsed(const QModelIndex&)),
    this->model(), SLOT(itemCollapsed(const QModelIndex&)) );
  connect (this, SIGNAL( expanded(const QModelIndex&)),
    this->model(), SLOT(itemExpanded (const QModelIndex&)) );
  connect (m_Model, SIGNAL( expand(const QModelIndex&) ),
    this, SLOT( expand(const QModelIndex&) ) );
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
MidasTreeViewServer::~MidasTreeViewServer()
{
  delete m_Model;
  delete m_ExpandTreeThread;
}

/** Set the web API */
void MidasTreeViewServer::SetWebAPI(mws::WebAPI* api)
{
  m_Model->SetWebAPI(api);
  m_WebAPI = api;
}

/** Initialize the tree */
bool MidasTreeViewServer::Initialize()
{
  if(!m_WebAPI)
    {
    std::cerr << "WebAPI not set" << std::endl;
    return false;
    } 
  
  m_Model->Populate();
  m_Model->restoreExpandedState();
  return true;
}

void MidasTreeViewServer::Update()
{
  this->Clear();
  this->Initialize();
}

/** Clear */
void MidasTreeViewServer::Clear()
{
  this->m_Model->clear();
  disconnect(this);
  this->reset();
}

bool MidasTreeViewServer::isModelIndexSelected() const
{
  QItemSelectionModel * selectionModel = this->selectionModel(); 
  assert(selectionModel != NULL); 
  return (selectionModel->selectedIndexes().size() > 0); 
}

const QModelIndex MidasTreeViewServer::getSelectedModelIndex() const
{
  QItemSelectionModel * selectionModel = this->selectionModel(); 
  assert(selectionModel != NULL); 
  QModelIndexList selectedIndexes = selectionModel->selectedIndexes(); 
  assert(selectedIndexes.size() == 1); 
  return selectedIndexes.first();
}

const MidasTreeItem * MidasTreeViewServer::getSelectedMidasTreeItem() const
{
  return reinterpret_cast<MidasTreeModelServer*>(
    this->model())->midasTreeItem(this->getSelectedModelIndex());
}

void MidasTreeViewServer::updateSelection(const QItemSelection &selected,
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

void MidasTreeViewServer::contextMenuEvent(QContextMenuEvent* e)
{
  emit midasTreeViewServerContextMenu(e);
}

void MidasTreeViewServer::decorateByUuid(std::string uuid)
{
  this->m_Model->decorateByUuid(uuid);
}

void MidasTreeViewServer::alertFetchedMore()
{
  emit fetchedMore();
}

void MidasTreeViewServer::selectByObject(mdo::Object* object)
{
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

void MidasTreeViewServer::expansionDone()
{
  emit finishedExpandingTree();
}

void MidasTreeViewServer::selectByIndex(const QModelIndex& index)
{
  selectionModel()->select(index,
    QItemSelectionModel::Select | QItemSelectionModel::Clear);
  scrollTo(index);
}

void MidasTreeViewServer::mousePressEvent(QMouseEvent* event)
{
  if(event->button() == Qt::LeftButton)
    {
    m_DragStart = event->pos();
    }
  QTreeView::mousePressEvent(event);
}

void MidasTreeViewServer::mouseMoveEvent(QMouseEvent* event)
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

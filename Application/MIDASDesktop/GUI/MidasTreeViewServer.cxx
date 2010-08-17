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

MidasTreeViewServer::MidasTreeViewServer(QWidget * parent):MidasTreeView(parent)
{
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

MidasTreeViewServer::~MidasTreeViewServer()
{
  delete m_Model;
  delete m_ExpandTreeThread;
}

void MidasTreeViewServer::SetWebAPI(mws::WebAPI* api)
{
  reinterpret_cast<MidasTreeModelServer*>(m_Model)->SetWebAPI(api);
  m_WebAPI = api;
}

void MidasTreeViewServer::alertFetchedMore()
{
  emit fetchedMore();
}

void MidasTreeViewServer::selectByUuid(std::string uuid)
{
  if(m_ExpandTreeThread)
    {
    disconnect(m_ExpandTreeThread);
    }
  delete m_ExpandTreeThread;

  m_ExpandTreeThread = new ExpandTreeThread(this, 
    reinterpret_cast<MidasTreeModelServer*>(m_Model), uuid);

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

void MidasTreeViewServer::fetchItemData(MidasTreeItem* item)
{
  MidasCommunityTreeItem* communityTreeItem = NULL;
  MidasCollectionTreeItem* collectionTreeItem = NULL;
  MidasItemTreeItem* itemTreeItem = NULL;
  MidasBitstreamTreeItem* bitstreamTreeItem = NULL;

  if((communityTreeItem = dynamic_cast<MidasCommunityTreeItem*>(item)) != NULL)
    {
    mdo::Community* community = communityTreeItem->getCommunity();
    mws::Community remote;
    remote.SetWebAPI(mws::WebAPI::Instance());
    remote.SetObject(community);
    remote.Fetch();

    emit midasCommunityTreeItemSelected(communityTreeItem);
    }
  else if((collectionTreeItem = dynamic_cast<MidasCollectionTreeItem*>(item)) != NULL)
    {
    mdo::Collection* collection = collectionTreeItem->getCollection();
    mws::Collection remote;
    remote.SetWebAPI(mws::WebAPI::Instance());
    remote.SetObject(collection);
    remote.Fetch();

    emit midasCollectionTreeItemSelected(collectionTreeItem);
    }
  else if((itemTreeItem = dynamic_cast<MidasItemTreeItem*>(item)) != NULL)
    {
    mdo::Item* item = itemTreeItem->getItem();
    mws::Item remote;
    remote.SetWebAPI(mws::WebAPI::Instance());
    remote.SetObject(item);
    remote.Fetch();

    emit midasItemTreeItemSelected(itemTreeItem);
    }
  else if((bitstreamTreeItem = dynamic_cast<MidasBitstreamTreeItem*>(item)) != NULL)
    {
    mdo::Bitstream* bitstream = bitstreamTreeItem->getBitstream();
    mws::Bitstream remote;
    remote.SetWebAPI(mws::WebAPI::Instance());
    remote.SetObject(bitstream);
    remote.Fetch();

    emit midasBitstreamTreeItemSelected(bitstreamTreeItem);
    }
}

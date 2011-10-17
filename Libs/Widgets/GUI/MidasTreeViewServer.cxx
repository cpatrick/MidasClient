#include "MidasTreeViewServer.h"

#include <QtGui>
#include <QItemSelection>
#include <QContextMenuEvent>
#include <QModelIndex>
#include <QPixmap>
#include <QMimeData>

#include "MidasTreeItem.h"
#include "MidasTreeModelServer.h"
#include "MidasCommunityTreeItem.h"
#include "MidasCollectionTreeItem.h"
#include "MidasItemTreeItem.h"
#include "MidasBitstreamTreeItem.h"
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
#include "midasSynchronizer.h"
#include <iostream>

MidasTreeViewServer::MidasTreeViewServer(QWidget* parent)
: MidasTreeView(parent)
{
  m_Model = new MidasTreeModelServer;
  this->setModel(m_Model);

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

  m_MimeType = "MIDAS/server_resource";
  m_AcceptMimeType = "MIDAS/client_resource";
}

MidasTreeViewServer::~MidasTreeViewServer()
{
  delete m_Model;
  delete m_ExpandTreeThread;
}

void MidasTreeViewServer::alertFetchedMore()
{
  emit fetchedMore();
}

void MidasTreeViewServer::selectByUuid(std::string uuid, bool select)
{
  if(m_ExpandTreeThread)
    {
    disconnect(m_ExpandTreeThread);
    }
  delete m_ExpandTreeThread;

  m_ExpandTreeThread = new ExpandTreeThread(this,
    reinterpret_cast<MidasTreeModelServer*>(m_Model), uuid, select);

  connect(m_ExpandTreeThread, SIGNAL( finished() ),
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
  emit enableActions(true);
}

void MidasTreeViewServer::selectByIndex(const QModelIndex& index)
{
  selectionModel()->select(index,
    QItemSelectionModel::Select | QItemSelectionModel::Clear);
  scrollTo(index);
}

void MidasTreeViewServer::fetchItemData(MidasTreeItem* item)
{
  MidasCommunityTreeItem* communityTreeItem = NULL;
  MidasCollectionTreeItem* collectionTreeItem = NULL;
  MidasItemTreeItem* itemTreeItem = NULL;
  MidasBitstreamTreeItem* bitstreamTreeItem = NULL;

  if((communityTreeItem = dynamic_cast<MidasCommunityTreeItem*>(item)) != NULL)
    {
    mdo::Community* community = communityTreeItem->GetCommunity();
    mws::Community remote;
    remote.SetObject(community);
    remote.SetAuthenticator(m_Synch->GetAuthenticator());
    remote.Fetch();

    emit midasCommunityTreeItemSelected(communityTreeItem);
    }
  else if((collectionTreeItem = dynamic_cast<MidasCollectionTreeItem*>(item)) != NULL)
    {
    mdo::Collection* collection = collectionTreeItem->GetCollection();
    mws::Collection remote;
    remote.SetObject(collection);
    remote.SetAuthenticator(m_Synch->GetAuthenticator());
    remote.Fetch();

    emit midasCollectionTreeItemSelected(collectionTreeItem);
    }
  else if((itemTreeItem = dynamic_cast<MidasItemTreeItem*>(item)) != NULL)
    {
    mdo::Item* item = itemTreeItem->GetItem();
    mws::Item remote;
    remote.SetObject(item);
    remote.SetAuthenticator(m_Synch->GetAuthenticator());
    remote.Fetch();

    emit midasItemTreeItemSelected(itemTreeItem);
    }
  else if((bitstreamTreeItem = dynamic_cast<MidasBitstreamTreeItem*>(item)) != NULL)
    {
    mdo::Bitstream* bitstream = bitstreamTreeItem->GetBitstream();
    mws::Bitstream remote;
    remote.SetObject(bitstream);
    remote.SetAuthenticator(m_Synch->GetAuthenticator());
    remote.Fetch();

    emit midasBitstreamTreeItemSelected(bitstreamTreeItem);
    }
}

void MidasTreeViewServer::dragMoveEvent(QDragMoveEvent* event)
{
  selectionModel()->clearSelection();
  selectionModel()->select(this->indexAt(event->pos()), QItemSelectionModel::Select | QItemSelectionModel::Rows);

  if(event->mimeData()->hasFormat("MIDAS/client_resource"))
    {
    event->setAccepted(!indexAt(event->pos()).isValid());
    }
}

void MidasTreeViewServer::dropEvent( QDropEvent * event )
{
  if (!event || !event->mimeData())
    {
    event->setAccepted(false);
    return;
    }
  const QMimeData* md = event->mimeData();
  if(md->hasFormat("MIDAS/client_resource"))
    {
    QString data = QString(md->data("MIDAS/client_resource"));
    std::vector<std::string> tokens;
    midasUtils::Tokenize(data.toStdString(), tokens);

    int type = atoi( tokens[0].c_str() );
    int id   = atoi( tokens[1].c_str() );
    emit resourceDropped(type, id);
    event->acceptProposedAction();
    }
}

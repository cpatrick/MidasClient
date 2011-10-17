#include "Midas3TreeViewServer.h"

#include <QtGui>
#include <QItemSelection>
#include <QContextMenuEvent>
#include <QModelIndex>
#include <QPixmap>
#include <QMimeData>

#include "Midas3TreeItem.h"
#include "Midas3TreeModelServer.h"
#include "Midas3FolderTreeItem.h"
#include "Midas3ItemTreeItem.h"
#include "Midas3BitstreamTreeItem.h"
#include "ExpandTreeThread.h"

#include "mwsWebAPI.h"
#include "m3wsFolder.h"
#include "m3doFolder.h"
#include "m3wsItem.h"
#include "m3doItem.h"
#include "m3wsBitstream.h"
#include "m3doBitstream.h"
#include "mdoObject.h"

#include "midasUtils.h"
#include "midasSynchronizer.h"
#include <iostream>

Midas3TreeViewServer::Midas3TreeViewServer(QWidget* parent)
: Midas3TreeView(parent)
{
  m_Model = new Midas3TreeModelServer;
  this->setModel(m_Model);

  this->setSelectionMode(QTreeView::SingleSelection);

  connect(this, SIGNAL(collapsed(const QModelIndex&)),
    this->model(), SLOT(itemCollapsed(const QModelIndex&)));
  connect(this, SIGNAL(expanded(const QModelIndex&)),
    this->model(), SLOT(itemExpanded(const QModelIndex&)));
  connect(m_Model, SIGNAL(expand(const QModelIndex&)),
    this, SLOT(expand(const QModelIndex&)));
  connect(this->model(), SIGNAL(fetchedMore()),
    this, SLOT(alertFetchedMore()));

  m_ExpandTreeThread = NULL;

  // define action to be triggered when tree item is selected
  QItemSelectionModel* itemSelectionModel = this->selectionModel(); 
  connect(itemSelectionModel,
     SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
     this, SLOT(updateSelection(const QItemSelection&, const QItemSelection&)));

  m_MimeType = "MIDAS/server_resource";
  m_AcceptMimeType = "MIDAS/client_resource";
}

Midas3TreeViewServer::~Midas3TreeViewServer()
{
  delete m_Model;
  delete m_ExpandTreeThread;
}

void Midas3TreeViewServer::alertFetchedMore()
{
  emit fetchedMore();
}

void Midas3TreeViewServer::selectByUuid(std::string uuid, bool select)
{
  if(m_ExpandTreeThread)
    {
    disconnect(m_ExpandTreeThread);
    }
  delete m_ExpandTreeThread;

  m_ExpandTreeThread = new ExpandTreeThread(this,
    reinterpret_cast<Midas3TreeModelServer*>(m_Model), uuid, select);

  connect(m_ExpandTreeThread, SIGNAL(finished()),
    this, SLOT(expansionDone()));
  connect(m_ExpandTreeThread, SIGNAL(expand(const QModelIndex&)),
    this, SLOT(expand(const QModelIndex&)));
  connect(m_ExpandTreeThread, SIGNAL(select(const QModelIndex&)),
    this, SLOT(selectByIndex(const QModelIndex&)));

  m_ExpandTreeThread->start();
  emit startedExpandingTree();
}

void Midas3TreeViewServer::expansionDone()
{
  emit finishedExpandingTree();
  emit enableActions(true);
}

void Midas3TreeViewServer::selectByIndex(const QModelIndex& index)
{
  selectionModel()->select(index,
    QItemSelectionModel::Select | QItemSelectionModel::Clear);
  scrollTo(index);
}

void Midas3TreeViewServer::fetchItemData(Midas3TreeItem* item)
{
  Midas3FolderTreeItem* folderTreeItem = NULL;
  Midas3ItemTreeItem* itemTreeItem = NULL;
  Midas3BitstreamTreeItem* bitstreamTreeItem = NULL;

  if((folderTreeItem = dynamic_cast<Midas3FolderTreeItem*>(item)) != NULL)
    {
    m3do::Folder* folder = folderTreeItem->getFolder();
    m3ws::Folder remote;
    remote.SetObject(folder);
    remote.SetAuthenticator(m_Synch->GetAuthenticator());
    remote.Fetch();

    emit midas3FolderTreeItemSelected(folderTreeItem);
    }
  else if((itemTreeItem = dynamic_cast<Midas3ItemTreeItem*>(item)) != NULL)
    {
    //TODO fetch?
    emit midas3ItemTreeItemSelected(itemTreeItem);
    }
  else if((bitstreamTreeItem = dynamic_cast<Midas3BitstreamTreeItem*>(item)) != NULL)
    {
    emit midas3BitstreamTreeItemSelected(bitstreamTreeItem);
    }
}

void Midas3TreeViewServer::dragMoveEvent(QDragMoveEvent* event)
{
  selectionModel()->clearSelection();
  selectionModel()->select(this->indexAt(event->pos()), QItemSelectionModel::Select | QItemSelectionModel::Rows);

  if(event->mimeData()->hasFormat("MIDAS/client_resource"))
    {
    event->setAccepted(!indexAt(event->pos()).isValid());
    }
}

void Midas3TreeViewServer::dropEvent(QDropEvent* event)
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

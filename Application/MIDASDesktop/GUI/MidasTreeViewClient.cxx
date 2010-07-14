#include "MidasTreeViewClient.h"

#include <QtGui>
#include <QItemSelection>
#include <QContextMenuEvent>
#include <QModelIndex>

#include "MIDASDesktopUI.h"
#include "MidasTreeItem.h"
#include "MidasTreeModelClient.h"
#include "MidasCommunityTreeItem.h"
#include "MidasCollectionTreeItem.h"
#include "MidasItemTreeItem.h"
#include "MidasBitstreamTreeItem.h"
#include "Logger.h"
#include "Utils.h"
#include "MidasClientGlobal.h"
#include "mwsCommunity.h"
#include "mdoBitstream.h"
#include "midasLog.h"
#include <iostream>

/** Constructor */
MidasTreeViewClient::MidasTreeViewClient(QWidget * parent):MidasTreeView(parent)
{
  // The tree model
  m_Model = new MidasTreeModelClient;
  this->setModel(m_Model);

  this->setAcceptDrops(true);
  
  this->setSelectionMode( QTreeView::SingleSelection );

  connect (this, SIGNAL( collapsed(const QModelIndex&)),
     this->model(), SLOT(itemCollapsed(const QModelIndex&)) );
  connect (this, SIGNAL( expanded(const QModelIndex&)),
     this->model(), SLOT(itemExpanded (const QModelIndex&)) );
  connect (m_Model, SIGNAL( expand(const QModelIndex&) ),
    this, SLOT( expand(const QModelIndex&) ) );

  // define action to be triggered when tree item is selected
  QItemSelectionModel * itemSelectionModel = this->selectionModel();
  connect(itemSelectionModel,
     SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
     this, SLOT(updateSelection(const QItemSelection&, const QItemSelection& )));
}

/** Destructor */
MidasTreeViewClient::~MidasTreeViewClient()
{
  delete m_Model;
}

void MidasTreeViewClient::SetDatabaseProxy(midasDatabaseProxy* proxy)
{
  reinterpret_cast<MidasTreeModelClient*>(m_Model)->SetDatabase(proxy);
}

void MidasTreeViewClient::SetLog(midasLog* log)
{
  reinterpret_cast<MidasTreeModelClient*>(m_Model)->SetLog(log);
}

void MidasTreeViewClient::contextMenuEvent( QContextMenuEvent * e )
{
  emit midasTreeViewContextMenu( e );
}

void MidasTreeViewClient::mouseDoubleClickEvent(QMouseEvent *event)
{
  MidasBitstreamTreeItem* bitstream = NULL;
  MidasTreeItem* node = const_cast<MidasTreeItem*>(
    m_Model->midasTreeItem(this->indexAt(event->pos())));

  if ((bitstream = dynamic_cast<MidasBitstreamTreeItem*>(node)) != NULL)
    {
    emit bitstreamOpenRequest();
    }
  else
    {
    QTreeView::mouseDoubleClickEvent(event);
    }
}

void MidasTreeViewClient::dragEnterEvent( QDragEnterEvent * event )
{
  if (event && event->mimeData())
    {
    const QMimeData * md = event->mimeData();
    if ( md->hasUrls() || md->hasFormat("MIDAS/resource"))
      {
      event->acceptProposedAction();
      }
    }
}

void MidasTreeViewClient::dragLeaveEvent( QDragLeaveEvent * event )
{
}

void MidasTreeViewClient::dragMoveEvent( QDragMoveEvent * event )
{
  selectionModel()->clearSelection();
  selectionModel()->select(this->indexAt(event->pos()), QItemSelectionModel::Select | QItemSelectionModel::Rows);

  if(event->mimeData()->hasUrls())
    {
    MidasItemTreeItem* item = NULL;
    MidasTreeItem* node = const_cast<MidasTreeItem*>(
      m_Model->midasTreeItem(this->indexAt(event->pos())));
    
    if ((item = dynamic_cast<MidasItemTreeItem*>(node)) != NULL)
      {
      event->acceptProposedAction();
      }
    else
      {
      event->setAccepted(false);
      }
    }
  else if(event->mimeData()->hasFormat("MIDAS/resource"))
    {
    event->setAccepted(!indexAt(event->pos()).isValid());
    }
}

void MidasTreeViewClient::dropEvent( QDropEvent * event )
{
  if (!event || !event->mimeData())
    {
    event->setAccepted(false);
    return;
    }
  const QMimeData* md = event->mimeData();
  if (md->hasUrls())
    {
    MidasItemTreeItem* item = NULL;
    MidasTreeItem* node = const_cast<MidasTreeItem*>(
      m_Model->midasTreeItem(this->indexAt(event->pos())));

    if ((item = dynamic_cast<MidasItemTreeItem*>(node)) != NULL)
      {
      QStringList files;
      foreach (QUrl url, md->urls())
        {
        QFileInfo info(url.toLocalFile());
        if (info.exists() && info.isFile())
          {
          files << url.toLocalFile();
          }
        }
      emit bitstreamsDropped( item, files );
      event->acceptProposedAction();
      }
    }
  else if(md->hasFormat("MIDAS/resource"))
    {
    QString data = QString(md->data("MIDAS/resource"));
    std::vector<std::string> tokens;
    kwutils::tokenize(data.toStdString(), tokens);

    int type = atoi( tokens[0].c_str() );
    int id   = atoi( tokens[1].c_str() );
    emit resourceDropped(type, id);
    event->acceptProposedAction();
    }
}

void MidasTreeViewClient::expandAll()
{
  m_Model->expandAllResources();
  MidasTreeView::expandAll();
}

void MidasTreeViewClient::collapseAll()
{
  m_Model->clearExpandedList();
  MidasTreeView::collapseAll();
}

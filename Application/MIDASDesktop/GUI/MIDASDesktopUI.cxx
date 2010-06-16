#include "MIDASDesktopUI.h"

#include <QModelIndex>
#include <QItemSelection>
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QFileDialog>
#include <QTest>
#include <QSettings>
#include <QDesktopServices>
#include <QMessageBox>
#include <QThread>

#include <kwsys/SystemTools.hxx>

#include "mwsSettings.h"
#include "mdoCommunity.h"
#include "mwsCommunity.h"
#include "mwsItem.h"
#include "mwsCollection.h"
#include "mwsNewResources.h"
#include "mwsSearch.h"
#include "midasAuthenticator.h"
#include "midasLog.h"
#include "midasSynchronizer.h"
#include "midasDatabaseProxy.h"
#include "midasProgressReporter.h"

#include "GUILogger.h"
#include "GUIProgress.h"
#include "Utils.h"

// ------------- Dialogs -------------
#include "CreateMidasCommunityUI.h"
#include "CreateMidasCollectionUI.h"
#include "CreateMidasItemUI.h"
#include "CreateProfileUI.h"
#include "DeleteResourceUI.h"
#include "UploadAgreementUI.h"
#include "SignInUI.h"
#include "AboutUI.h"
#include "AutoRefreshUI.h"
#include "PullUI.h"
#include "ProcessingStatusUI.h"
// ------------- Dialogs -------------

// ------------- Threads -------------
#include "RefreshServerTreeThread.h"
#include "SynchronizerThread.h"
#include "SearchThread.h"
// ------------- Threads -------------

// ------------- TreeModel / TreeView -------------
#include "MidasTreeItem.h"
#include "MidasTreeView.h"
#include "MidasTreeModel.h"
#include "MidasTreeViewClient.h"
#include "MidasTreeModelClient.h"
#include "MidasCommunityTreeItem.h"
#include "MidasCollectionTreeItem.h"
#include "MidasBitstreamTreeItem.h"
#include "MidasItemTreeItem.h"
#include "Logger.h"
#include "MidasClientGlobal.h"
#include "mwsWebAPI.h"

// ------------- TreeModel / TreeView -------------
MIDASDesktopUI::MIDASDesktopUI()
{
  setupUi(this); // this sets up GUI
  int time = static_cast<unsigned int>(kwsys::SystemTools::GetTime() * 1000);
  srand (time); //init random number generator 
  this->setWindowTitle( STR2QSTR( MIDAS_CLIENT_VERSION_STR ) );

  m_DecorateObject = NULL;
  
  // ------------- Instantiate and setup tray icon -------------
  showAction = new QAction(tr("&Show MIDASDesktop"), this);
  connect(showAction, SIGNAL(triggered()), this, SLOT(showNormal()));

  trayIconMenu = new QMenu(this);
  trayIconMenu->addAction(showAction);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(actionQuit);

  trayIcon = new QSystemTrayIcon(this);
  trayIcon->setContextMenu(trayIconMenu);
  trayIcon->setIcon(QPixmap(":icons/MIDAS_Desktop_Icon.png"));
  trayIcon->setToolTip(STR2QSTR(MIDAS_CLIENT_VERSION_STR));
  trayIcon->setVisible(true);

  connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
    this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
  // ------------- Instantiate and setup tray icon -------------

  // ------------- Instantiate and setup UI dialogs -------------
  dlg_createMidasCommunityUI =    new CreateMidasCommunityUI( this );
  dlg_createMidasSubCommunityUI = new CreateMidasCommunityUI( this, CreateMidasCommunityUI::SubCommunity );
  dlg_addMidasCommunityUI =       new CreateMidasCommunityUI( this, CreateMidasCommunityUI::ClientCommunity);
  dlg_addMidasSubCommunityUI =    new CreateMidasCommunityUI( this, CreateMidasCommunityUI::ClientSubCommunity);
  dlg_createMidasCollectionUI =   new CreateMidasCollectionUI( this );
  dlg_addMidasCollectionUI =      new CreateMidasCollectionUI( this, CreateMidasCollectionUI::ClientCollection);
  dlg_createMidasItemUI =         new CreateMidasItemUI( this );
  dlg_addMidasItemUI =            new CreateMidasItemUI( this, CreateMidasItemUI::ClientItem);

  dlg_uploadAgreementUI =  new UploadAgreementUI( this );
  dlg_signInUI =           new SignInUI( this );
  dlg_createProfileUI =    new CreateProfileUI( this );
  dlg_aboutUI =            new AboutUI( this );
  dlg_autoRefreshUI =      new AutoRefreshUI( this );
  dlg_pullUI =             new PullUI( this );
  dlg_deleteResourceUI =   new DeleteResourceUI( this );
  ProcessingStatusUI::init( this );
  // ------------- Instantiate and setup UI dialogs -------------

  // ------------- Auto Refresh Timer -----------
  refreshTimer = new QTimer(this);
  connect(refreshTimer, SIGNAL( timeout() ), treeView, SLOT( Update() ) );
  connect(dlg_autoRefreshUI, SIGNAL( intervalChanged() ), this, SLOT( setTimerInterval() ) );
  connect(dlg_autoRefreshUI, SIGNAL( settingChanged() ), this, SLOT( adjustTimerSettings() ) );
  // ------------- Auto Refresh Timer -----------

  // ------------- Item info panel -------------
  midasTreeItemInfoTable->horizontalHeader()->setStretchLastSection( true ); 
  midasTreeItemInfoTable->horizontalHeader()->hide();
  midasTreeItemInfoTable->verticalHeader()->hide();
  // ------------- Item info panel -------------

  // ------------- Status bar -------------
  stateLabel    = new QLabel();
  progressBar   = new QProgressBar();
  connectLabel  = new QLabel();
  hostLabel     = new QLabel();

  progressBar->setTextVisible(false);
  //progressBar->setMinimumSize( progressBar->sizeHint() );

  connectLabel->setAlignment( Qt::AlignCenter );
  connectLabel->setFrameShape( QFrame::Panel );
  connectLabel->setFrameShadow( QFrame::Sunken );
  connectLabel->setMinimumSize( connectLabel->sizeHint() );
  connectLabel->clear();

  hostLabel->setAlignment( Qt::AlignCenter );
  hostLabel->setFrameShape( QFrame::Panel );
  hostLabel->setFrameShadow( QFrame::Sunken );
  hostLabel->setMinimumSize( hostLabel->sizeHint() );
  hostLabel->clear();

  statusBar()->addWidget( stateLabel, 1 );
  statusBar()->addWidget( progressBar, 1 );
  statusBar()->addWidget( connectLabel );
  statusBar()->addWidget( hostLabel );
  // ------------- Status bar -------------

  // ------------- setup TreeView signals -------------

  connect(treeView, SIGNAL(midasTreeItemSelected(const MidasTreeItem*)),
    this, SLOT( updateActionState(const MidasTreeItem*) ));
  connect(treeViewClient, SIGNAL(midasTreeItemSelected(const MidasTreeItem*)),
    this, SLOT( updateActionStateClient(const MidasTreeItem*) ));

  connect(treeViewClient, SIGNAL(bitstreamsDropped(const MidasItemTreeItem*, const QStringList&)),
    this, SLOT( addBitstreams(const MidasItemTreeItem*, const QStringList&)));

  connect(treeViewClient, SIGNAL(bitstreamOpenRequest(int)), this, SLOT(openBitstream(int)));

  connect(treeView, SIGNAL(midasCommunityTreeItemSelected(const MidasCommunityTreeItem*)),
    this, SLOT( updateInfoPanel(const MidasCommunityTreeItem*) ));

  //connect(treeView, SIGNAL(fetchedMore()), this, SLOT( decorateServerTree() ));

  connect(treeView, SIGNAL(midasCollectionTreeItemSelected(const MidasCollectionTreeItem*)),
    this, SLOT( updateInfoPanel(const MidasCollectionTreeItem*) ));

  connect(treeView, SIGNAL(midasNoTreeItemSelected()),
    this, SLOT( clearInfoPanel() ));

  connect(treeView, SIGNAL(midasItemTreeItemSelected(const MidasItemTreeItem*)),
    this, SLOT( updateInfoPanel(const MidasItemTreeItem*) ));

  connect(treeView, SIGNAL(midasBitstreamTreeItemSelected(const MidasBitstreamTreeItem*)),
    this, SLOT( updateInfoPanel(const MidasBitstreamTreeItem*) ) );

  connect(treeView, SIGNAL(midasNoTreeItemSelected()),
    dlg_pullUI, SLOT( resetState() ));

  connect(treeView, SIGNAL(midasTreeViewContextMenu(QContextMenuEvent*)),
    this, SLOT( displayServerResourceContextMenu(QContextMenuEvent*) ));

  connect(treeView->model(), SIGNAL(serverPolled()), this, SLOT( storeLastPollTime()));

  connect(treeView, SIGNAL( startedExpandingTree() ), this, SLOT( startedExpandingTree() ) );
  connect(treeView, SIGNAL( finishedExpandingTree() ), this, SLOT( finishedExpandingTree() ) );

  connect(treeViewClient, SIGNAL(midasTreeViewContextMenu(QContextMenuEvent*)),
    this, SLOT( displayClientResourceContextMenu(QContextMenuEvent*) ));

  connect(dlg_pullUI, SIGNAL(pulledResources()), this, SLOT( updateClientTreeView() ) );
  // ------------- setup TreeView signals -------------

  // ------------- signal/slot connections -------------
  connect( actionPush_Resources,          SIGNAL( triggered() ), this, SLOT( pushResources() ) );
  connect( actionPull_Resource,           SIGNAL( triggered() ), dlg_pullUI, SLOT( exec() ) );
  connect( actionOpenURL,                 SIGNAL( triggered() ), this, SLOT( viewInBrowser() ) );

  connect( actionCreate_Profile, SIGNAL( triggered() ), dlg_createProfileUI, SLOT( exec() ) );
  connect( dlg_createProfileUI, SIGNAL( createdProfile(std::string, std::string, std::string, std::string)),
    this, SLOT( createProfile(std::string, std::string, std::string, std::string)));
  connect( dlg_createProfileUI, SIGNAL( serverURLSet(std::string)), this, SLOT( setServerURL(std::string)));
  connect( dlg_signInUI, SIGNAL( createProfileRequest() ), dlg_createProfileUI, SLOT( exec() ) );
  connect( dlg_deleteResourceUI, SIGNAL( deleteResource(bool) ), this, SLOT( deleteLocalResource(bool) ) );

  connect( actionChoose_Local_Database, SIGNAL( triggered() ), this, SLOT( chooseLocalDatabase() ) );

  connect( actionSign_In,      SIGNAL( triggered() ), this, SLOT( signInOrOut() ) );
  connect( actionQuit,         SIGNAL( triggered() ), qApp, SLOT( quit() ) );
  connect( actionAbout,        SIGNAL( triggered() ), dlg_aboutUI, SLOT( exec() ) );
  connect( actionAuto_Refresh, SIGNAL( triggered() ), dlg_autoRefreshUI, SLOT( exec() ) );

  connect( actionAdd_community,    SIGNAL(triggered()), this, SLOT(addCommunity()));
  connect( actionAdd_subcommunity, SIGNAL(triggered()), this, SLOT(addSubcommunity()));
  connect( actionAdd_collection,   SIGNAL(triggered()), this, SLOT(addCollection()));
  connect( actionAdd_item,         SIGNAL(triggered()), this, SLOT(addItem()));
  connect( actionAdd_bitstream,    SIGNAL(triggered()), this, SLOT(addBitstream()));
  connect( actionDelete_Resource,  SIGNAL(triggered()), dlg_deleteResourceUI, SLOT( exec() ) );

  connect( searchItemsListWidget, SIGNAL( midasListWidgetItemClicked( QListWidgetItemMidasItem * ) ),
    this, SLOT( searchItemClicked( QListWidgetItemMidasItem * ) ) );
  connect( searchItemsListWidget, SIGNAL( midasListWidgetContextMenu( QContextMenuEvent * ) ),
    this, SLOT( searchItemContextMenu( QContextMenuEvent * ) ) );

  connect( push_Button,   SIGNAL( released() ), this, SLOT( pushResources() ) );
  connect( pull_Button,   SIGNAL( released() ), dlg_pullUI, SLOT( exec() ) );
  connect( refreshButton, SIGNAL( released() ), this, SLOT( updateServerTreeView() ) );
  connect( searchButton,  SIGNAL( released() ), this, SLOT( search() ) );

  //connect( log, SIGNAL( textChanged() ), this, SLOT( showLogTab() ) );

  // ------------- signal/slot connections -------------

  // ------------- thread init -----------------
  m_RefreshThread = NULL;
  m_SynchronizerThread = NULL;
  m_SearchThread = NULL;
  // ------------- thread init -----------------

  // ------------- setup client members and logging ----
  this->m_database = NULL;
  this->m_synch = new midasSynchronizer();
  this->m_auth = new midasAuthenticator();
  this->m_progress = new GUIProgress(this->progressBar);
  this->m_logger = new GUILogger(this);
  this->m_auth->SetLog(m_logger);
  this->m_synch->SetLog(m_logger);
  this->m_synch->SetProgressReporter(m_progress);
  this->m_signIn = false;
  // ------------- setup client members and logging ----

  // ------------- Handle stored settings -------------
  QSettings settings("Kitware", "MIDASDesktop");
  std::string lastDatabase =
    settings.value("lastDatabase", "").toString().toStdString();
  this->setLocalDatabase(lastDatabase);

  // ------------- Handle stored settings -------------
}

/** Destructor */
MIDASDesktopUI::~MIDASDesktopUI()
{
  ProcessingStatusUI::finalize();
  delete dlg_aboutUI;
  delete dlg_signInUI;
  delete dlg_createProfileUI;
  delete dlg_autoRefreshUI;
  delete dlg_createMidasCommunityUI;
  delete dlg_createMidasSubCommunityUI;
  delete dlg_createMidasCollectionUI;
  delete dlg_createMidasItemUI;
  delete dlg_uploadAgreementUI;
  delete dlg_pullUI;
  delete stateLabel;
  delete connectLabel;
  delete hostLabel;
  delete refreshTimer;
  delete m_database;
  delete m_auth;
  delete m_logger;
  delete m_progress;
  delete m_synch;
  delete m_RefreshThread;
  delete m_SynchronizerThread;
  delete m_SearchThread;
}

void MIDASDesktopUI::showNormal()
{
  trayIcon->setIcon(QPixmap(":icons/MIDAS_Desktop_Icon.png"));

  if(m_database)
    {
    m_database->Open();
    if(atoi(m_database->GetSetting(midasDatabaseProxy::AUTO_REFRESH_SETTING).c_str()) == 0)
      {
      refreshTimer->stop();
      }
    m_database->Close();
    }
  QMainWindow::showNormal();
  QMainWindow::activateWindow();
}

void MIDASDesktopUI::activateActions(bool value, ActivateActions activateAction)
{
  if ( activateAction & ACTION_LOCAL_DATABASE )
    {
    this->treeViewClient->setEnabled( value );
    this->clientCollapseAllButton->setEnabled( value );
    this->clientExpandAllButton->setEnabled( value );
    this->actionAdd_community->setEnabled( value );
    this->actionCreate_Profile->setEnabled( value );
    this->actionAuto_Refresh->setEnabled( value );
    }

  if ( activateAction & ACTION_CONNECTED )
    {
    this->searchTab->setEnabled( value );
    this->treeView->setEnabled( value );
    this->midasTreeItemInfoGroupBox->setEnabled( value );
    this->pull_Button->setEnabled( value );
    this->push_Button->setEnabled( value );
    this->actionPush_Resources->setEnabled( value );
    this->searchButton->setEnabled( value );
    this->searchQueryEdit->setEnabled( value );
    this->refreshButton->setEnabled( value );
    actionSign_In->setText( value ? tr("Sign Out") : tr("Sign In") );
    }

  if ( activateAction & ACTION_COMMUNITY  )
    {
    this->actionPull_Resource->setEnabled( value );
    this->actionOpenURL->setEnabled( value );
    }

  if ( activateAction & ACTION_COLLECTION  )
    {
    this->actionPull_Resource->setEnabled( value );
    this->actionOpenURL->setEnabled( value );
    }

  if ( activateAction & ACTION_ITEM  )
    {
    this->actionPull_Resource->setEnabled( value );
    this->actionOpenURL->setEnabled( value );
    }

  if ( activateAction & ACTION_BITSTREAM )
    {
    this->actionPull_Resource->setEnabled( value );
    }

  if ( activateAction & ACTION_CLIENT_COMMUNITY )
    {
    this->actionAdd_subcommunity->setEnabled( value );
    this->actionAdd_collection->setEnabled( value );
    }

  if ( activateAction & ACTION_CLIENT_COLLECTION )
    {
    this->actionAdd_item->setEnabled( value );
    }

  if ( activateAction & ACTION_CLIENT_ITEM )
    {
    this->actionAdd_bitstream->setEnabled( value );
    }

  if ( activateAction & ACTION_CLIENT_BITSTREAM )
    {
    }

  if( activateAction & ACTION_CLIENT_RESOURCE )
    {
    this->actionDelete_Resource->setEnabled( value );
    }
}

void MIDASDesktopUI::closeEvent(QCloseEvent *event)
{
  if (trayIcon->isVisible())
    {
    trayIcon->showMessage(tr("MIDASDesktop"),
      tr("The program will keep running in the system tray.  To terminate "
      "the program, choose Quit in the menu "));
    hide();
    event->ignore();

    m_database->Open();
    if(atoi(m_database->GetSetting(midasDatabaseProxy::AUTO_REFRESH_SETTING).c_str()) == 0)
      {
      refreshTimer->start();
      }
    m_database->Close();
    }
}

void MIDASDesktopUI::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
  switch (reason)
    {
    case QSystemTrayIcon::Trigger:
      break;
    case QSystemTrayIcon::DoubleClick:
      showNormal();
      break;
    case QSystemTrayIcon::MiddleClick:
      break;
    default:
      break;
    }
}

void MIDASDesktopUI::updateActionState( const MidasTreeItem* item )
{
  MidasCommunityTreeItem * communityTreeItem = NULL; 
  MidasCollectionTreeItem * collectionTreeItem = NULL; 
  MidasItemTreeItem * itemTreeItem = NULL; 
  MidasBitstreamTreeItem * bitstreamTreeItem = NULL;

  // disable all actions
  this->activateActions( false, ACTION_ALL_CONNECTED );

  if ((communityTreeItem = dynamic_cast<MidasCommunityTreeItem*>( const_cast<MidasTreeItem*>( item ) ) ) != NULL )
    {
    this->activateActions( true, ACTION_COMMUNITY );
    this->dlg_pullUI->setPullId(communityTreeItem->getCommunity()->GetId());
    this->dlg_pullUI->setResourceTypeIndex(0);
    }
  else if ((collectionTreeItem = dynamic_cast<MidasCollectionTreeItem*>( const_cast<MidasTreeItem*>( item ) ) ) != NULL )
    {
    this->activateActions( true, ACTION_COLLECTION );
    this->dlg_pullUI->setPullId(collectionTreeItem->getCollection()->GetId());
    this->dlg_pullUI->setResourceTypeIndex(1);
    }
  else if ((itemTreeItem = dynamic_cast<MidasItemTreeItem*>( const_cast<MidasTreeItem*>( item ) ) ) != NULL )
    {
    this->activateActions( true, ACTION_ITEM | ACTION_BITSTREAM );
    this->dlg_pullUI->setPullId(itemTreeItem->getItem()->GetId());
    this->dlg_pullUI->setResourceTypeIndex(2);
    }
  else if ((bitstreamTreeItem = dynamic_cast<MidasBitstreamTreeItem*>( const_cast<MidasTreeItem*>( item ) ) ) != NULL )
    {
    this->activateActions( true, ACTION_BITSTREAM );
    this->dlg_pullUI->setPullId(bitstreamTreeItem->getBitstream()->GetId());
    this->dlg_pullUI->setResourceTypeIndex(3);
    }
}

void MIDASDesktopUI::updateActionStateClient( const MidasTreeItem* item )
{
  MidasCommunityTreeItem * communityTreeItem = NULL;
  MidasCollectionTreeItem * collectionTreeItem = NULL;
  MidasItemTreeItem * itemTreeItem = NULL;
  MidasBitstreamTreeItem * bitstreamTreeItem = NULL;

  // disable all actions
  this->activateActions( false, ACTION_ALL_CONNECTED );
  this->activateActions( false, ACTION_CLIENT_COMMUNITY
                              | ACTION_CLIENT_COLLECTION
                              | ACTION_CLIENT_ITEM
                              | ACTION_CLIENT_BITSTREAM );

  if ((communityTreeItem = dynamic_cast<MidasCommunityTreeItem*>( const_cast<MidasTreeItem*>( item ) ) ) != NULL )
    {
    this->activateActions( true, ACTION_CLIENT_COMMUNITY );
    }
  else if ((collectionTreeItem = dynamic_cast<MidasCollectionTreeItem*>( const_cast<MidasTreeItem*>( item ) ) ) != NULL )
    {
    this->activateActions( true, ACTION_CLIENT_COLLECTION );
    }
  else if ((itemTreeItem = dynamic_cast<MidasItemTreeItem*>( const_cast<MidasTreeItem*>( item ) ) ) != NULL )
    {
    this->activateActions( true, ACTION_CLIENT_ITEM );
    }
  else if ((bitstreamTreeItem = dynamic_cast<MidasBitstreamTreeItem*>( const_cast<MidasTreeItem*>( item ) ) ) != NULL )
    {
    this->activateActions( true, ACTION_CLIENT_BITSTREAM );
    }
}

void MIDASDesktopUI::updateClientTreeView()
{
  this->treeTabContainer->setCurrentIndex(1);
  this->treeViewClient->Update();
}

void MIDASDesktopUI::updateServerTreeView()
{
  this->treeTabContainer->setCurrentIndex(0);
  if(m_RefreshThread)
    {
    disconnect(m_RefreshThread);
    }
  delete m_RefreshThread;

  m_RefreshThread = new RefreshServerTreeThread;
  m_RefreshThread->SetParentUI(this);
  
  connect(m_RefreshThread, SIGNAL( enableRefresh(bool) ), refreshButton, SLOT( setEnabled(bool) ) );
  connect(m_RefreshThread, SIGNAL( threadComplete() ), this, SLOT( resetStatus() ) );

  displayStatus("Refreshing server tree...");
  setProgressIndeterminate();

  m_RefreshThread->start();
}

void MIDASDesktopUI::enableActions(bool val)
{
  this->activateActions(val, MIDASDesktopUI::ACTION_CONNECTED);
}

void MIDASDesktopUI::resetStatus()
{
  setProgressEmpty();
  displayStatus("");
}

void MIDASDesktopUI::alertNewResources()
{
  disconnect(trayIcon, SIGNAL( messageClicked() ), this, SLOT( showNormal() ) );
  trayIcon->showMessage(tr("MIDASDesktop - New Resources"),
    tr("There are new resources on the MIDAS server.  Click this message "
    "to show the MIDASDesktop window."));
  
  if(this->isHidden())
    {
    trayIcon->setIcon(QPixmap(":icons/MIDAS_Desktop_Icon_Red_v1.png"));
    connect(trayIcon, SIGNAL( messageClicked() ), this, SLOT( showNormal() ) );
    }
}

void MIDASDesktopUI::showLogTab()
{
  this->logAndSearchTabContainer->setCurrentIndex(1);
}

/** Show the community information */
void MIDASDesktopUI::updateInfoPanel( const MidasCommunityTreeItem* communityTreeItem )
{ 
  QTableWidgetDescriptionItem::Options options = QTableWidgetDescriptionItem::Tooltip;

  midasTreeItemInfoGroupBox->setTitle(tr(" Community description ")); 

  midasTreeItemInfoTable->disconnect( SIGNAL( itemChanged ( QTableWidgetItem * ) ) );
  midasTreeItemInfoTable->clearSelection();

  mdo::Community* community = communityTreeItem->getCommunity();
  mws::Community remote;
  remote.SetWebAPI(mws::WebAPI::Instance());
  remote.SetObject(community);
  remote.Fetch();

  int i = 0;

  if(community->GetName() != "") i++;
  if(community->GetDescription() != "") i++;
  if(community->GetIntroductoryText() != "") i++;
  if(community->GetCopyright() != "") i++;
  if(community->GetLinks() != "") i++;

  midasTreeItemInfoTable->setRowCount( i );
  i = 0; 
  
  if(community->GetName() != "")
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Name", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCommunityDescItem(community, community->GetName().c_str(), COMMUNITY_NAME, options));
    i++; 
    }

  if(community->GetDescription() != "")
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Description", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCommunityDescItem(community, community->GetDescription().c_str(), COMMUNITY_DESCRIPTION, options));
    i++;
    }

  if(community->GetIntroductoryText() != "")
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Introductory", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCommunityDescItem(community, community->GetIntroductoryText().c_str(), COMMUNITY_INTRODUCTORY, options));
    i++;
    }
  
  if(community->GetCopyright() != "")
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Copyright", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCommunityDescItem(community, community->GetCopyright().c_str(), COMMUNITY_COPYRIGHT, options));
    i++;
    }

  if(community->GetLinks() != "")
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Links", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCommunityDescItem(community, community->GetLinks().c_str(), COMMUNITY_LINKS, options));
    i++;
    }
  midasTreeItemInfoTable->resizeColumnsToContents();
}

void MIDASDesktopUI::updateInfoPanel( const MidasCollectionTreeItem* collectionTreeItem )
{
  QTableWidgetDescriptionItem::Options options = QTableWidgetDescriptionItem::Tooltip;

  mdo::Collection* collection = collectionTreeItem->getCollection();
  mws::Collection remote;
  remote.SetWebAPI(mws::WebAPI::Instance());
  remote.SetObject(collection);
  remote.Fetch();

  midasTreeItemInfoGroupBox->setTitle(tr(" Collection description "));

  midasTreeItemInfoTable->disconnect( SIGNAL( itemChanged ( QTableWidgetItem * ) ) );
  midasTreeItemInfoTable->clearSelection();
  
  int i = 0;
  if(collection->GetName() != "") i++;
  if(collection->GetDescription() != "") i++;
  
  midasTreeItemInfoTable->setRowCount( i );
  i = 0;

  if(collection->GetName() != "")
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Name", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCollectionDescItem(collection, collection->GetName().c_str(), COLLECTION_NAME, options));
    i++;
    }

  if(collection->GetDescription() != "")
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Description", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCollectionDescItem(collection, collection->GetDescription().c_str(), COLLECTION_DESCRIPTION, options));
    i++;
    }

  midasTreeItemInfoTable->resizeColumnsToContents();
}

void MIDASDesktopUI::updateInfoPanel( const MidasItemTreeItem* itemTreeItem )
{
  QTableWidgetDescriptionItem::Options options = QTableWidgetDescriptionItem::Tooltip; 

  mdo::Item* item = itemTreeItem->getItem();
  mws::Item remote;
  remote.SetWebAPI(mws::WebAPI::Instance());
  remote.SetObject(item);
  remote.Fetch();

  midasTreeItemInfoGroupBox->setTitle(tr(" Item description "));

  midasTreeItemInfoTable->disconnect( SIGNAL( itemChanged ( QTableWidgetItem * ) ) ); 
  midasTreeItemInfoTable->clearSelection();
  
  int i = 0;
  
  if(item->GetTitle() != "") i++;
  if(item->GetAuthors().size()) i++;
  if(item->GetKeywords().size()) i++;
  if(item->GetAbstract() != "") i++;
  if(item->GetDescription() != "") i++;

  midasTreeItemInfoTable->setRowCount(i);
  i = 0;

  if(item->GetTitle() != "")
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Title", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasItemDescItem(item, item->GetTitle().c_str(), ITEM_TITLE, options));
    i++;
    }

  if(item->GetAuthors().size())
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Authors", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasItemDescItem(item, kwutils::concatenate(item->GetAuthors(), "/ ").c_str(), ITEM_AUTHORS, options));
    i++;
    }

  if(item->GetKeywords().size())
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Keywords", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasItemDescItem(item, kwutils::concatenate(item->GetKeywords(), "/ ").c_str(), ITEM_KEYWORDS, options));
    i++;
    }

  if(item->GetAbstract() != "")
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Abstract", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasItemDescItem(item, item->GetAbstract().c_str(), ITEM_ABSTRACT, options));
    i++;
    }

  if(item->GetDescription() != "")
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Description", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasItemDescItem(item, item->GetDescription().c_str(), ITEM_DESCRIPTION, options));
    i++;
    }

  midasTreeItemInfoTable->resizeColumnsToContents();
}

void MIDASDesktopUI::updateInfoPanel(const MidasBitstreamTreeItem* bitstreamTreeItem)
{
  QTableWidgetDescriptionItem::Options options = QTableWidgetDescriptionItem::Tooltip; 

  mdo::Bitstream* bitstream = bitstreamTreeItem->getBitstream();
  mws::Bitstream remote;
  remote.SetWebAPI(mws::WebAPI::Instance());
  remote.SetObject(bitstream);
  remote.Fetch();

  midasTreeItemInfoGroupBox->setTitle(tr(" Bitstream description "));

  //midasTreeItemInfoTable->disconnect( SIGNAL( bitstreamChanged ( QTableWidgetItem * ) ) ); 
  midasTreeItemInfoTable->clearSelection();
  midasTreeItemInfoTable->setRowCount(2);
  int i = 0;

  midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
  midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Filename", QTableWidgetDescriptionItem::Bold));
  midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasBitstreamDescItem(bitstream, bitstream->GetName().c_str(), BITSTREAM_NAME, options));
  i++;

  midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
  midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Size", QTableWidgetDescriptionItem::Bold));
  midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasBitstreamDescItem(bitstream, midasUtils::FileSizeString(atol(bitstream->GetSize().c_str())).c_str(), BITSTREAM_SIZE, options));
  i++;

  midasTreeItemInfoTable->resizeColumnsToContents();
}

void MIDASDesktopUI::clearInfoPanel()
{
  midasTreeItemInfoTable->clear();
  midasTreeItemInfoTable->setRowCount( 0 );
}

void MIDASDesktopUI::displayClientResourceContextMenu( QContextMenuEvent* e )
{
  QMenu menu( this );
  MidasCommunityTreeItem * communityTreeItem = NULL;
  MidasCollectionTreeItem * collectionTreeItem = NULL;
  MidasItemTreeItem * itemTreeItem = NULL;
  MidasBitstreamTreeItem * bitstreamTreeItem = NULL;

  QModelIndex index = treeViewClient->indexAt( e->pos() );
  MidasTreeItem * item = const_cast<MidasTreeItem*>( 
                         reinterpret_cast<MidasTreeModelClient*>(treeViewClient->model())->midasTreeItem( index ) );
  
  if ( index.isValid() )
    {
    treeViewClient->selectionModel()->select( index, QItemSelectionModel::SelectCurrent ); 

    if ( ( communityTreeItem = dynamic_cast<MidasCommunityTreeItem*>( item ) ) != NULL)
      {
      menu.addAction( this->actionAdd_subcommunity );
      menu.addAction( this->actionAdd_collection );
      }
    else if ( ( collectionTreeItem = dynamic_cast<MidasCollectionTreeItem*>( item ) ) != NULL )
      {
      menu.addAction( this->actionAdd_item );
      }
    else if ( ( itemTreeItem = dynamic_cast<MidasItemTreeItem*>( item ) ) != NULL )
      {
      menu.addAction( this->actionAdd_bitstream );
      }
    else if ( ( bitstreamTreeItem = dynamic_cast<MidasBitstreamTreeItem*>( item ) ) != NULL )
      {
      //add any bitstream context menu actions here.
      }
    menu.addSeparator();
    menu.addAction( this->actionDelete_Resource );
    }
  else 
    {
    treeView->selectionModel()->clearSelection(); 
    menu.addAction( this->actionAdd_community );
    menu.addAction( this->actionPush_Resources );
    }
  menu.exec( e->globalPos() );
}

void MIDASDesktopUI::displayServerResourceContextMenu( QContextMenuEvent* e )
{
  QMenu menu( this );
  QModelIndex index = treeView->indexAt( e->pos() );

  if ( index.isValid() )
    {
    treeView->selectionModel()->select( index, QItemSelectionModel::SelectCurrent ); 
    }
  else 
    {
    treeView->selectionModel()->clearSelection(); 
    }
  menu.addAction( this->actionPull_Resource );
  menu.addAction( this->actionOpenURL );
  menu.exec( e->globalPos() );
}

void MIDASDesktopUI::addCommunity()
{
  this->dlg_addMidasCommunityUI->exec();
}

void MIDASDesktopUI::addSubcommunity()
{
  this->dlg_addMidasSubCommunityUI->exec();
}

void MIDASDesktopUI::addCollection()
{
  this->dlg_addMidasCollectionUI->exec();
}

void MIDASDesktopUI::addItem()
{
  this->dlg_addMidasItemUI->exec();
}

void MIDASDesktopUI::addBitstream()
{
  QStringList files = QFileDialog::getOpenFileNames(
                                    this, tr("Load Files"),
                                    QDir::current().absolutePath(),
                                    tr("All Files (*)"));

  if(files.size())
    {
    addBitstreams(reinterpret_cast<MidasItemTreeItem*>(
      const_cast<MidasTreeItem*>(
      treeViewClient->getSelectedMidasTreeItem())), files);
    }
}

void MIDASDesktopUI::addBitstreams(const MidasItemTreeItem* parentItem,
                               const QStringList & files)
{
  for(QStringList::const_iterator i = files.begin(); i != files.end(); ++i)
    {
    std::string path = i->toStdString();
    std::string name = kwsys::SystemTools::GetFilenameName(path.c_str());
    std::string uuid = midasUtils::GenerateUUID();
    this->m_database->Open();
    std::string parentUuid = this->m_database->GetUuid(
      midasResourceType::ITEM, parentItem->getItem()->GetId());
    int id = this->m_database->AddResource(midasResourceType::BITSTREAM, uuid, 
      path, name, parentUuid, 0);
    this->m_database->MarkDirtyResource(uuid, midasDirtyAction::ADDED);
    this->m_database->Close();

    if(id)
      {
      std::stringstream text;
      text << "Added bitstream " << name << " under item " << 
        parentItem->getItem()->GetTitle() << ".";
      this->m_logger->Message(text.str());
      }
    }
  this->updateClientTreeView();
}

void MIDASDesktopUI::viewInBrowser()
{
  std::string baseUrl = m_url;
  kwsys::SystemTools::ReplaceString(baseUrl, "/api/rest", "");
  std::stringstream path;
  path << baseUrl;

  MidasTreeItem* resource = const_cast<MidasTreeItem*>(
    treeView->getSelectedMidasTreeItem());
  MidasCommunityTreeItem* comm = NULL;
  MidasCollectionTreeItem* coll = NULL;
  MidasItemTreeItem* item = NULL;
  MidasBitstreamTreeItem* bitstream = NULL;

  if ((comm = dynamic_cast<MidasCommunityTreeItem*>(resource)) != NULL)
    {
    path << "/community/view/" << comm->getCommunity()->GetId();
    }
  else if ((coll = dynamic_cast<MidasCollectionTreeItem*>(resource)) != NULL)
    {
    path << "/collection/view/" << coll->getCollection()->GetId();
    }
  else if ((item = dynamic_cast<MidasItemTreeItem*>(resource)) != NULL)
    {
    path << "/item/view/" << item->getItem()->GetId();
    }

  QUrl url(path.str().c_str());
  if(!QDesktopServices::openUrl(url))
    {
    std::stringstream text;
    text << "The operating system does not know how to open "
      << path.str() << std::endl;
    m_logger->Error(text.str());
    }
}

void MIDASDesktopUI::displayStatus(const QString& message)
{
  stateLabel->setText(message); 
}

void MIDASDesktopUI::signInOrOut()
{
  if ( !this->m_signIn )
    {
    this->dlg_signInUI->exec();
    }
  else
    {
    this->signOut(); 
    }
}

void MIDASDesktopUI::setTimerInterval()
{
  m_database->Open();
  int minutes = atoi(m_database->GetSetting(midasDatabaseProxy::AUTO_REFRESH_INTERVAL).c_str());
  m_database->Close();
  refreshTimer->setInterval(minutes * 60 * 1000);
}

void MIDASDesktopUI::adjustTimerSettings()
{
  m_database->Open();
  int setting = atoi(m_database->GetSetting(midasDatabaseProxy::AUTO_REFRESH_SETTING).c_str());
  m_database->Close();

  refreshTimer->stop();

  switch(setting)
    {
    case 0:
      if(this->isHidden() && this->m_signIn)
        {
        refreshTimer->start();
        }
      break;
    case 1:
      if(this->m_signIn)
        {
        refreshTimer->start();
        }
      break;
    default:
      break;
    }
}

void MIDASDesktopUI::signIn(bool ok)
{
  if(ok)
    {
    connectLabel->hide();
    hostLabel->hide();
    activateActions( true, MIDASDesktopUI::ACTION_CONNECTED );

    // start the refresh timer here if our setting = 1
    m_database->Open();
    if(atoi(m_database->GetSetting(midasDatabaseProxy::AUTO_REFRESH_SETTING).c_str()) == 1)
      {
      refreshTimer->start();
      }
    m_database->Close();

    // Satus bar
    std::string connect = "  Connected to " + std::string(mws::WebAPI::Instance()->GetServerUrl()) + "  "; 
    connectLabel->setText( connect.c_str() );
    connectLabel->show();
    setTreeTabIndex(0);
      
    std::string host = "  " + std::string(mws::WebAPI::Instance()->GetServerUrl()) + "  ";
    hostLabel->setText( host.c_str() );
    hostLabel->show();

    std::stringstream text;
    text << "Signed in with profile " << m_auth->GetProfile();
    getLog()->Message(text.str());
    m_signIn = true;
    displayStatus(tr(""));
    }
  else
    {
    getLog()->Error("The URL provided is not a valid MIDAS server Web API.");
    displayStatus(tr("Failed to connect"));
    }
  setProgressEmpty();
}

void MIDASDesktopUI::chooseLocalDatabase()
{
  // ------------- display FileDialog -------------
  QString file = QFileDialog::getOpenFileName(
                                    this, tr("Choose Database File"),
                                    QDir::current().absolutePath(),
                                    tr("All Files (*)"));

  setLocalDatabase(file.toStdString());
}

void MIDASDesktopUI::setLocalDatabase(std::string file)
{
  if(file == "" || !midasUtils::IsDatabaseValid(file))
    {
    std::stringstream text;
    text << file << " is not a valid MIDAS SQLite database. Defaulting "
      " to midas.db.";
    this->m_logger->Message(text.str());
    std::string path = kwsys::SystemTools::GetCurrentWorkingDirectory()
      + "/midas.db";
    if(midasUtils::IsDatabaseValid(path))
      {
      setLocalDatabase(path);
      return;
      }
    this->m_logger->Error("No suitable database file found!");
    return;
    }

  if(midasUtils::IsDatabaseValid(file))
    {
    delete m_database;
    this->m_auth->SetDatabase(file);
    this->m_synch->SetDatabase(file);
    this->m_database = new midasDatabaseProxy(file);
    QSettings settings("Kitware", "MIDASDesktop");
    settings.setValue("lastDatabase", file.c_str());
    settings.sync();
    this->displayStatus(tr("Opened database successfully."));
    this->treeViewClient->SetDatabaseProxy(m_database);
    this->activateActions(true, MIDASDesktopUI::ACTION_LOCAL_DATABASE);
    this->treeViewClient->SetLog(m_logger);
    this->updateClientTreeView();
    this->treeViewClient->collapseAll();
    setTimerInterval();
    adjustTimerSettings();
    }
  else
    {
    std::stringstream text;
    text << "The path " << file << " does not refer to a valid MidasDesktop "
      "database.";
    m_logger->Error(text.str());
    }
}

void MIDASDesktopUI::setServerURL(std::string url)
{
  this->m_synch->SetServerURL(url);
  this->m_auth->SetServerURL(url);
  mws::WebAPI::Instance()->SetServerUrl(url.c_str());
  this->m_url = url;
}

void MIDASDesktopUI::createProfile(std::string name, std::string email,
                               std::string apiName, std::string apiKey)
{
  if(!m_database)
    {
    this->displayStatus(tr("Please choose a local database first."));
    return;
    }

  std::string msg;
  m_auth->SetServerURL(m_url);
  if(m_auth->AddAuthProfile(email, apiName, apiKey, name))
    {
    msg = "Successfully created profile \"" + name + "\".";
    this->dlg_signInUI->profileCreated(name);
    }
  else
    {
    msg = "Failed to create authentication profile.";
    }
  this->displayStatus(tr(msg.c_str()));
}

/** Signing out */
void MIDASDesktopUI::signOut()
{
  this->activateActions(false, ACTION_ALL); 
  treeView->Clear(); 
  this->clearInfoPanel();
  connectLabel->hide();
  hostLabel->hide();
  this->displayStatus(tr("Logout"));
  this->searchItemsListWidget->clear();
  m_signIn = false;

  refreshTimer->stop();
}

void MIDASDesktopUI::pushResources()
{
  this->displayStatus(tr("Pushing locally added resources..."));
  this->setProgressIndeterminate();
  this->m_synch->SetOperation(midasSynchronizer::OPERATION_PUSH);

  if(m_SynchronizerThread)
    {
    disconnect(m_SynchronizerThread);
    }
  delete m_SynchronizerThread;

  m_SynchronizerThread = new SynchronizerThread;
  m_SynchronizerThread->SetParentUI(this);

  connect(m_SynchronizerThread, SIGNAL(enableActions(bool) ),
    this, SLOT(enableActions(bool) ) );
  connect(m_SynchronizerThread, SIGNAL(performReturned(bool) ),
    this, SLOT(pushReturned(bool) ) );

  m_SynchronizerThread->start();
}

void MIDASDesktopUI::pushReturned(bool ok)
{
  this->updateClientTreeView();
  this->updateServerTreeView();

  if(ok)
    {
    this->displayStatus(tr("Finished pushing locally added resources."));
    }
  else
    {
    this->displayStatus(tr("Failed to push resources to the server."));
    }
  this->setProgressEmpty();
}

void MIDASDesktopUI::search()
{
  this->displayStatus(tr("Searching...")); 
  searchItemsListWidget->clear();
  std::vector<std::string> words;
  kwutils::tokenize(searchQueryEdit->text().toStdString(), words);

  if(m_SearchThread)
    {
    disconnect(m_SearchThread);
    }
  delete m_SearchThread;
  
  m_SearchResults.clear();

  m_SearchThread = new SearchThread;
  m_SearchThread->SetParentUI(this);
  m_SearchThread->SetWords(words);
  m_SearchThread->SetResults(&this->m_SearchResults);
  
  connect(m_SearchThread, SIGNAL( threadComplete() ),
    this, SLOT( showSearchResults() ) );

  activateActions(false, ACTION_CONNECTED);
  displayStatus("Performing search...");
  setProgressIndeterminate();

  m_SearchThread->start();
}

void MIDASDesktopUI::showSearchResults()
{
  for(std::vector<mdo::Object*>::iterator i = m_SearchResults.begin();
      i != m_SearchResults.end(); ++i)
    {
    new QListWidgetItemMidasItem(this->searchItemsListWidget, *i);
    }
  activateActions(true, ACTION_CONNECTED);
  this->resetStatus();
}

void MIDASDesktopUI::searchItemClicked(QListWidgetItemMidasItem * listItem)
{
  this->treeTabContainer->setCurrentIndex(0);
  this->treeView->selectByObject(listItem->getObject());
}

void MIDASDesktopUI::searchItemContextMenu(QContextMenuEvent* e)
{
  QMenu menu( this );
  QModelIndex index = searchItemsListWidget->indexAt( e->pos() );

  if ( index.isValid() )
    {
    menu.addAction( this->actionOpenURL );
    menu.addAction( this->actionPull_Resource );
    menu.exec( e->globalPos() );
    }
}

void MIDASDesktopUI::openBitstream(int id)
{
  if(!this->m_database)
    {
    return;
    }
  m_database->Open();
  std::string path = m_database->GetRecordByUuid(
    m_database->GetUuid(midasResourceType::BITSTREAM, id)).Path;
  m_database->Close();

  if(path != "")
    {
    path = "file:" + path;
    QUrl url(path.c_str());
    if(!QDesktopServices::openUrl(url))
      {
      std::stringstream text;
      text << "The operating system does not know how to open "
        << path << std::endl;
      m_logger->Error(text.str());
      }
    }
}

void MIDASDesktopUI::storeLastPollTime()
{
  mws::NewResources newResources;
  m_database->Open();
  newResources.SetSince(m_database->GetSetting(midasDatabaseProxy::LAST_FETCH_TIME));
  newResources.Fetch();
  this->m_dirtyUuids = newResources.GetUuids();

  if(m_dirtyUuids.size())
    {
    alertNewResources();
    }

  m_database->SetSetting(midasDatabaseProxy::LAST_FETCH_TIME, newResources.GetTimestamp());
  m_database->Close();

  this->decorateServerTree();
}

void MIDASDesktopUI::decorateServerTree()
{
  if(m_dirtyUuids.size())
    {
    m_DecorateObject = midasUtils::FetchByUuid(m_dirtyUuids[0]);
    this->treeView->selectByObject(m_DecorateObject);
    connect(treeView, SIGNAL(finishedExpandingTree()), this,
      SLOT(decorateTheObject() ) );
    }
}

void MIDASDesktopUI::decorateTheObject()
{
  this->treeView->decorateByUuid(m_DecorateObject->GetUuid());
  m_dirtyUuids.erase(m_dirtyUuids.begin());
  delete m_DecorateObject;
  disconnect(treeView, SIGNAL(finishedExpandingTree()),
    this, SLOT(decorateTheObject() ) );
  decorateServerTree();
}

void MIDASDesktopUI::setProgressEmpty()
{
  this->m_progress->ResetProgress();
}

void MIDASDesktopUI::setProgressIndeterminate()
{
  this->m_progress->SetIndeterminate();
}

void MIDASDesktopUI::setTreeTabIndex(int index)
{
  this->treeTabContainer->setCurrentIndex(index);
}

void MIDASDesktopUI::startedExpandingTree()
{
  this->setProgressIndeterminate();
  this->displayStatus("Finding resource in the server tree...");
  this->activateActions(false, MIDASDesktopUI::ACTION_CONNECTED);
}

void MIDASDesktopUI::finishedExpandingTree()
{
  this->setProgressEmpty();
  this->displayStatus("");
  this->activateActions(true, MIDASDesktopUI::ACTION_CONNECTED);
}

void MIDASDesktopUI::deleteLocalResource(bool deleteFiles)
{
  std::string uuid = this->treeViewClient->getSelectedMidasTreeItem()->getUuid();
  this->m_database->Open();
  this->m_database->DeleteResource(uuid, deleteFiles);
  this->m_database->Close();

  this->updateClientTreeView();
  std::stringstream text;
  text << "Deleted resource with uuid=" << uuid << ".";
  this->m_logger->Message(text.str());
}

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
#include <QItemDelegate>
#include <QUrl>

#include "midasStandardIncludes.h"

#include "mwsSettings.h"
#include "mdoCommunity.h"
#include "mwsCommunity.h"
#include "mwsItem.h"
#include "mwsCollection.h"
#include "mdsBitstream.h"
#include "mwsNewResources.h"
#include "mwsSearch.h"
#include "mwsRestXMLParser.h"
#include "midasAuthenticator.h"
#include "midasLog.h"
#include "midasSynchronizer.h"
#include "midasDatabaseProxy.h"
#include "midasProgressReporter.h"

#include "GUIAgreement.h"
#include "GUILogger.h"
#include "GUIProgress.h"
#include "Utils.h"
#include "ResourceEdit.h"
#include "ButtonDelegate.h"

// ------------- Dialogs -------------
#include "AboutUI.h"
#include "AddKeywordUI.h"
#include "AddAuthorUI.h"
#include "AgreementUI.h"
#include "CreateMidasResourceUI.h"
#include "CreateProfileUI.h"
#include "DeleteResourceUI.h"
#include "PreferencesUI.h"
#include "ProcessingStatusUI.h"
#include "PullUI.h"
#include "SignInUI.h"
// ------------- Dialogs -------------

// ------------- Threads -------------
#include "RefreshServerTreeThread.h"
#include "SynchronizerThread.h"
#include "SearchThread.h"
#include "ReadDatabaseThread.h"
#include "PollFilesystemThread.h"
// ------------- Threads -------------

// ------------- TreeModel / TreeView -------------
#include "MidasTreeItem.h"
#include "MidasTreeViewServer.h"
#include "MidasTreeModelServer.h"
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
  dlg_createMidasResourceUI =  new CreateMidasResourceUI( this );
  dlg_signInUI =               new SignInUI( this );
  dlg_createProfileUI =        new CreateProfileUI( this );
  dlg_aboutUI =                new AboutUI( this );
  dlg_preferencesUI =          new PreferencesUI( this );
  dlg_pullUI =                 new PullUI( this );
  dlg_deleteClientResourceUI = new DeleteResourceUI( this, false );
  dlg_deleteServerResourceUI = new DeleteResourceUI( this, true );
  dlg_addAuthorUI =            new AddAuthorUI( this );
  dlg_addKeywordUI =           new AddKeywordUI( this );
  m_agreementHandler =         new GUIAgreement( this );
  dlg_agreementUI =            new AgreementUI( this,
    dynamic_cast<GUIAgreement*>(this->m_agreementHandler) );
  ProcessingStatusUI::init( this );
  // ------------- Instantiate and setup UI dialogs -------------

  // ------------- Auto Refresh Timer -----------
  refreshTimer = new QTimer(this);
  connect(refreshTimer, SIGNAL( timeout() ), treeViewServer, SLOT( Update() ) );
  connect(dlg_preferencesUI, SIGNAL( intervalChanged() ), this, SLOT( setTimerInterval() ) );
  connect(dlg_preferencesUI, SIGNAL( settingChanged() ), this, SLOT( adjustTimerSettings() ) );
  // ------------- Auto Refresh Timer -----------

  // ------------- Item info panel -------------
  midasTreeItemInfoTable->horizontalHeader()->setStretchLastSection( true ); 
  midasTreeItemInfoTable->horizontalHeader()->hide();
  midasTreeItemInfoTable->verticalHeader()->hide();

  connect(midasTreeItemInfoTable, SIGNAL( itemChanged( QTableWidgetItem*) ), this, SLOT(
    resourceEdited(QTableWidgetItem*) ) );

  authorsEditor = new ButtonDelegate(this);
  authorsEditor->setField(ITEM_AUTHORS);
  authorsEditor->setEditUI(dlg_addAuthorUI);
  
  keywordsEditor = new ButtonDelegate(this);
  keywordsEditor->setField(ITEM_KEYWORDS);
  keywordsEditor->setEditUI(dlg_addKeywordUI);
  // ------------- Item info panel -------------

  saveButton = new QPushButton();
  saveButton->setText("Save");

  // ------------- Status bar -------------
  stateLabel    = new QLabel();
  progressBar   = new QProgressBar();
  connectLabel  = new QLabel();
  cancelButton  = new QPushButton();

  stateLabel->setWordWrap(true);

  progressBar->setTextVisible(false);

  connectLabel->setAlignment( Qt::AlignCenter );
  connectLabel->setFrameShape( QFrame::Panel );
  connectLabel->setFrameShadow( QFrame::Sunken );
  connectLabel->setMinimumSize( connectLabel->sizeHint() );
  connectLabel->clear();

  cancelButton->setText("Cancel");
  cancelButton->setIcon(QPixmap(":icons/delete2.png"));
  cancelButton->setEnabled(false);
  cancelButton->setMaximumHeight(21);

  statusBar()->addWidget( stateLabel, 1 );
  statusBar()->addWidget( progressBar, 1 );
  statusBar()->addWidget( cancelButton );
  statusBar()->addWidget( connectLabel );
  // ------------- Status bar -------------

  // ------------- setup TreeView signals -------------

  treeViewServer->SetParentUI(this);
  treeViewClient->SetParentUI(this);

  connect(treeViewServer, SIGNAL(midasTreeItemSelected(const MidasTreeItem*)),
    this, SLOT( updateActionState(const MidasTreeItem*) ));
  connect(treeViewClient, SIGNAL(midasTreeItemSelected(const MidasTreeItem*)),
    this, SLOT( updateActionStateClient(const MidasTreeItem*) ));

  connect(treeViewClient, SIGNAL(bitstreamsDropped(const MidasItemTreeItem*, const QStringList&)),
    this, SLOT( addBitstreams(const MidasItemTreeItem*, const QStringList&)));
  connect(treeViewClient, SIGNAL(resourceDropped(int, int)),
    this, SLOT( pullRecursive(int, int) ) );

  connect(treeViewClient, SIGNAL( bitstreamOpenRequest() ), this, SLOT( viewDirectory() ) );

  connect(treeViewServer, SIGNAL(midasCommunityTreeItemSelected(const MidasCommunityTreeItem*)),
    this, SLOT( updateInfoPanel(const MidasCommunityTreeItem*) ));
  connect(treeViewClient, SIGNAL(midasCommunityTreeItemSelected(const MidasCommunityTreeItem*)),
    this, SLOT( updateInfoPanel(const MidasCommunityTreeItem*) ));

  connect(treeViewServer, SIGNAL(midasCollectionTreeItemSelected(const MidasCollectionTreeItem*)),
    this, SLOT( updateInfoPanel(const MidasCollectionTreeItem*) ));
  connect(treeViewClient, SIGNAL(midasCollectionTreeItemSelected(const MidasCollectionTreeItem*)),
    this, SLOT( updateInfoPanel(const MidasCollectionTreeItem*) ));

  connect(treeViewServer, SIGNAL(midasItemTreeItemSelected(const MidasItemTreeItem*)),
    this, SLOT( updateInfoPanel(const MidasItemTreeItem*) ));
  connect(treeViewClient, SIGNAL(midasItemTreeItemSelected(const MidasItemTreeItem*)),
    this, SLOT( updateInfoPanel(const MidasItemTreeItem*) ));

  connect(treeViewServer, SIGNAL(midasBitstreamTreeItemSelected(const MidasBitstreamTreeItem*)),
    this, SLOT( updateInfoPanel(const MidasBitstreamTreeItem*) ) );
  connect(treeViewClient, SIGNAL(midasBitstreamTreeItemSelected(const MidasBitstreamTreeItem*)),
    this, SLOT( updateInfoPanel(const MidasBitstreamTreeItem*) ) );

  connect(treeViewServer, SIGNAL(midasNoTreeItemSelected()),
    this, SLOT( clearInfoPanel() ));
  connect(treeViewClient, SIGNAL(midasNoTreeItemSelected()),
    this, SLOT( clearInfoPanel() ));

  connect(treeViewServer, SIGNAL(midasNoTreeItemSelected()),
    dlg_pullUI, SLOT( resetState() ));

  connect(treeViewServer, SIGNAL(midasTreeViewContextMenu(QContextMenuEvent*)),
    this, SLOT( displayServerResourceContextMenu(QContextMenuEvent*) ));
  connect(treeViewClient, SIGNAL(midasTreeViewContextMenu(QContextMenuEvent*)),
    this, SLOT( displayClientResourceContextMenu(QContextMenuEvent*) ));

  connect(treeViewServer->model(), SIGNAL(serverPolled()), this, SLOT( storeLastPollTime()));

  connect(treeViewServer, SIGNAL( startedExpandingTree() ), this, SLOT( startedExpandingTree() ) );
  connect(treeViewServer, SIGNAL( finishedExpandingTree() ), this, SLOT( finishedExpandingTree() ) ); 

  connect(dlg_pullUI, SIGNAL(pulledResources()), this, SLOT( updateClientTreeView() ) );
  // ------------- setup TreeView signals -------------

  // ------------- signal/slot connections -------------
  connect( actionPush_Resources,          SIGNAL( triggered() ), this, SLOT( pushResources() ) );
  connect( actionPull_Resource,           SIGNAL( triggered() ), dlg_pullUI, SLOT( exec() ) );
  connect( actionOpenURL,                 SIGNAL( triggered() ), this, SLOT( viewInBrowser() ) );
  //connect( actionSwap_with_MD5_reference, SIGNAL( triggered() ), this, SLOT( 

  connect( actionCreate_Profile, SIGNAL( triggered() ), dlg_createProfileUI, SLOT( exec() ) );
  connect( dlg_createProfileUI, SIGNAL( createdProfile(std::string, std::string, std::string, std::string, std::string)),
    this, SLOT( createProfile(std::string, std::string, std::string, std::string, std::string)));
  connect( dlg_createProfileUI, SIGNAL( deletedProfile(std::string)),
    dlg_signInUI, SLOT( removeProfile(std::string)));
  connect( dlg_createProfileUI, SIGNAL( serverURLSet(std::string)), this, SLOT( setServerURL(std::string)));
  connect( dlg_signInUI, SIGNAL( createProfileRequest() ), dlg_createProfileUI, SLOT( exec() ) );
  connect( dlg_deleteClientResourceUI, SIGNAL( deleteResource(bool) ), this, SLOT( deleteLocalResource(bool) ) );
  connect( dlg_deleteServerResourceUI, SIGNAL( deleteResource(bool) ), this, SLOT( deleteServerResource(bool) ) );

  connect( actionChoose_Local_Database, SIGNAL( triggered() ), this, SLOT( chooseLocalDatabase() ) );
  connect( actionNew_Local_Database, SIGNAL( triggered() ), this, SLOT( createLocalDatabase() ) );

  connect( actionSign_In,      SIGNAL( triggered() ), this, SLOT( signInOrOut() ) );
  connect( actionQuit,         SIGNAL( triggered() ), qApp, SLOT( quit() ) );
  connect( actionAbout,        SIGNAL( triggered() ), dlg_aboutUI, SLOT( exec() ) );
  connect( actionPreferences,  SIGNAL( triggered() ), dlg_preferencesUI, SLOT( exec() ) );

  connect( actionAdd_community,    SIGNAL(triggered()), this, SLOT(addCommunity()));
  connect( actionAdd_subcommunity, SIGNAL(triggered()), this, SLOT(addSubcommunity()));
  connect( actionAdd_collection,   SIGNAL(triggered()), this, SLOT(addCollection()));
  connect( actionAdd_item,         SIGNAL(triggered()), this, SLOT(addItem()));
  connect( actionAdd_bitstream,    SIGNAL(triggered()), this, SLOT(addBitstream()));
  connect( actionDelete_Resource,  SIGNAL(triggered()), dlg_deleteClientResourceUI, SLOT( exec() ) );
  connect( actionDelete_server,    SIGNAL(triggered()), dlg_deleteServerResourceUI, SLOT( exec() ) );
  connect( actionView_Directory,   SIGNAL(triggered()), this, SLOT(viewDirectory()));

  connect( searchItemsListWidget, SIGNAL( midasListWidgetItemClicked( QListWidgetItemMidasItem * ) ),
    this, SLOT( searchItemClicked( QListWidgetItemMidasItem * ) ) );
  connect( searchItemsListWidget, SIGNAL( midasListWidgetContextMenu( QContextMenuEvent * ) ),
    this, SLOT( searchItemContextMenu( QContextMenuEvent * ) ) );

  connect( push_Button,    SIGNAL( released() ), this, SLOT( pushResources() ) );
  connect( pull_Button,    SIGNAL( released() ), dlg_pullUI, SLOT( exec() ) );
  connect( refreshButton,  SIGNAL( released() ), this, SLOT( updateServerTreeView() ) );
  connect( searchButton,   SIGNAL( released() ), this, SLOT( search() ) );
  connect( cancelButton,   SIGNAL( released() ), this, SLOT( cancel() ) );
  connect( editInfoButton, SIGNAL( released() ), this, SLOT( editInfo() ) );

  connect( log, SIGNAL( textChanged() ), this, SLOT( showLogTab() ) );
  connect( logAndSearchTabContainer, SIGNAL( currentChanged(int) ),
    this, SLOT( clearLogTabIcon(int) ) );

  // ------------- signal/slot connections -------------

  // ------------- thread init -----------------
  m_RefreshThread = NULL;
  m_SynchronizerThread = NULL;
  m_SearchThread = NULL;
  m_ReadDatabaseThread = NULL;
  m_PollFilesystemThread = NULL;
  connect(&m_CreateDBWatcher, SIGNAL(finished()), this, SLOT(newDBFinished()));
  // ------------- thread init -----------------

  // ------------- setup client members and logging ----
  this->m_database = NULL;
  this->m_synch = new midasSynchronizer();
  this->m_auth = new midasAuthenticator();
  this->m_progress = new GUIProgress(this->progressBar);
  this->Log = new GUILogger(this);
  this->m_auth->SetLog(this->Log);
  this->m_synch->SetLog(this->Log);
  this->m_synch->SetProgressReporter(m_progress);
  this->m_signIn = false;
  this->m_editMode = false;
  this->m_cancel = false;
  // ------------- setup client members and logging ----

  // ------------- Progress bar ------------------------
  connect(dynamic_cast<GUIProgress*>(m_progress), SIGNAL( ProgressMessage(const QString&) ), this, SLOT( currentFileMessage(const QString&) ) );
  connect(dynamic_cast<GUIProgress*>(m_progress), SIGNAL( OverallProgress(int, int) ), this, SLOT( overallProgressUpdate(int, int) ) );
  connect(dynamic_cast<GUIProgress*>(m_progress), SIGNAL( CurrentProgress(double, double) ), this, SLOT( currentProgressUpdate(double, double) ) );
  connect(dynamic_cast<GUIProgress*>(m_progress), SIGNAL( Speed(double) ), this, SLOT( progressSpeedUpdate(double) ) );
  connect(dynamic_cast<GUIProgress*>(m_progress), SIGNAL( EstimatedTime(double) ), this, SLOT( estimatedTimeUpdate(double) ) );
  // ------------- Progress bar ------------------------

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
  delete trayIconMenu;
  delete showAction;
  delete trayIcon;
  delete dlg_aboutUI;
  delete dlg_signInUI;
  delete dlg_createProfileUI;
  delete dlg_preferencesUI;
  delete dlg_createMidasResourceUI;
  delete dlg_pullUI;
  delete dlg_addAuthorUI;
  delete dlg_addKeywordUI;
  delete dlg_agreementUI;
  delete dlg_deleteClientResourceUI;
  delete dlg_deleteServerResourceUI;
  delete stateLabel;
  delete connectLabel;
  delete cancelButton;
  delete saveButton;
  delete refreshTimer;
  delete m_auth;
  delete Log;
  delete m_progress;
  delete m_synch;
  delete m_agreementHandler;
  if(m_RefreshThread && m_RefreshThread->isRunning())
    {
    m_RefreshThread->terminate();
    m_RefreshThread->wait();
    }
  delete m_RefreshThread;
  if(m_SynchronizerThread && m_SynchronizerThread->isRunning())
    {
    m_SynchronizerThread->terminate();
    m_SynchronizerThread->wait();
    }
  delete m_SynchronizerThread;
  delete m_SearchThread;
  if(m_ReadDatabaseThread && m_ReadDatabaseThread->isRunning())
    {
    m_ReadDatabaseThread->terminate();
    m_ReadDatabaseThread->wait();
    }

  if(m_PollFilesystemThread && m_PollFilesystemThread->isRunning())
    {
    m_PollFilesystemThread->terminate();
    m_PollFilesystemThread->wait();
    }
  delete m_PollFilesystemThread;
  delete m_ReadDatabaseThread;
  delete authorsEditor;
  delete keywordsEditor;
  delete mws::WebAPI::Instance()->GetRestAPI();
}

void MIDASDesktopUI::showNormal()
{
  trayIcon->setIcon(QPixmap(":icons/MIDAS_Desktop_Icon.png"));

  if(m_database)
    {
    if(atoi(m_database->GetSetting(midasDatabaseProxy::AUTO_REFRESH_SETTING).c_str()) == 0)
      {
      refreshTimer->stop();
      }
    }
  if(m_PollFilesystemThread)
    {
    m_PollFilesystemThread->Resume();
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
    this->actionPreferences->setEnabled( value );
    this->midasTreeItemInfoGroupBox->setEnabled( value );
    }

  if ( activateAction & ACTION_CONNECTED )
    {
    this->searchTab->setEnabled( value );
    this->treeViewServer->setEnabled( value );
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
    this->actionDelete_server->setEnabled( value );
    }

  if ( activateAction & ACTION_COLLECTION  )
    {
    this->actionPull_Resource->setEnabled( value );
    this->actionOpenURL->setEnabled( value );
    this->actionDelete_server->setEnabled( value );
    }

  if ( activateAction & ACTION_ITEM  )
    {
    this->actionPull_Resource->setEnabled( value );
    this->actionOpenURL->setEnabled( value );
    this->actionDelete_server->setEnabled( value );
    this->actionDownload_key_files_tgz->setEnabled( value );
    this->actionDownload_key_files_zip->setEnabled( value );
    }

  if ( activateAction & ACTION_BITSTREAM )
    {
    this->actionPull_Resource->setEnabled( value );
    this->actionDelete_server->setEnabled( value );
    this->actionDownload_key_file->setEnabled( value );
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
    this->actionView_Directory->setEnabled( value );
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

    if(m_signIn)
      {
      if(atoi(m_database->GetSetting(midasDatabaseProxy::AUTO_REFRESH_SETTING).c_str()) == 0)
        {
        refreshTimer->start();
        }
      }
    }

  // stop filesystem polling (we don't care about updating the UI)
  if(m_PollFilesystemThread)
    {
    m_PollFilesystemThread->Pause();
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
  // disable all actions
  this->activateActions( false, ACTION_ALL_CONNECTED );
  this->dlg_pullUI->setPullId(item->getId());
  this->dlg_pullUI->setResourceType(item->getType());
  this->dlg_pullUI->setResourceName(item->data(0).toString().toStdString());

  if (item->getType() == midasResourceType::COMMUNITY)
    {
    this->activateActions( true, ACTION_COMMUNITY );
    }
  else if (item->getType() == midasResourceType::COLLECTION)
    {
    this->activateActions( true, ACTION_COLLECTION );
    }
  else if (item->getType() == midasResourceType::ITEM)
    {
    this->activateActions( true, ACTION_ITEM | ACTION_BITSTREAM );
    }
  else if (item->getType() == midasResourceType::BITSTREAM)
    {
    this->activateActions( true, ACTION_BITSTREAM );
    }
}

void MIDASDesktopUI::updateActionStateClient( const MidasTreeItem* item )
{
  // disable all actions
  this->activateActions( false, ACTION_ALL_CONNECTED );
  this->activateActions( false, ACTION_CLIENT_COMMUNITY
                              | ACTION_CLIENT_COLLECTION
                              | ACTION_CLIENT_ITEM
                              | ACTION_CLIENT_BITSTREAM );

  if (item->getType() == midasResourceType::COMMUNITY)
    {
    this->activateActions( true, ACTION_CLIENT_COMMUNITY );
    }
  else if (item->getType() == midasResourceType::COLLECTION)
    {
    this->activateActions( true, ACTION_CLIENT_COLLECTION );
    }
  else if (item->getType() == midasResourceType::ITEM)
    {
    this->activateActions( true, ACTION_CLIENT_ITEM );
    }
  else if (item->getType() == midasResourceType::BITSTREAM)
    {
    this->activateActions( true, ACTION_CLIENT_BITSTREAM );
    }
}

void MIDASDesktopUI::updateClientTreeView()
{
  if(m_ReadDatabaseThread && m_ReadDatabaseThread->isRunning())
    {
    return;
    }
  if(m_ReadDatabaseThread)
    {
    disconnect(m_ReadDatabaseThread);
    }
  delete m_ReadDatabaseThread;

  m_ReadDatabaseThread = new ReadDatabaseThread;
  m_ReadDatabaseThread->SetParentUI(this);

  connect(m_ReadDatabaseThread, SIGNAL( threadComplete() ), this, SLOT( resetStatus() ) );
  connect(m_ReadDatabaseThread, SIGNAL( enableActions(bool) ), this, SLOT( enableClientActions(bool) ) );

  displayStatus("Reading local database...");
  setProgressIndeterminate();

  m_ReadDatabaseThread->start();
}

void MIDASDesktopUI::updateServerTreeView()
{
  if(m_RefreshThread)
    {
    disconnect(m_RefreshThread);
    }
  delete m_RefreshThread;

  m_RefreshThread = new RefreshServerTreeThread;
  m_RefreshThread->SetParentUI(this);
  
  connect(m_RefreshThread, SIGNAL( threadComplete() ), this, SLOT( resetStatus() ) );
  connect(m_RefreshThread, SIGNAL( threadComplete() ), this, SLOT( clearInfoPanel() ) );
  connect(m_RefreshThread, SIGNAL( enableActions(bool) ), this, SLOT( enableActions(bool) ) );

  displayStatus("Refreshing server tree...");
  setProgressIndeterminate();

  m_RefreshThread->start();
}

void MIDASDesktopUI::enableActions(bool val)
{
  this->activateActions(val, MIDASDesktopUI::ACTION_CONNECTED);
  this->cancelButton->setEnabled(!val);
}

void MIDASDesktopUI::enableClientActions(bool val)
{
  this->activateActions(val, MIDASDesktopUI::ACTION_LOCAL_DATABASE);
}

//Send cancel signals to any active push or pull operation
void MIDASDesktopUI::cancel()
{
  if(m_SynchronizerThread)
    {
    m_SynchronizerThread->Cancel();
    }
  if(dlg_pullUI->getSynchronizerThread())
    {
    dlg_pullUI->getSynchronizerThread()->Cancel();
    }
  if(m_dirtyUuids.size())
    {
    m_cancel = true;
    }
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
  // Put anything that should happen whenever new text appears in the log.
}

void MIDASDesktopUI::updateInfoPanel( const MidasCommunityTreeItem* communityTreeItem )
{
  this->m_editMode = false;
  infoPanel(const_cast<MidasCommunityTreeItem*>(communityTreeItem), false);
}

void MIDASDesktopUI::updateInfoPanel( const MidasCollectionTreeItem* collectionTreeItem )
{
  this->m_editMode = false;
  infoPanel(const_cast<MidasCollectionTreeItem*>(collectionTreeItem), false);
}

void MIDASDesktopUI::updateInfoPanel( const MidasItemTreeItem* itemTreeItem )
{
  this->m_editMode = false;
  infoPanel(const_cast<MidasItemTreeItem*>(itemTreeItem), false);
}

void MIDASDesktopUI::updateInfoPanel( const MidasBitstreamTreeItem* bitstreamTreeItem )
{
  this->m_editMode = false;
  infoPanel(const_cast<MidasBitstreamTreeItem*>(bitstreamTreeItem), false);
}

/** Show the community information */
void MIDASDesktopUI::infoPanel(MidasCommunityTreeItem* communityTreeItem, bool edit)
{
  QTableWidgetDescriptionItem::Options options = QTableWidgetDescriptionItem::Tooltip;
  if(edit) options |= QTableWidgetDescriptionItem::Editable;

  midasTreeItemInfoGroupBox->setTitle(edit ? " Edit community info " : " Community description "); 
  midasTreeItemInfoTable->setGridStyle(edit ? Qt::DashDotLine : Qt::NoPen);
  midasTreeItemInfoTable->clearSelection();

  mdo::Community* community = communityTreeItem->getCommunity();

  enableResourceEditing(communityTreeItem->isClientResource() && !edit);

  int i = 0;

  if(community->GetName() != "" || edit) i++;
  if(community->GetDescription() != "" || edit) i++;
  if(community->GetIntroductoryText() != "" || edit) i++;
  if(community->GetCopyright() != "" || edit) i++;
  if(community->GetLinks() != "" || edit) i++;

  midasTreeItemInfoTable->setRowCount( i );
  i = 0; 
  
  if(community->GetName() != "" || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Name", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCommunityDescItem(community, community->GetName().c_str(), COMMUNITY_NAME, options));
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++; 
    }

  if(community->GetDescription() != "" || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Description", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCommunityDescItem(community, community->GetDescription().c_str(), COMMUNITY_DESCRIPTION, options));
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  if(community->GetIntroductoryText() != "" || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Introductory", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCommunityDescItem(community, community->GetIntroductoryText().c_str(), COMMUNITY_INTRODUCTORY, options));
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }
  
  if(community->GetCopyright() != "" || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Copyright", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCommunityDescItem(community, community->GetCopyright().c_str(), COMMUNITY_COPYRIGHT, options));
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  if(community->GetLinks() != "" || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Links", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCommunityDescItem(community, community->GetLinks().c_str(), COMMUNITY_LINKS, options));
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  midasTreeItemInfoTable->resizeColumnsToContents();
}

void MIDASDesktopUI::infoPanel(MidasCollectionTreeItem* collectionTreeItem, bool edit)
{
  QTableWidgetDescriptionItem::Options options = QTableWidgetDescriptionItem::Tooltip;
  if(edit) options |= QTableWidgetDescriptionItem::Editable;

  mdo::Collection* collection = collectionTreeItem->getCollection();

  midasTreeItemInfoGroupBox->setTitle(edit ? " Edit collection info " : " Collection description ");
  midasTreeItemInfoTable->setGridStyle(edit ? Qt::DashDotLine : Qt::NoPen);
  midasTreeItemInfoTable->clearSelection();

  enableResourceEditing(collectionTreeItem->isClientResource() && !edit);

  int i = 0;
  if(collection->GetName() != "" || edit) i++;
  if(collection->GetDescription() != "" || edit) i++;
  if(collection->GetCopyright() != "" || edit) i++;
  if(collection->GetIntroductoryText() != "" || edit) i++;
  
  midasTreeItemInfoTable->setRowCount( i );
  i = 0;

  if(collection->GetName() != "" || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Name", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCollectionDescItem(collection, collection->GetName().c_str(), COLLECTION_NAME, options));
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  if(collection->GetDescription() != "" || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Description", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCollectionDescItem(collection, collection->GetDescription().c_str(), COLLECTION_DESCRIPTION, options));
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  if(collection->GetIntroductoryText() != "" || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Introductory Text", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCollectionDescItem(collection, collection->GetIntroductoryText().c_str(), COLLECTION_INTRODUCTORY, options));
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  if(collection->GetCopyright() != "" || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Copyright", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCollectionDescItem(collection, collection->GetCopyright().c_str(), COLLECTION_COPYRIGHT, options));
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  midasTreeItemInfoTable->resizeColumnsToContents();
}

void MIDASDesktopUI::infoPanel(MidasItemTreeItem* itemTreeItem, bool edit)
{
  QTableWidgetDescriptionItem::Options options = QTableWidgetDescriptionItem::Tooltip;
  if(edit) options |= QTableWidgetDescriptionItem::Editable;

  mdo::Item* item = itemTreeItem->getItem();

  midasTreeItemInfoGroupBox->setTitle(edit? " Edit item info " : " Item description ");
  midasTreeItemInfoTable->setGridStyle(edit ? Qt::DashDotLine : Qt::NoPen);
  midasTreeItemInfoTable->clearSelection();

  enableResourceEditing(itemTreeItem->isClientResource() && !edit);
  
  int i = 0;
  
  if(item->GetTitle() != "" || edit) i++;
  if(item->GetAuthors().size() || edit) i++;
  if(item->GetKeywords().size() || edit) i++;
  if(item->GetAbstract() != "" || edit) i++;
  if(item->GetDescription() != "" || edit) i++;

  midasTreeItemInfoTable->setRowCount(i);
  i = 0;

  if(item->GetTitle() != "" || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Title", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasItemDescItem(item, item->GetTitle().c_str(), ITEM_TITLE, options));
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  if(item->GetAuthors().size() || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Authors", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasItemDescItem(item, item->GetAuthorsString().c_str(), ITEM_AUTHORS, options));
    authorsEditor->setItem(itemTreeItem);
    midasTreeItemInfoTable->setItemDelegateForRow(i, this->authorsEditor);
    i++;
    }

  if(item->GetKeywords().size() || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Keywords", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasItemDescItem(item, item->GetKeywordsString().c_str(), ITEM_KEYWORDS, options));
    keywordsEditor->setItem(itemTreeItem);
    midasTreeItemInfoTable->setItemDelegateForRow(i, this->keywordsEditor);
    i++;
    }

  if(item->GetAbstract() != "" || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Abstract", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasItemDescItem(item, item->GetAbstract().c_str(), ITEM_ABSTRACT, options));
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  if(item->GetDescription() != "" || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Description", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasItemDescItem(item, item->GetDescription().c_str(), ITEM_DESCRIPTION, options));
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  midasTreeItemInfoTable->resizeColumnsToContents();
}

void MIDASDesktopUI::infoPanel(MidasBitstreamTreeItem* bitstreamTreeItem, bool edit)
{
  QTableWidgetDescriptionItem::Options options = QTableWidgetDescriptionItem::Tooltip;
  if(edit) options |= QTableWidgetDescriptionItem::Editable;

  //enableResourceEditing(bitstreamTreeItem->isClientResource() && !edit);
  enableResourceEditing(false); //false for now (nothing to edit about a bitstream)

  mdo::Bitstream* bitstream = bitstreamTreeItem->getBitstream();

  midasTreeItemInfoGroupBox->setTitle(tr(" Bitstream description "));
  midasTreeItemInfoTable->setGridStyle(edit ? Qt::DashDotLine : Qt::NoPen);

  midasTreeItemInfoTable->clearSelection();
  int i = 2;
  
  if(bitstream->GetPath() != "" || edit) i++;
  midasTreeItemInfoTable->setRowCount(i);
  i = 0;

  midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
  midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Filename", QTableWidgetDescriptionItem::Bold));
  midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasBitstreamDescItem(bitstream, bitstream->GetName().c_str(), BITSTREAM_NAME, options));
  midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
  i++;

  midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
  midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Size", QTableWidgetDescriptionItem::Bold));
  midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasBitstreamDescItem(bitstream, midasUtils::BytesToString(strtod(bitstream->GetSize().c_str(), 0)).c_str(), BITSTREAM_SIZE, QTableWidgetDescriptionItem::Tooltip));
  midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
  i++;

  if(bitstream->GetPath() != "")
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Location", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasBitstreamDescItem(bitstream, bitstream->GetPath().c_str(), BITSTREAM_PATH, QTableWidgetDescriptionItem::Tooltip));
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  midasTreeItemInfoTable->resizeColumnsToContents();
}

void MIDASDesktopUI::clearInfoPanel()
{
  midasTreeItemInfoTable->clear();
  midasTreeItemInfoTable->setRowCount( 0 );
  enableResourceEditing(false);
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
      menu.addAction( this->actionSwap_with_MD5_reference );
      }
    menu.addSeparator();
    menu.addAction( this->actionView_Directory );
    menu.addAction( this->actionDelete_Resource );
    }
  else 
    {
    treeViewServer->selectionModel()->clearSelection();
    menu.addAction( this->actionAdd_community );
    menu.addAction( this->actionPush_Resources );
    }
  menu.exec( e->globalPos() );
}

void MIDASDesktopUI::displayServerResourceContextMenu( QContextMenuEvent* e )
{
  QMenu menu( this );
  QModelIndex index = treeViewServer->indexAt( e->pos() );
  MidasTreeItem * item = const_cast<MidasTreeItem*>( 
                         reinterpret_cast<MidasTreeModelClient*>(treeViewServer->model())->midasTreeItem( index ) );
  MidasItemTreeItem * itemTreeItem = NULL;
  MidasBitstreamTreeItem * bitstreamTreeItem = NULL;

  if ( index.isValid() )
    {
    menu.addAction( this->actionPull_Resource );
    menu.addAction( this->actionOpenURL );
    menu.addAction( this->actionDelete_server );
    treeViewServer->selectionModel()->select( index, QItemSelectionModel::SelectCurrent );
    if ( ( itemTreeItem = dynamic_cast<MidasItemTreeItem*>( item ) ) != NULL )
      {
      menu.addSeparator();
      menu.addAction( this->actionDownload_key_files_tgz );
      menu.addAction( this->actionDownload_key_files_zip );
      }
    else if ( ( bitstreamTreeItem = dynamic_cast<MidasBitstreamTreeItem*>( item ) ) != NULL )
      {
      menu.addSeparator();
      menu.addAction( this->actionDownload_key_file );
      }
    }
  else 
    {
    treeViewServer->selectionModel()->clearSelection();
    return;
    }
  menu.exec( e->globalPos() );
}

void MIDASDesktopUI::addCommunity()
{
  dlg_createMidasResourceUI->SetType(CreateMidasResourceUI::Community);
  dlg_createMidasResourceUI->exec();
}

void MIDASDesktopUI::addSubcommunity()
{
  dlg_createMidasResourceUI->SetType(CreateMidasResourceUI::SubCommunity);
  dlg_createMidasResourceUI->exec();
}

void MIDASDesktopUI::addCollection()
{
  dlg_createMidasResourceUI->SetType(CreateMidasResourceUI::Collection);
  dlg_createMidasResourceUI->exec();
}

void MIDASDesktopUI::addItem()
{
  dlg_createMidasResourceUI->SetType(CreateMidasResourceUI::Item);
  dlg_createMidasResourceUI->exec();
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
  m_PollFilesystemThread->Pause();
  for(QStringList::const_iterator i = files.begin(); i != files.end(); ++i)
    {
    std::string path = i->toStdString();
    kwsys::SystemTools::ConvertToUnixSlashes(path);
    std::string name = kwsys::SystemTools::GetFilenameName(path.c_str());
    std::string uuid = midasUtils::GenerateUUID();
    
    if(m_database->GetSettingBool(midasDatabaseProxy::UNIFIED_TREE))
      {
      std::string copyTo = m_database->GetRecordByUuid(parentItem->getUuid()).Path;
      copyTo += "/" + name;
      kwsys::SystemTools::CopyAFile(path.c_str(), copyTo.c_str());
      path = copyTo;
      }

    std::string parentUuid = this->m_database->GetUuid(
      midasResourceType::ITEM, parentItem->getItem()->GetId());
    int id = this->m_database->AddResource(midasResourceType::BITSTREAM, uuid, 
      path, name, parentUuid, 0);

    // Get and save file size
    std::stringstream size;
    size << midasUtils::GetFileLength(path.c_str());
    mdo::Bitstream bitstream;
    bitstream.SetId(id);
    bitstream.SetName(name.c_str());
    bitstream.SetSize(size.str());
    bitstream.SetUuid(uuid.c_str());
    bitstream.SetLastModified(kwsys::SystemTools::ModifiedTime(path.c_str()));
    mds::Bitstream mdsBitstream;
    mdsBitstream.SetDatabase(m_database);
    mdsBitstream.SetObject(&bitstream);
    mdsBitstream.MarkAsDirty();
    mdsBitstream.Commit();

    if(id)
      {
      std::stringstream text;
      text << "Added bitstream " << name << " under item " << 
        parentItem->getItem()->GetTitle() << ".";
      this->GetLog()->Message(text.str());
      }
    }
  m_PollFilesystemThread->Resume();
  this->updateClientTreeView();
}

void MIDASDesktopUI::viewDirectory()
{
  MidasTreeItem* resource = const_cast<MidasTreeItem*>(
    treeViewClient->getSelectedMidasTreeItem());

  std::string path = m_database->GetRecordByUuid(resource->getUuid()).Path;

  path = "file:" + path;
  QUrl url(path.c_str());
  if(!QDesktopServices::openUrl(url))
    {
    std::stringstream text;
    text << "The operating system does not know how to open "
      << path << std::endl;
    GetLog()->Error(text.str());
    }
}

void MIDASDesktopUI::viewInBrowser()
{
  std::string baseUrl = m_url;
  kwsys::SystemTools::ReplaceString(baseUrl, "/api/rest", "");
  std::stringstream path;
  path << baseUrl;

  MidasTreeItem* resource = const_cast<MidasTreeItem*>(
    treeViewServer->getSelectedMidasTreeItem());
  MidasCommunityTreeItem* comm = NULL;
  MidasCollectionTreeItem* coll = NULL;
  MidasItemTreeItem* item = NULL;

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
    GetLog()->Error(text.str());
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
  int minutes = atoi(m_database->GetSetting(midasDatabaseProxy::AUTO_REFRESH_INTERVAL).c_str());
  refreshTimer->setInterval(minutes * 60 * 1000);
}

void MIDASDesktopUI::adjustTimerSettings()
{
  int setting = atoi(m_database->GetSetting(midasDatabaseProxy::AUTO_REFRESH_SETTING).c_str());

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
    activateActions( true, MIDASDesktopUI::ACTION_CONNECTED );

    // start the refresh timer here if our setting = 1
    if(atoi(m_database->GetSetting(midasDatabaseProxy::AUTO_REFRESH_SETTING).c_str()) == 1)
      {
      refreshTimer->start();
      }

    // Satus bar
    std::string connect = "  Connected to " + std::string(mws::WebAPI::Instance()->GetServerUrl()) + "  "; 
    connectLabel->setText( connect.c_str() );
    connectLabel->show();

    std::stringstream text;
    text << "Signed in with profile " << m_auth->GetProfile();
    GetLog()->Message(text.str());
    m_signIn = true;
    displayStatus(tr(""));
    }
  else
    {
    GetLog()->Error("The URL provided is not a valid MIDAS server Web API.");
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

void MIDASDesktopUI::createLocalDatabase()
{
  std::string file = QFileDialog::getSaveFileName(this,
    tr("New Database File"),
    QDir::current().absolutePath(),
    tr("Database Files (*.db)"), 0,
    QFileDialog::ShowDirsOnly).toStdString();

  if(kwsys::SystemTools::FileExists(file.c_str()))
    {
    std::stringstream text;
    text << "Error: " << file << " already exists.  Choose a new file name.";
    this->Log->Error(text.str());
    return;
    }

  if(file != "")
    {
    this->Log->Message("Creating new local database at " + file);
    this->actionNew_Local_Database->setEnabled(false);
    this->setProgressIndeterminate();
    QFuture<bool> future = QtConcurrent::run(midasUtils::CreateNewDatabase, file);
    m_CreateDBWatcher.setFuture(future);
    }
}

void MIDASDesktopUI::setLocalDatabase(std::string file)
{
  if(file == "" || !midasUtils::IsDatabaseValid(file))
    {
    std::stringstream text;
    text << file << " is not a valid MIDAS SQLite database. Defaulting "
      " to midas.db.";
    GetLog()->Message(text.str());
    std::string path = kwsys::SystemTools::GetCurrentWorkingDirectory()
      + "/midas.db";
    if(midasUtils::IsDatabaseValid(path))
      {
      setLocalDatabase(path);
      return;
      }
    GetLog()->Error("No suitable database file found!");
    return;
    }

  if(midasUtils::IsDatabaseValid(file))
    {
    this->m_auth->SetDatabase(file);
    this->m_synch->SetDatabase(file);
    this->m_database = this->m_synch->GetDatabase();
    QSettings settings("Kitware", "MIDASDesktop");
    settings.setValue("lastDatabase", file.c_str());
    settings.sync();
    this->displayStatus(tr("Opened database successfully."));
    this->Log->Message("Opened database " + file);
    this->treeViewClient->SetDatabaseProxy(m_database);
    this->activateActions(true, MIDASDesktopUI::ACTION_LOCAL_DATABASE);
    this->treeViewClient->collapseAll();
    this->updateClientTreeView();
    setTimerInterval();
    adjustTimerSettings();

    //start the filesystem monitoring thread
    m_PollFilesystemThread = new PollFilesystemThread;
    midasDatabaseProxy* db = new midasDatabaseProxy(this->m_database->GetDatabasePath());
    db->SetLog(this->Log);
    m_PollFilesystemThread->SetDatabase(db);

    connect(m_PollFilesystemThread, SIGNAL(needToRefresh()), this, SLOT(updateClientTreeView()));
    m_PollFilesystemThread->start();
    m_PollFilesystemThread->setPriority(QThread::LowestPriority);
    }
  else
    {
    std::stringstream text;
    text << "The path " << file << " does not refer to a valid MIDASDesktop "
      "database.";
    GetLog()->Error(text.str());
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
                               std::string apiName, std::string apiKey,
                               std::string rootDir)
{
  if(!m_database)
    {
    this->displayStatus(tr("Please choose a local database first."));
    return;
    }

  std::string msg;
  m_auth->SetServerURL(m_url);
  if(m_auth->AddAuthProfile(email, apiName, apiKey, rootDir, name))
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
  treeViewServer->Clear();
  this->clearInfoPanel();
  connectLabel->hide();
  this->displayStatus(tr("Logout"));
  this->searchItemsListWidget->clear();
  m_signIn = false;

  refreshTimer->stop();
}

void MIDASDesktopUI::pushResources()
{
  this->displayStatus(tr("Pushing locally added resources..."));
  this->setProgressIndeterminate();

  if(m_SynchronizerThread)
    {
    disconnect(m_SynchronizerThread);
    }
  delete m_SynchronizerThread;

  midasSynchronizer* synchronizer = new midasSynchronizer;
  synchronizer->SetDatabase(m_synch->GetDatabase()->GetDatabasePath());
  synchronizer->SetLog(this->m_synch->GetLog());
  synchronizer->SetProgressReporter(this->m_progress);
  synchronizer->SetServerURL(this->m_synch->GetServerURL());
  synchronizer->SetOperation(midasSynchronizer::OPERATION_PUSH);

  m_SynchronizerThread = new SynchronizerThread;
  m_SynchronizerThread->SetSynchronizer(synchronizer);
  m_SynchronizerThread->SetDelete(true); //delete this synchronizer object when done

  connect(m_SynchronizerThread, SIGNAL(enableActions(bool) ),
    this, SLOT(enableActions(bool) ) );
  connect(m_SynchronizerThread, SIGNAL(performReturned(int) ),
    this, SLOT(pushReturned(int) ) );

  m_SynchronizerThread->start();
}

void MIDASDesktopUI::pushReturned(int rc)
{
  this->updateClientTreeView();
  this->updateServerTreeView();

  std::stringstream text;
  if(rc == MIDAS_OK)
    {
    text << "Finished pushing locally added resources.";
    this->GetLog()->Message(text.str());
    }
  else if(rc == MIDAS_CANCELED)
    {
    text << "Push canceled by user.";
    this->GetLog()->Message(text.str());
    }
  else
    {
    text << "Failed to push resources to the server. Error code = " << rc;
    this->GetLog()->Error(text.str());
    }
  this->displayStatus(text.str().c_str());
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

  m_SearchThread = new SearchThread(this);
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
  this->treeViewServer->selectByUuid(listItem->getObject()->GetUuid());
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

void MIDASDesktopUI::storeLastPollTime()
{
  enableActions(false);
  mws::NewResources newResources;
  newResources.SetSince(m_database->GetSetting(midasDatabaseProxy::LAST_FETCH_TIME));
  newResources.Fetch();
  enableActions(true);

  this->m_dirtyUuids = newResources.GetUuids();
  std::reverse(m_dirtyUuids.begin(), m_dirtyUuids.end());

  if(m_dirtyUuids.size())
    {
    alertNewResources();
    }

  m_database->SetSetting(midasDatabaseProxy::LAST_FETCH_TIME, newResources.GetTimestamp());

  this->decorateServerTree();
}

void MIDASDesktopUI::decorateServerTree()
{
  if(m_cancel)
    {
    m_cancel = false;
    m_dirtyUuids.clear();
    return;
    }
  if(m_dirtyUuids.size())
    {
    enableActions(false);
    connect(treeViewServer, SIGNAL( finishedExpandingTree() ),
      this, SLOT( decorateCallback() ) );
    this->treeViewServer->selectByUuid(m_dirtyUuids[0]);
    }
}

void MIDASDesktopUI::decorateCallback()
{
  this->treeViewServer->decorateByUuid(m_dirtyUuids[0]);
  m_dirtyUuids.erase(m_dirtyUuids.begin());
  disconnect(treeViewServer, SIGNAL( finishedExpandingTree() ),
    this, SLOT( decorateCallback() ) );
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
  m_PollFilesystemThread->Pause();
  std::string uuid = treeViewClient->getSelectedMidasTreeItem()->getUuid();
  std::string name = treeViewClient->getSelectedMidasTreeItem()->data(0).toString().toStdString();
  if(!this->m_database->DeleteResource(uuid, deleteFiles))
    {
    this->Log->Error("Error: Delete failed on resource " + name);
    }
  else
    {
    std::stringstream text;
    text << "Deleted resource " << 
      treeViewClient->getSelectedMidasTreeItem()->data(0).toString().toStdString();
    GetLog()->Message(text.str());
    }
  m_PollFilesystemThread->Resume();

  this->updateClientTreeView();
}

// Controller for deleting server resources
void MIDASDesktopUI::deleteServerResource(bool val)
{
  const MidasTreeItem* resource = this->treeViewServer->getSelectedMidasTreeItem();
  int id = resource->getId();
  std::string typeName = kwsys::SystemTools::LowerCase(midasUtils::GetTypeName(resource->getType()));

  std::stringstream url;
  url << "midas." << typeName << ".delete?id=" << id;

  mws::RestXMLParser parser;
  mws::WebAPI::Instance()->GetRestAPI()->SetXMLParser(&parser);
  std::stringstream text;
  if(mws::WebAPI::Instance()->Execute(url.str().c_str()))
    {
    text << "Successfully deleted " << typeName
         << " with id=" << id << " from the server.";
    this->Log->Message(text.str());
    this->updateServerTreeView();
    }
  else
    {
    text << "Failed to delete " << typeName << " with id=" << id
      << " from the server: " << mws::WebAPI::Instance()->GetErrorMessage();
    this->Log->Error(text.str());
    }
}

void MIDASDesktopUI::alertErrorInLog()
{
  if(this->logAndSearchTabContainer->currentIndex() != 2)
    {
    this->logAndSearchTabContainer->setTabIcon(2, QPixmap(":icons/exclamation.png"));
    }
}

void MIDASDesktopUI::clearLogTabIcon(int index)
{
  if(index == 2)
    {
    this->logAndSearchTabContainer->setTabIcon(2, QPixmap());
    }
}

void MIDASDesktopUI::pullRecursive(int type, int id)
{
  if(QMessageBox::question(this, "Confirm Pull",
     "Are you sure you want to pull the selected resource and all of its children?",
     QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
    {
    dlg_pullUI->setPull();
    dlg_pullUI->setRecursive(true);
    dlg_pullUI->setResourceType(type);
    dlg_pullUI->setPullId(id);
    dlg_pullUI->accept();
    }
}

void MIDASDesktopUI::enableResourceEditing(bool val)
{
  editInfoButton->setEnabled(val);
}

void MIDASDesktopUI::editInfo()
{
  MidasTreeItem* node = const_cast<MidasTreeItem*>(
    treeViewClient->getSelectedMidasTreeItem());

  MidasCommunityTreeItem* comm = NULL;
  MidasCollectionTreeItem* coll = NULL;
  MidasItemTreeItem* item = NULL;
  MidasBitstreamTreeItem* bitstream = NULL;

  if((comm = dynamic_cast<MidasCommunityTreeItem*>(node)) != NULL)
    {
    infoPanel(comm, true);
    }
  else if((coll = dynamic_cast<MidasCollectionTreeItem*>(node)) != NULL)
    {
    infoPanel(coll, true);
    }
  else if((item = dynamic_cast<MidasItemTreeItem*>(node)) != NULL)
    {
    infoPanel(item, true);
    }
  else if((bitstream = dynamic_cast<MidasBitstreamTreeItem*>(node)) != NULL)
    {
    infoPanel(bitstream, true);
    }
  this->m_editMode = true;
}

void MIDASDesktopUI::resourceEdited(QTableWidgetItem* row)
{
  if(this->m_editMode)
    {
    ResourceEdit editor(this->m_database);
    editor.SetLog(this->Log);

    connect(&editor, SIGNAL(DataModified(std::string)), treeViewClient,
      SLOT( decorateByUuid(std::string) ) );

    editor.Save(row);
    disconnect(&editor);
    }
}

void MIDASDesktopUI::newDBFinished()
{
  this->actionNew_Local_Database->setEnabled(true);
  this->setProgressEmpty();
  if(m_CreateDBWatcher.result())
    {
    this->Log->Message("New local database created successfully.");
    }
  else
    {
    this->Log->Error("Failed to create new local database.");
    }
}

void MIDASDesktopUI::currentFileMessage(const QString& message)
{
  std::string labelText = "Current File: " + message.toStdString();
  this->currentFilenameLabel->setText(labelText.c_str());
}

void MIDASDesktopUI::overallProgressUpdate(int current, int max)
{
  std::stringstream text;
  text << "Overall Progress: " << current << " / " << max << " files transferred";
  this->overallProgressLabel->setText(text.str().c_str());

  progressBar_overall->setMaximum(max);
  progressBar_overall->setValue(current);
}

void MIDASDesktopUI::currentProgressUpdate(double current, double max)
{
  std::string currentText = midasUtils::BytesToString(current);
  std::string maxText = midasUtils::BytesToString(max);
  std::stringstream text;
  if(max == 0)
    {
    text << "Progress: Calculating...";
    }
  else
    {
    text << "Progress: " << currentText << " / " << maxText;
    }
  this->currentProgressLabel->setText(text.str().c_str());

  if (max == 0)
    {
    progressBar_current->setMaximum(100);
    progressBar_current->setValue(0);
    return;
    }
  double fraction = current / max;
  int percent = static_cast<int>(fraction * 100.0);
  progressBar_current->setMaximum(100);
  progressBar_current->setValue(percent);
}

void MIDASDesktopUI::progressSpeedUpdate(double bytesPerSec)
{
  std::stringstream text;
  if(bytesPerSec == 0)
    {
    text << "Speed: Calculating...";
    }
  else
    {
    text << "Speed: " << midasUtils::BytesToString(bytesPerSec)
         << " / sec";
    }
  this->speedLabel->setText(text.str().c_str());
}

void MIDASDesktopUI::estimatedTimeUpdate(double seconds)
{
  std::stringstream text;
  if(seconds == 0)
    {
    text << "Estimated Time Remaining: Calculating...";
    }
  else
    {
    text << "Estimated Time Remaining: "
         << midasUtils::FormatTimeString(seconds);
    }
  this->estimatedTimeLabel->setText(text.str().c_str());
}

void MIDASDesktopUI::showUserAgreementDialog()
{
  dlg_agreementUI->exec();
}

void MIDASDesktopUI::checkingUserAgreement()
{
  this->Log->Status("Checking license agreement...");
}

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
#include <QTextEdit>

#include "midasStandardIncludes.h"

#include "mdoCommunity.h"
#include "mwsCommunity.h"
#include "mwsItem.h"
#include "mwsCollection.h"
#include "mdsBitstream.h"
#include "mwsNewResources.h"
#include "mwsSearch.h"
#include "mwsRestXMLParser.h"
#include "mdsCommunity.h"
#include "mdsCollection.h"
#include "mdsItem.h"
#include "mdsDatabaseAPI.h"
#include "midasAuthenticator.h"
#include "midasLog.h"
#include "midasSynchronizer.h"
#include "midasProgressReporter.h"

#include "GUIAgreement.h"
#include "GUIFileOverwriteHandler.h"
#include "GUILogger.h"
#include "GUIProgress.h"
#include "GUIMirrorHandler.h"
#include "GUIUpgradeHandler.h"
#include "Utils.h"
#include "ResourceEdit.h"
#include "ButtonDelegate.h"
#include "TextEditDelegate.h"

// ------------- Dialogs -------------
#include "AboutUI.h"
#include "AddKeywordUI.h"
#include "AddAuthorUI.h"
#include "AgreementUI.h"
#include "FileOverwriteUI.h"
#include "CreateMidasResourceUI.h"
#include "CreateProfileUI.h"
#include "DeleteResourceUI.h"
#include "PreferencesUI.h"
#include "UpgradeUI.h"
#include "PullUI.h"
#include "SignInUI.h"
#include "MirrorPickerUI.h"
// ------------- Dialogs -------------

// ------------- Threads -------------
#include "AddBitstreamsThread.h"
#include "DeleteThread.h"
#include "PollFilesystemThread.h"
#include "SearchThread.h"
#include "SynchronizerThread.h"
#include "UpdateTreeViewThread.h"
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
#include "TreeViewUpdateHandler.h"

#include <QDesktopWidget>

// ------------- TreeModel / TreeView -------------
MIDASDesktopUI::MIDASDesktopUI()
{
  setupUi(this); // this sets up GUI
  unsigned int currTime = static_cast<unsigned int>(kwsys::SystemTools::GetTime() * 1000);
  srand (currTime); //init random number generator
  this->setWindowTitle( STR2QSTR( MIDAS_CLIENT_VERSION_STR ) );

  // center the main window
  int scrn = QApplication::desktop()->screenNumber(this);
  QRect desk(QApplication::desktop()->availableGeometry(scrn));
  move((desk.width() - frameGeometry().width()) / 2,
      (desk.height() - frameGeometry().height()) / 2);
  // center the main window

  // ------------- Synchronizer --------------
  this->m_synch = new midasSynchronizer();
  this->Log = new GUILogger(this);
  // ------------- Synchronizer --------------

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
  dlg_createMidasResourceUI =  new CreateMidasResourceUI( this, m_synch );
  dlg_signInUI =               new SignInUI( this, m_synch );
  dlg_pullUI =                 new PullUI( this, m_synch );
  dlg_createProfileUI =        new CreateProfileUI( this );
  dlg_aboutUI =                new AboutUI( this );
  dlg_preferencesUI =          new PreferencesUI( this );
  dlg_deleteClientResourceUI = new DeleteResourceUI( this, false );
  dlg_deleteServerResourceUI = new DeleteResourceUI( this, true );
  dlg_addAuthorUI =            new AddAuthorUI( this );
  dlg_addKeywordUI =           new AddKeywordUI( this );
  dlg_agreementUI =            new AgreementUI( this );
  dlg_overwriteUI =            new FileOverwriteUI( this );
  dlg_mirrorPickerUI =         new MirrorPickerUI( this );
  dlg_upgradeUI =              new UpgradeUI( this );
  // ------------- Instantiate and setup UI dialogs -------------

  // ------------- Auto Refresh Timer -----------
  refreshTimer = new QTimer(this);
  connect(refreshTimer, SIGNAL( timeout() ), treeViewServer, SLOT( Update() ) );
  connect(dlg_preferencesUI, SIGNAL( intervalChanged() ), this, SLOT( setTimerInterval() ) );
  connect(dlg_preferencesUI, SIGNAL( settingChanged() ), this, SLOT( adjustTimerSettings() ) );
  // ------------- Auto Refresh Timer -----------

  // ------------- Item info panel -------------
  midasTreeItemInfoTable->horizontalHeader()->setStretchLastSection( false );
  midasTreeItemInfoTable->horizontalHeader()->hide();
  midasTreeItemInfoTable->verticalHeader()->hide();

  connect(midasTreeItemInfoTable, SIGNAL( itemChanged( QTableWidgetItem*) ), this, SLOT(
    resourceEdited(QTableWidgetItem*) ) );

  textMetadataEditor = new TextEditDelegate(this);

  authorsEditor = new ButtonDelegate(this);
  authorsEditor->setField(ITEM_AUTHORS);
  authorsEditor->setEditUI(dlg_addAuthorUI);
  
  keywordsEditor = new ButtonDelegate(this);
  keywordsEditor->setField(ITEM_KEYWORDS);
  keywordsEditor->setEditUI(dlg_addKeywordUI);
  // ------------- Item info panel -------------

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

  treeViewServer->SetSynchronizer(m_synch);
  treeViewClient->SetSynchronizer(m_synch);

  connect(treeViewServer, SIGNAL(midasTreeItemSelected(const MidasTreeItem*)),
    this, SLOT( updateActionState(const MidasTreeItem*) ));
  connect(treeViewClient, SIGNAL(midasTreeItemSelected(const MidasTreeItem*)),
    this, SLOT( updateActionStateClient(const MidasTreeItem*) ));

  connect(treeViewClient, SIGNAL(bitstreamsDropped(const MidasItemTreeItem*, const QStringList&)),
    this, SLOT( addBitstreams(const MidasItemTreeItem*, const QStringList&)));
  connect(treeViewClient, SIGNAL(resourceDropped(int, int)),
    this, SLOT( pullRecursive(int, int) ) );

  connect(treeViewClient, SIGNAL( bitstreamOpenRequest() ), this, SLOT( openBitstream() ) );

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

  connect(refreshClientButton, SIGNAL(released()), this, SLOT( updateClientTreeView() ) );

  connect(treeViewServer, SIGNAL(midasTreeViewContextMenu(QContextMenuEvent*)),
    this, SLOT( displayServerResourceContextMenu(QContextMenuEvent*) ));
  connect(treeViewClient, SIGNAL(midasTreeViewContextMenu(QContextMenuEvent*)),
    this, SLOT( displayClientResourceContextMenu(QContextMenuEvent*) ));

  connect(treeViewServer->model(), SIGNAL(serverPolled()), this, SLOT( storeLastPollTime()));

  connect(treeViewServer, SIGNAL( startedExpandingTree() ), this, SLOT( startedExpandingTree() ) );
  connect(treeViewServer, SIGNAL( finishedExpandingTree() ), this, SLOT( finishedExpandingTree() ) );

  connect(treeViewServer, SIGNAL( enableActions(bool) ), this, SLOT( enableActions(bool) ) );

  // ------------- setup TreeView signals -------------

  // ------------- signal/slot connections -------------
  connect( dlg_createProfileUI, SIGNAL( createdProfile(std::string, std::string, std::string, std::string, std::string, std::string)),
    this, SLOT( createProfile(std::string, std::string, std::string, std::string, std::string, std::string)));
  connect( dlg_createProfileUI, SIGNAL( deletedProfile(std::string)),
    dlg_signInUI, SLOT( removeProfile(std::string)));
  connect( dlg_createProfileUI, SIGNAL( deletedProfile(std::string) ),
    dynamic_cast<GUILogger*>(this->Log), SLOT( Status(std::string) ) );

  connect( dlg_signInUI, SIGNAL( createProfileRequest() ), dlg_createProfileUI, SLOT( exec() ) );
  connect( dlg_signInUI, SIGNAL( signingIn() ), this, SLOT( signingIn() ) );
  connect( dlg_signInUI, SIGNAL( signedIn(bool) ), this, SLOT( signIn(bool) ) );

  connect( dlg_deleteClientResourceUI, SIGNAL( deleteResource(bool) ), this, SLOT( deleteLocalResource(bool) ) );
  connect( dlg_deleteServerResourceUI, SIGNAL( deleteResource(bool) ), this, SLOT( deleteServerResource(bool) ) );

  connect( dlg_preferencesUI, SIGNAL( unifyingTree() ), this, SLOT( unifyingTree() ) );
  connect( dlg_preferencesUI, SIGNAL( treeUnified() ), this, SLOT( treeUnified() ) );

  connect( dlg_pullUI, SIGNAL( enableActions(bool) ), this, SLOT( enableActions(bool) ) );

  connect( dlg_createMidasResourceUI, SIGNAL( resourceCreated() ), this, SLOT( updateClientTreeView() ) );

  connect( actionPush_Resources,          SIGNAL( triggered() ), this, SLOT( pushResources() ) );
  connect( actionPull_Resource,           SIGNAL( triggered() ), dlg_pullUI, SLOT( exec() ) );
  connect( actionOpenURL,                 SIGNAL( triggered() ), this, SLOT( viewInBrowser() ) );
  //connect( actionSwap_with_MD5_reference, SIGNAL( triggered() ), this, SLOT( 

  connect( actionCreate_Profile, SIGNAL( triggered() ), dlg_createProfileUI, SLOT( exec() ) );

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
  m_AddBitstreamsThread = NULL;
  m_DeleteThread = NULL;
  connect(&m_CreateDBWatcher, SIGNAL(finished()), this, SLOT(newDBFinished()));
  // ------------- thread init -----------------

  // ------------- setup client members and logging ----
  this->m_synch->SetLog(this->Log);
  this->m_resourceUpdateHandler = new TreeViewUpdateHandler(treeViewClient);
  this->m_mirrorHandler = new GUIMirrorHandler(dlg_mirrorPickerUI);
  this->m_agreementHandler = new GUIAgreement(dlg_agreementUI);
  this->m_overwriteHandler = new GUIFileOverwriteHandler( dlg_overwriteUI );
  this->m_dbUpgradeHandler = new GUIUpgradeHandler( dlg_upgradeUI );
  this->m_dbUpgradeHandler->SetLog(this->Log);
  this->m_progress = new GUIProgress(this->progressBar);
  mds::DatabaseInfo::Instance()->SetLog(this->Log);
  mds::DatabaseInfo::Instance()->SetResourceUpdateHandler(m_resourceUpdateHandler);
  mws::WebAPI::Instance()->SetLog(this->Log);
  mws::WebAPI::Instance()->SetAuthenticator(m_synch->GetAuthenticator());
  mws::WebAPI::Instance()->SetMirrorHandler(m_mirrorHandler);
  this->m_synch->SetOverwriteHandler(this->m_overwriteHandler);
  this->m_synch->SetProgressReporter(m_progress);
  this->m_signIn = false;
  this->m_editMode = false;
  this->m_cancel = false;

  connect(dynamic_cast<GUIAgreement*>(m_agreementHandler), SIGNAL( errorMessage(const QString&) ),
          this, SLOT( logError(const QString&) ) );
  // ------------- setup handlers and logging -------------

  // ------------- Progress bar ------------------------
  connect(dynamic_cast<GUIProgress*>(m_progress), SIGNAL( ProgressMessage(const QString&) ), this, SLOT( currentFileMessage(const QString&) ) );
  connect(dynamic_cast<GUIProgress*>(m_progress), SIGNAL( OverallProgressCount(int, int) ), this, SLOT( overallProgressUpdate(int, int) ) );
  connect(dynamic_cast<GUIProgress*>(m_progress), SIGNAL( CurrentProgress(double, double) ), this, SLOT( currentProgressUpdate(double, double) ) );
  connect(dynamic_cast<GUIProgress*>(m_progress), SIGNAL( Speed(double) ), this, SLOT( progressSpeedUpdate(double) ) );
  connect(dynamic_cast<GUIProgress*>(m_progress), SIGNAL( EstimatedTime(double) ), this, SLOT( estimatedTimeUpdate(double) ) );
  connect(dynamic_cast<GUIProgress*>(m_progress), SIGNAL( OverallProgressTotal(double, double) ), this, SLOT( totalProgressUpdate(double, double) ) );
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
  if(m_SynchronizerThread && m_SynchronizerThread->isRunning())
    {
    m_SynchronizerThread->Cancel();
    m_SynchronizerThread->wait();
    }
  delete m_SynchronizerThread;
  delete dlg_pullUI;
  delete trayIconMenu;
  delete showAction;
  delete trayIcon;
  delete dlg_aboutUI;
  delete dlg_signInUI;
  delete dlg_createProfileUI;
  delete dlg_preferencesUI;
  delete dlg_createMidasResourceUI;
  delete dlg_addAuthorUI;
  delete dlg_addKeywordUI;
  delete dlg_agreementUI;
  delete dlg_overwriteUI;
  delete dlg_deleteClientResourceUI;
  delete dlg_deleteServerResourceUI;
  delete dlg_mirrorPickerUI;
  delete stateLabel;
  delete connectLabel;
  delete cancelButton;
  delete refreshTimer;
  delete Log;
  delete m_progress;
  delete m_synch;
  delete m_agreementHandler;
  delete m_overwriteHandler;
  delete m_mirrorHandler;
  if(m_RefreshThread && m_RefreshThread->isRunning())
    {
    m_RefreshThread->terminate();
    m_RefreshThread->wait();
    }
  delete m_RefreshThread;
  delete m_SearchThread;
  if(m_ReadDatabaseThread && m_ReadDatabaseThread->isRunning())
    {
    m_ReadDatabaseThread->terminate();
    m_ReadDatabaseThread->wait();
    }
  if(m_DeleteThread && m_DeleteThread->isRunning())
    {
    m_DeleteThread->terminate();
    m_DeleteThread->wait();
    }
  delete m_DeleteThread;

  /*if(m_PollFilesystemThread && m_PollFilesystemThread->isRunning())
    {
    m_PollFilesystemThread->Terminate();
    m_PollFilesystemThread->wait();
    }
  delete m_PollFilesystemThread;*/
  delete m_ReadDatabaseThread;
  delete authorsEditor;
  delete keywordsEditor;
  delete textMetadataEditor;
}

mds::ResourceUpdateHandler* MIDASDesktopUI::getResourceUpdateHandler()
{
  return m_resourceUpdateHandler;
}

void MIDASDesktopUI::showNormal()
{
  trayIcon->setIcon(QPixmap(":icons/MIDAS_Desktop_Icon.png"));

  if(mds::DatabaseInfo::Instance()->GetPath() != "")
    {
    mds::DatabaseAPI db;
    if(atoi(db.GetSetting(mds::DatabaseAPI::AUTO_REFRESH_SETTING).c_str()) == 0)
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
    this->refreshClientButton->setEnabled( value );
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
      mds::DatabaseAPI db;
      if(atoi(db.GetSetting(mds::DatabaseAPI::AUTO_REFRESH_SETTING).c_str()) == 0)
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

  m_ReadDatabaseThread = new UpdateTreeViewThread(this->treeViewClient);

  connect(m_ReadDatabaseThread, SIGNAL( finished() ), this, SLOT( resetStatus() ) );
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

  m_RefreshThread = new UpdateTreeViewThread(this->treeViewServer);
  
  connect(m_RefreshThread, SIGNAL( finished() ), this, SLOT( resetStatus() ) );
  connect(m_RefreshThread, SIGNAL( finished() ), this, SLOT( clearInfoPanel() ) );
  connect(m_RefreshThread, SIGNAL( enableActions(bool) ), this, SLOT( enableActions(bool) ) );

  displayStatus("Refreshing server tree...");
  setProgressIndeterminate();

  m_RefreshThread->start();
}

void MIDASDesktopUI::enableActions(bool val)
{
  this->activateActions(val, MIDASDesktopUI::ACTION_CONNECTED);
  this->cancelButton->setEnabled(!val);
  this->refreshClientButton->setEnabled(val);
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
  QTableWidgetDescriptionItem::Options options = QTableWidgetDescriptionItem::Tooltip | QTableWidgetDescriptionItem::AlignLeft;
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
  if(community->GetSize() != "" || communityTreeItem->isClientResource()) i++;

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
    textMetadataEditor->setField(COMMUNITY_DESCRIPTION);
    textMetadataEditor->setItem(communityTreeItem);
    midasTreeItemInfoTable->setItemDelegateForRow(i, textMetadataEditor);
    i++;
    }

  if(community->GetIntroductoryText() != "" || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Introductory", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCommunityDescItem(community, community->GetIntroductoryText().c_str(), COMMUNITY_INTRODUCTORY, options));
    textMetadataEditor->setField(COMMUNITY_INTRODUCTORY);
    textMetadataEditor->setItem(communityTreeItem);
    midasTreeItemInfoTable->setItemDelegateForRow(i, textMetadataEditor);
    i++;
    }
  
  if(community->GetCopyright() != "" || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Copyright", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCommunityDescItem(community, community->GetCopyright().c_str(), COMMUNITY_COPYRIGHT, options));
    textMetadataEditor->setField(COMMUNITY_COPYRIGHT);
    textMetadataEditor->setItem(communityTreeItem);
    midasTreeItemInfoTable->setItemDelegateForRow(i, textMetadataEditor);
    i++;
    }

  if(community->GetLinks() != "" || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Links", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCommunityDescItem(community, community->GetLinks().c_str(), COMMUNITY_LINKS, options));
    textMetadataEditor->setField(COMMUNITY_LINKS);
    textMetadataEditor->setItem(communityTreeItem);
    midasTreeItemInfoTable->setItemDelegateForRow(i, textMetadataEditor);
    i++;
    }

  if(community->GetSize() != "")
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Total Size", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCommunityDescItem(community, midasUtils::BytesToString(midasUtils::StringToDouble(community->GetSize())).c_str(), COMMUNITY_SIZE, QTableWidgetDescriptionItem::Tooltip));
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }
  else if(communityTreeItem->isClientResource())
    {
    mds::Community mdsComm;
    mdsComm.SetObject(community);
    mdsComm.FetchSize();
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Total Size", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCommunityDescItem(community, midasUtils::BytesToString(midasUtils::StringToDouble(community->GetSize())).c_str(), COMMUNITY_SIZE, QTableWidgetDescriptionItem::Tooltip));
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  midasTreeItemInfoTable->resizeColumnsToContents();
  int leftoverSpace = midasTreeItemInfoTable->width() - midasTreeItemInfoTable->columnWidth(0) - midasTreeItemInfoTable->columnWidth(1);
  midasTreeItemInfoTable->horizontalHeader()->setStretchLastSection(leftoverSpace > 0);
  midasTreeItemInfoTable->resizeColumnsToContents();
  midasTreeItemInfoTable->resizeRowsToContents();
}

void MIDASDesktopUI::infoPanel(MidasCollectionTreeItem* collectionTreeItem, bool edit)
{
  QTableWidgetDescriptionItem::Options options = QTableWidgetDescriptionItem::Tooltip | QTableWidgetDescriptionItem::AlignLeft;
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
  if(collection->GetSize() != "" || collectionTreeItem->isClientResource()) i++;
  
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
    textMetadataEditor->setItem(collectionTreeItem);
    textMetadataEditor->setField(COLLECTION_DESCRIPTION);
    midasTreeItemInfoTable->setItemDelegateForRow(i, textMetadataEditor);
    i++;
    }

  if(collection->GetIntroductoryText() != "" || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Introductory Text", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCollectionDescItem(collection, collection->GetIntroductoryText().c_str(), COLLECTION_INTRODUCTORY, options));
    textMetadataEditor->setItem(collectionTreeItem);
    textMetadataEditor->setField(COLLECTION_INTRODUCTORY);
    midasTreeItemInfoTable->setItemDelegateForRow(i, textMetadataEditor);
    i++;
    }

  if(collection->GetCopyright() != "" || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Copyright", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCollectionDescItem(collection, collection->GetCopyright().c_str(), COLLECTION_COPYRIGHT, options));
    textMetadataEditor->setItem(collectionTreeItem);
    textMetadataEditor->setField(COLLECTION_COPYRIGHT);
    midasTreeItemInfoTable->setItemDelegateForRow(i, textMetadataEditor);
    i++;
    }

  if(collection->GetSize() != "")
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Total Size", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCollectionDescItem(collection, midasUtils::BytesToString(midasUtils::StringToDouble(collection->GetSize())).c_str(), COLLECTION_SIZE, QTableWidgetDescriptionItem::Tooltip));
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }
  else if(collectionTreeItem->isClientResource())
    {
    mds::Collection mdsColl;
    mdsColl.SetObject(collection);
    mdsColl.FetchSize();
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Total Size", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCollectionDescItem(collection, midasUtils::BytesToString(midasUtils::StringToDouble(collection->GetSize())).c_str(), COLLECTION_SIZE, QTableWidgetDescriptionItem::Tooltip));
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  midasTreeItemInfoTable->resizeColumnsToContents();
  int leftoverSpace = midasTreeItemInfoTable->width() - midasTreeItemInfoTable->columnWidth(0) - midasTreeItemInfoTable->columnWidth(1);
  midasTreeItemInfoTable->horizontalHeader()->setStretchLastSection(leftoverSpace > 0);
  midasTreeItemInfoTable->resizeColumnsToContents();
  midasTreeItemInfoTable->resizeRowsToContents();
}

void MIDASDesktopUI::infoPanel(MidasItemTreeItem* itemTreeItem, bool edit)
{
  QTableWidgetDescriptionItem::Options options = QTableWidgetDescriptionItem::Tooltip | QTableWidgetDescriptionItem::AlignLeft;
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
  if(item->GetSize() != "" || itemTreeItem->isClientResource()) i++;

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
    textMetadataEditor->setField(ITEM_ABSTRACT);
    textMetadataEditor->setItem(itemTreeItem);
    midasTreeItemInfoTable->setItemDelegateForRow(i, textMetadataEditor);
    i++;
    }

  if(item->GetDescription() != "" || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Description", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasItemDescItem(item, item->GetDescription().c_str(), ITEM_DESCRIPTION, options));
    textMetadataEditor->setField(ITEM_DESCRIPTION);
    textMetadataEditor->setItem(itemTreeItem);
    midasTreeItemInfoTable->setItemDelegateForRow(i, textMetadataEditor);
    i++;
    }

  if(item->GetSize() != "")
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Total Size", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasItemDescItem(item, midasUtils::BytesToString(midasUtils::StringToDouble(item->GetSize())).c_str(), ITEM_SIZE, QTableWidgetDescriptionItem::Tooltip));
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }
  else if(itemTreeItem->isClientResource())
    {
    mds::Item mdsItem;
    mdsItem.SetObject(item);
    mdsItem.FetchSize();
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Total Size", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasItemDescItem(item, midasUtils::BytesToString(midasUtils::StringToDouble(item->GetSize())).c_str(), ITEM_SIZE, QTableWidgetDescriptionItem::Tooltip));
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  midasTreeItemInfoTable->resizeColumnsToContents();
  int leftoverSpace = midasTreeItemInfoTable->width() - midasTreeItemInfoTable->columnWidth(0) - midasTreeItemInfoTable->columnWidth(1);
  midasTreeItemInfoTable->horizontalHeader()->setStretchLastSection(leftoverSpace > 0);
  midasTreeItemInfoTable->resizeColumnsToContents();
  midasTreeItemInfoTable->resizeRowsToContents();
}

void MIDASDesktopUI::infoPanel(MidasBitstreamTreeItem* bitstreamTreeItem, bool edit)
{
  QTableWidgetDescriptionItem::Options options = QTableWidgetDescriptionItem::Tooltip | QTableWidgetDescriptionItem::AlignLeft;
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
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasBitstreamDescItem(bitstream, bitstream->GetPath().c_str(), BITSTREAM_PATH, QTableWidgetDescriptionItem::Tooltip | QTableWidgetDescriptionItem::AlignLeft));
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  midasTreeItemInfoTable->resizeColumnsToContents();
  int leftoverSpace = midasTreeItemInfoTable->width() - midasTreeItemInfoTable->columnWidth(0) - midasTreeItemInfoTable->columnWidth(1);
  midasTreeItemInfoTable->horizontalHeader()->setStretchLastSection(leftoverSpace > 0);
  midasTreeItemInfoTable->resizeColumnsToContents();
  midasTreeItemInfoTable->resizeRowsToContents();
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
  dlg_createMidasResourceUI->SetParentResource(NULL);
  dlg_createMidasResourceUI->exec();
}

void MIDASDesktopUI::addSubcommunity()
{
  dlg_createMidasResourceUI->SetType(CreateMidasResourceUI::SubCommunity);
  dlg_createMidasResourceUI->SetParentResource(treeViewClient->getSelectedMidasTreeItem());
  dlg_createMidasResourceUI->exec();
}

void MIDASDesktopUI::addCollection()
{
  dlg_createMidasResourceUI->SetType(CreateMidasResourceUI::Collection);
  dlg_createMidasResourceUI->SetParentResource(treeViewClient->getSelectedMidasTreeItem());
  dlg_createMidasResourceUI->exec();
}

void MIDASDesktopUI::addItem()
{
  dlg_createMidasResourceUI->SetType(CreateMidasResourceUI::Item);
  dlg_createMidasResourceUI->SetParentResource(treeViewClient->getSelectedMidasTreeItem());
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
  if(m_AddBitstreamsThread && m_AddBitstreamsThread->isRunning())
    {
    return; //already busy adding bitstreams
    }
  m_PollFilesystemThread->Pause();
  delete m_AddBitstreamsThread;
  m_AddBitstreamsThread = new AddBitstreamsThread;
  m_AddBitstreamsThread->SetFiles(files);
  m_AddBitstreamsThread->SetParentItem(
    const_cast<MidasItemTreeItem*>(parentItem));
  
  connect(m_AddBitstreamsThread, SIGNAL( finished() ),
          m_PollFilesystemThread, SLOT( Resume()) );
  connect(m_AddBitstreamsThread, SIGNAL( finished() ),
          this, SLOT( resetStatus() ) );
  connect(m_AddBitstreamsThread, SIGNAL(enableActions(bool)),
          this, SLOT( enableClientActions(bool) ) );
  connect(m_AddBitstreamsThread, SIGNAL(progress(int, int, const QString&)),
          this, SLOT(addBitstreamsProgress(int, int, const QString&)) );
  m_progress->ResetProgress();
  m_progress->ResetOverall();
  m_progress->SetUnit(" files");
  m_AddBitstreamsThread->start();
}

void MIDASDesktopUI::addBitstreamsProgress(int current, int total,
                                           const QString& message)
{
  m_progress->SetMessage(message.toStdString());
  m_progress->SetMaxCount(total);
  m_progress->SetMaxTotal(total);
  m_progress->UpdateProgress(current, total);
  m_progress->UpdateOverallCount(current);
  this->Log->Status("Adding bitstream " + message.toStdString());
}

void MIDASDesktopUI::viewDirectory()
{
  MidasTreeItem* resource = const_cast<MidasTreeItem*>(
    treeViewClient->getSelectedMidasTreeItem());
  mds::DatabaseAPI db;
  midasResourceRecord record = db.GetRecordByUuid(resource->getUuid());

  std::string path = record.Type == midasResourceType::BITSTREAM ?
    kwsys::SystemTools::GetParentDirectory(record.Path.c_str())
    : record.Path;

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

void MIDASDesktopUI::openBitstream()
{
  MidasTreeItem* resource = const_cast<MidasTreeItem*>(
    treeViewClient->getSelectedMidasTreeItem());
  mds::DatabaseAPI db;
  std::string path = db.GetRecordByUuid(resource->getUuid()).Path;

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
  std::string baseUrl = mws::WebAPI::Instance()->GetServerUrl();
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

void MIDASDesktopUI::signingIn()
{
  displayStatus("Connecting to server...");
  setProgressIndeterminate();
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
  mds::DatabaseAPI db;
  int minutes = atoi(db.GetSetting(mds::DatabaseAPI::AUTO_REFRESH_INTERVAL).c_str());
  refreshTimer->setInterval(minutes * 60 * 1000);
}

void MIDASDesktopUI::adjustTimerSettings()
{
  mds::DatabaseAPI db;
  int setting = atoi(db.GetSetting(mds::DatabaseAPI::AUTO_REFRESH_SETTING).c_str());

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
    mds::DatabaseAPI db;
    if(atoi(db.GetSetting(mds::DatabaseAPI::AUTO_REFRESH_SETTING).c_str()) == 1)
      {
      refreshTimer->start();
      }

    // Satus bar
    std::string connect = "  Connected to " + std::string(mws::WebAPI::Instance()->GetServerUrl()) + "  "; 
    connectLabel->setText( connect.c_str() );
    connectLabel->show();

    std::stringstream text;
    text << "Signed in with profile " << m_synch->GetAuthenticator()->GetProfile();
    GetLog()->Message(text.str());
    m_signIn = true;
    displayStatus(tr(""));
    this->treeViewServer->Initialize();
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
  if(file == "" || !midasUtils::IsDatabaseValid(file, m_dbUpgradeHandler))
    {
    std::stringstream text;
    text << file << " is not a valid MIDAS SQLite database. Defaulting "
      " to midas.db.";
    GetLog()->Message(text.str());
    std::string path = kwsys::SystemTools::GetCurrentWorkingDirectory()
      + "/midas.db";
    if(midasUtils::IsDatabaseValid(path, m_dbUpgradeHandler))
      {
      setLocalDatabase(path);
      return;
      }
    GetLog()->Error("No suitable database file found!");
    return;
    }

  if(midasUtils::IsDatabaseValid(file, m_dbUpgradeHandler))
    {
    mds::DatabaseInfo::Instance()->SetPath(file);
    QSettings settings("Kitware", "MIDASDesktop");
    settings.setValue("lastDatabase", file.c_str());
    settings.sync();
    this->displayStatus(tr("Opened database successfully."));
    this->Log->Message("Opened database " + file);
    this->activateActions(true, MIDASDesktopUI::ACTION_LOCAL_DATABASE);
    this->treeViewClient->collapseAll();
    this->updateClientTreeView();
    setTimerInterval();
    adjustTimerSettings();

    //start the filesystem monitoring thread
    m_PollFilesystemThread = new PollFilesystemThread;
    
    connect(m_PollFilesystemThread, SIGNAL(needToRefresh()), this, SLOT(updateClientTreeView()), Qt::BlockingQueuedConnection);
    connect(dlg_pullUI, SIGNAL( startingSynchronizer() ), m_PollFilesystemThread, SLOT( Pause() ), Qt::BlockingQueuedConnection );
    connect(dlg_pullUI, SIGNAL( pulledResources() ), m_PollFilesystemThread, SLOT( Resume() ) );

    m_PollFilesystemThread->start();
    }
  else
    {
    std::stringstream text;
    text << "The path " << file << " does not refer to a valid MIDASDesktop "
      "database.";
    GetLog()->Error(text.str());
    }
}

void MIDASDesktopUI::createProfile(std::string name, std::string email,
                                   std::string apiName, std::string apiKey,
                                   std::string rootDir, std::string url)
{
  if(mds::DatabaseInfo::Instance()->GetPath() == "")
    {
    this->displayStatus(tr("Please choose a local database first."));
    return;
    }

  mds::DatabaseAPI db;
  db.SetSetting(mds::DatabaseAPI::LAST_URL, url);
  std::string oldUrl = mws::WebAPI::Instance()->GetServerUrl();
  mws::WebAPI::Instance()->SetServerUrl(url.c_str());
  std::string msg;
  if(m_synch->GetAuthenticator()->AddAuthProfile(email, apiName, apiKey, rootDir, name))
    {
    msg = "Successfully created profile \"" + name + "\".";
    this->dlg_signInUI->profileCreated(name);
    }
  else
    {
    msg = "Failed to create authentication profile.";
    }
  mws::WebAPI::Instance()->SetServerUrl(oldUrl.c_str());
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

  m_synch->SetOperation(midasSynchronizer::OPERATION_PUSH);
  m_SynchronizerThread = new SynchronizerThread;
  m_SynchronizerThread->SetSynchronizer(m_synch);

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

  m_SearchThread = new SearchThread;
  m_SearchThread->SetWords(words);
  m_SearchThread->SetResults(&this->m_SearchResults);
  
  connect(m_SearchThread, SIGNAL( finished() ),
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
  this->treeViewServer->selectByUuid(listItem->getObject()->GetUuid(), true);
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
  mds::DatabaseAPI db;
  enableActions(false);
  mws::NewResources newResources;
  newResources.SetAuthenticator(m_synch->GetAuthenticator());
  newResources.SetSince(db.GetSetting(mds::DatabaseAPI::LAST_FETCH_TIME));
  newResources.Fetch();
  enableActions(true);

  this->m_dirtyUuids = newResources.GetUuids();
  std::reverse(m_dirtyUuids.begin(), m_dirtyUuids.end());

  if(m_dirtyUuids.size())
    {
    alertNewResources();
    }

  db.SetSetting(mds::DatabaseAPI::LAST_FETCH_TIME, newResources.GetTimestamp());

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
  this->treeViewServer->setFocus();
}

void MIDASDesktopUI::deleteLocalResource(bool deleteFiles)
{
  m_PollFilesystemThread->Pause();

  const MidasTreeItem* treeItem = treeViewClient->getSelectedMidasTreeItem();
  if(m_DeleteThread && m_DeleteThread->isRunning())
    {
    return;
    }
  delete m_DeleteThread;

  m_DeleteThread = new DeleteThread;
  m_DeleteThread->SetResource(const_cast<MidasTreeItem*>(treeItem));
  m_DeleteThread->SetDeleteOnDisk(deleteFiles);

  connect(m_DeleteThread, SIGNAL( finished() ), this, SLOT( resetStatus() ) );
  connect(m_DeleteThread, SIGNAL( finished() ), this, SLOT( updateClientTreeView() ) );
  connect(m_DeleteThread, SIGNAL( enableActions(bool) ), this, SLOT( enableClientActions(bool) ) );

  this->Log->Status("Deleting local resources...");
  setProgressIndeterminate();

  m_DeleteThread->start();
}

// Controller for deleting server resources
void MIDASDesktopUI::deleteServerResource(bool val)
{
  const MidasTreeItem* resource = this->treeViewServer->getSelectedMidasTreeItem();
  int id = resource->getId();
  std::string typeName = kwsys::SystemTools::LowerCase(midasUtils::GetTypeName(resource->getType()));

  std::stringstream text;
  if(mws::WebAPI::Instance()->DeleteResource(typeName, id))
    {
    text << "Successfully deleted " << typeName
         << " with id=" << id << " from the server.";
    this->Log->Message(text.str());
    this->updateServerTreeView();
    }
  else
    {
    text << "Failed to delete " << typeName << " with id=" << id
      << " from the server";
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
    ResourceEdit editor;
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
  text << "File Count: " << current << " / " << max << " files";
  this->fileCountLabel->setText(text.str().c_str());
}

void MIDASDesktopUI::currentProgressUpdate(double current, double max)
{
  std::string currentText = midasUtils::BytesToString(current, m_progress->GetUnit());
  std::string maxText = midasUtils::BytesToString(max, m_progress->GetUnit());
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

void MIDASDesktopUI::totalProgressUpdate(double current, double max)
{
  std::string currentText = midasUtils::BytesToString(current, m_progress->GetUnit());
  std::string maxText = midasUtils::BytesToString(max, m_progress->GetUnit());
  std::stringstream text;
  if(max == 0)
    {
    text << "Overall Progress: Calculating...";
    }
  else
    {
    text << "Overall Progress: " << currentText << " / " << maxText;
    }
  this->overallProgressLabel->setText(text.str().c_str());

  if (max == 0)
    {
    progressBar_overall->setMaximum(100);
    progressBar_overall->setValue(0);
    return;
    }
  double fraction = current / max;
  int percent = static_cast<int>(fraction * 100.0);
  progressBar_overall->setMaximum(100);
  progressBar_overall->setValue(percent);
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
    text << "Speed: " << midasUtils::BytesToString(bytesPerSec, m_progress->GetUnit())
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

void MIDASDesktopUI::logError(const QString& text)
{
  this->Log->Error(text.toStdString());
}

void MIDASDesktopUI::logMessage(const QString& text)
{
  this->Log->Message(text.toStdString());
}

void MIDASDesktopUI::unifyingTree()
{
  this->displayStatus("Copying resources into a single tree...");
  this->setProgressIndeterminate();
  m_PollFilesystemThread->Pause();
}

void MIDASDesktopUI::treeUnified()
{
  this->displayStatus("Finished unifying resources on disk.");
  this->Log->Message("Finished unifying resources on disk.");
  this->setProgressEmpty();
  this->updateClientTreeView();

  m_PollFilesystemThread->Resume();
}

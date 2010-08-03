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
#include "PreferencesUI.h"
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
  dlg_preferencesUI =      new PreferencesUI( this );
  dlg_pullUI =             new PullUI( this );
  dlg_deleteResourceUI =   new DeleteResourceUI( this );
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
  // ------------- Item info panel -------------

  saveButton = new QPushButton();
  saveButton->setText("Save");

  // ------------- Status bar -------------
  stateLabel    = new QLabel();
  progressBar   = new QProgressBar();
  connectLabel  = new QLabel();
  cancelButton  = new QPushButton();

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

  connect( actionCreate_Profile, SIGNAL( triggered() ), dlg_createProfileUI, SLOT( exec() ) );
  connect( dlg_createProfileUI, SIGNAL( createdProfile(std::string, std::string, std::string, std::string, std::string)),
    this, SLOT( createProfile(std::string, std::string, std::string, std::string, std::string)));
  connect( dlg_createProfileUI, SIGNAL( deletedProfile(std::string)),
    dlg_signInUI, SLOT( removeProfile(std::string)));
  connect( dlg_createProfileUI, SIGNAL( serverURLSet(std::string)), this, SLOT( setServerURL(std::string)));
  connect( dlg_signInUI, SIGNAL( createProfileRequest() ), dlg_createProfileUI, SLOT( exec() ) );
  connect( dlg_deleteResourceUI, SIGNAL( deleteResource(bool) ), this, SLOT( deleteLocalResource(bool) ) );

  connect( actionChoose_Local_Database, SIGNAL( triggered() ), this, SLOT( chooseLocalDatabase() ) );

  connect( actionSign_In,      SIGNAL( triggered() ), this, SLOT( signInOrOut() ) );
  connect( actionQuit,         SIGNAL( triggered() ), qApp, SLOT( quit() ) );
  connect( actionAbout,        SIGNAL( triggered() ), dlg_aboutUI, SLOT( exec() ) );
  connect( actionPreferences,  SIGNAL( triggered() ), dlg_preferencesUI, SLOT( exec() ) );

  connect( actionAdd_community,    SIGNAL(triggered()), this, SLOT(addCommunity()));
  connect( actionAdd_subcommunity, SIGNAL(triggered()), this, SLOT(addSubcommunity()));
  connect( actionAdd_collection,   SIGNAL(triggered()), this, SLOT(addCollection()));
  connect( actionAdd_item,         SIGNAL(triggered()), this, SLOT(addItem()));
  connect( actionAdd_bitstream,    SIGNAL(triggered()), this, SLOT(addBitstream()));
  connect( actionDelete_Resource,  SIGNAL(triggered()), dlg_deleteResourceUI, SLOT( exec() ) );
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
  // ------------- thread init -----------------

  // ------------- setup client members and logging ----
  this->m_database = NULL;
  this->m_synch = new midasSynchronizer();
  this->m_auth = new midasAuthenticator();
  this->m_progress = new GUIProgress(this->progressBar);
  this->Log = new GUILogger(this);
  this->m_auth->SetLog(Log);
  this->m_synch->SetLog(Log);
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
  delete dlg_preferencesUI;
  delete dlg_createMidasCommunityUI;
  delete dlg_createMidasSubCommunityUI;
  delete dlg_createMidasCollectionUI;
  delete dlg_createMidasItemUI;
  delete dlg_uploadAgreementUI;
  delete dlg_pullUI;
  delete stateLabel;
  delete connectLabel;
  delete cancelButton;
  delete saveButton;
  delete refreshTimer;
  delete m_database;
  delete m_auth;
  delete Log;
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
  // disable all actions
  this->activateActions( false, ACTION_ALL_CONNECTED );
  this->dlg_pullUI->setPullId(item->getId());
  this->dlg_pullUI->setResourceType(item->getType());

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
  this->treeViewClient->Update();
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

void MIDASDesktopUI::cancel()
{
  this->m_synch->Cancel();
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
  infoPanel(const_cast<MidasCommunityTreeItem*>(communityTreeItem), false);
}

void MIDASDesktopUI::updateInfoPanel( const MidasCollectionTreeItem* collectionTreeItem )
{
  infoPanel(const_cast<MidasCollectionTreeItem*>(collectionTreeItem), false);
}

void MIDASDesktopUI::updateInfoPanel( const MidasItemTreeItem* itemTreeItem )
{
  infoPanel(const_cast<MidasItemTreeItem*>(itemTreeItem), false);
}

void MIDASDesktopUI::updateInfoPanel( const MidasBitstreamTreeItem* bitstreamTreeItem )
{
  infoPanel(const_cast<MidasBitstreamTreeItem*>(bitstreamTreeItem), false);
}

/** Show the community information */
void MIDASDesktopUI::infoPanel(MidasCommunityTreeItem* communityTreeItem, bool edit)
{ 
  QTableWidgetDescriptionItem::Options options = QTableWidgetDescriptionItem::Tooltip;
  if(edit) options |= QTableWidgetDescriptionItem::Editable;

  midasTreeItemInfoGroupBox->setTitle(edit ? " Edit community info " : " Community description "); 
  midasTreeItemInfoTable->setGridStyle(edit ? Qt::DashDotLine : Qt::NoPen);
  midasTreeItemInfoTable->disconnect( SIGNAL( itemChanged ( QTableWidgetItem * ) ) );
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
    i++; 
    }

  if(community->GetDescription() != "" || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Description", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCommunityDescItem(community, community->GetDescription().c_str(), COMMUNITY_DESCRIPTION, options));
    i++;
    }

  if(community->GetIntroductoryText() != "" || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Introductory", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCommunityDescItem(community, community->GetIntroductoryText().c_str(), COMMUNITY_INTRODUCTORY, options));
    i++;
    }
  
  if(community->GetCopyright() != "" || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Copyright", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCommunityDescItem(community, community->GetCopyright().c_str(), COMMUNITY_COPYRIGHT, options));
    i++;
    }

  if(community->GetLinks() != "" || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Links", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCommunityDescItem(community, community->GetLinks().c_str(), COMMUNITY_LINKS, options));
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
  midasTreeItemInfoTable->disconnect( SIGNAL( itemChanged ( QTableWidgetItem * ) ) );
  midasTreeItemInfoTable->clearSelection();

  enableResourceEditing(collectionTreeItem->isClientResource() && !edit);

  int i = 0;
  if(collection->GetName() != "" || edit) i++;
  if(collection->GetDescription() != "" || edit) i++;
  
  midasTreeItemInfoTable->setRowCount( i );
  i = 0;

  if(collection->GetName() != "" || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Name", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCollectionDescItem(collection, collection->GetName().c_str(), COLLECTION_NAME, options));
    i++;
    }

  if(collection->GetDescription() != "" || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Description", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasCollectionDescItem(collection, collection->GetDescription().c_str(), COLLECTION_DESCRIPTION, options));
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
  midasTreeItemInfoTable->disconnect( SIGNAL( itemChanged ( QTableWidgetItem * ) ) ); 
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
    i++;
    }

  if(item->GetAuthors().size() || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Authors", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasItemDescItem(item, kwutils::concatenate(item->GetAuthors(), "/ ").c_str(), ITEM_AUTHORS, options));
    i++;
    }

  if(item->GetKeywords().size() || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Keywords", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasItemDescItem(item, kwutils::concatenate(item->GetKeywords(), "/ ").c_str(), ITEM_KEYWORDS, options));
    i++;
    }

  if(item->GetAbstract() != "" || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Abstract", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasItemDescItem(item, item->GetAbstract().c_str(), ITEM_ABSTRACT, options));
    i++;
    }

  if(item->GetDescription() != "" || edit)
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i,0,new QTableWidgetDescriptionItem("Description", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasItemDescItem(item, item->GetDescription().c_str(), ITEM_DESCRIPTION, options));
    i++;
    }

  midasTreeItemInfoTable->resizeColumnsToContents();
}

void MIDASDesktopUI::infoPanel(MidasBitstreamTreeItem* bitstreamTreeItem, bool edit)
{
  QTableWidgetDescriptionItem::Options options = QTableWidgetDescriptionItem::Tooltip;
  if(edit) options |= QTableWidgetDescriptionItem::Editable;

  enableResourceEditing(bitstreamTreeItem->isClientResource() && !edit);

  mdo::Bitstream* bitstream = bitstreamTreeItem->getBitstream();

  midasTreeItemInfoGroupBox->setTitle(tr(" Bitstream description "));
  midasTreeItemInfoTable->setGridStyle(edit ? Qt::DashDotLine : Qt::NoPen);
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
  midasTreeItemInfoTable->setItem(i,1,new QTableWidgetMidasBitstreamDescItem(bitstream, midasUtils::FileSizeString(atol(bitstream->GetSize().c_str())).c_str(), BITSTREAM_SIZE, QTableWidgetDescriptionItem::Tooltip));
  i++;

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
      // put any bitstream-only actions here
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

  if ( index.isValid() )
    {
    treeViewServer->selectionModel()->select( index, QItemSelectionModel::SelectCurrent ); 
    }
  else 
    {
    treeViewServer->selectionModel()->clearSelection(); 
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
    this->m_database->MarkDirtyResource(uuid, midasDirtyAction::ADDED);
    this->m_database->Close();

    if(id)
      {
      std::stringstream text;
      text << "Added bitstream " << name << " under item " << 
        parentItem->getItem()->GetTitle() << ".";
      this->GetLog()->Message(text.str());
      }
    }
  this->updateClientTreeView();
}

void MIDASDesktopUI::viewDirectory()
{
  MidasTreeItem* resource = const_cast<MidasTreeItem*>(
    treeViewClient->getSelectedMidasTreeItem());

  m_database->Open();
  std::string path = m_database->GetRecordByUuid(resource->getUuid()).Path;
  m_database->Close();

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
    text << "Failed to push resources to the server.";
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
  mws::NewResources newResources;
  m_database->Open();
  newResources.SetSince(m_database->GetSetting(midasDatabaseProxy::LAST_FETCH_TIME));
  newResources.Fetch();
  this->m_dirtyUuids = newResources.GetUuids();
  std::reverse(m_dirtyUuids.begin(), m_dirtyUuids.end());

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
  std::string uuid = this->treeViewClient->getSelectedMidasTreeItem()->getUuid();
  this->m_database->Open();
  this->m_database->DeleteResource(uuid, deleteFiles);
  this->m_database->Close();

  this->updateClientTreeView();
  std::stringstream text;
  text << "Deleted resource with uuid=" << uuid << ".";
  GetLog()->Message(text.str());
}

void MIDASDesktopUI::alertErrorInLog()
{
  if(this->logAndSearchTabContainer->currentIndex() != 1)
    {
    this->logAndSearchTabContainer->setTabIcon(1, QPixmap(":icons/exclamation.png"));
    }
}

void MIDASDesktopUI::clearLogTabIcon(int index)
{
  if(index == 1)
    {
    this->logAndSearchTabContainer->setTabIcon(1, QPixmap());
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
}

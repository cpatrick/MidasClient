#include "MIDASDesktopUI.h"

#include <QModelIndex>
#include <QItemSelection>
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QFileDialog>
#include <QFileInfo>
#include <QString>
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
#include "mwsRestResponseParser.h"
#include "mdsCommunity.h"
#include "mdsCollection.h"
#include "mdsItem.h"
#include "m3doFolder.h"
#include "m3doItem.h"
#include "m3doBitstream.h"
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
#include "ResourceEdit.h"
#include "ButtonDelegate.h"
#include "TextEditDelegate.h"
#include "IncompleteTransferWidget.h"

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
#include "PushUI.h"
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
#include "MidasTreeViewBase.h"
#include "MidasTreeItem.h"
#include "Midas3TreeItem.h"
#include "Midas3FolderTreeItem.h"
#include "Midas3ItemTreeItem.h"
#include "Midas3BitstreamTreeItem.h"
#include "MidasTreeViewServer.h"
#include "Midas3TreeViewServer.h"
#include "MidasTreeModelServer.h"
#include "MidasTreeViewClient.h"
#include "Midas3TreeViewClient.h"
#include "MidasTreeModelClient.h"
#include "MidasCommunityTreeItem.h"
#include "MidasCollectionTreeItem.h"
#include "MidasBitstreamTreeItem.h"
#include "MidasItemTreeItem.h"
#include "MidasClientGlobal.h"
#include "mwsWebAPI.h"
#include "TreeViewUpdateHandler.h"

#include <QDesktopWidget>

// ------------- TreeModel / TreeView -------------
MIDASDesktopUI::MIDASDesktopUI()
{
  setupUi(this); // this sets up GUI
  unsigned int currTime = static_cast<unsigned int>(midasUtils::CurrentTime() * 1000);
  srand(currTime); //init random number generator
  this->setWindowTitle(STR2QSTR(MIDAS_CLIENT_VERSION_STR));

  this->treeViewServer = NULL;
  this->treeViewClient = NULL;
  this->m_resourceUpdateHandler = NULL;

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
  trayIcon->setIcon(QPixmap(":icons/MIDAS_Desktop_LargeIcon.png"));
  trayIcon->setToolTip(STR2QSTR(MIDAS_CLIENT_VERSION_STR));
  trayIcon->setVisible(true);

  connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
    this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
  // ------------- Instantiate and setup tray icon -------------

  // ------------- Instantiate and setup UI dialogs -------------
  dlg_createMidasResourceUI =  new CreateMidasResourceUI(this, m_synch);
  dlg_signInUI =               new SignInUI(this, m_synch);
  dlg_pullUI =                 new PullUI(this, m_synch);
  dlg_pushUI =                 new PushUI(this, m_synch);
  dlg_createProfileUI =        new CreateProfileUI(this);
  dlg_aboutUI =                new AboutUI(this);
  dlg_preferencesUI =          new PreferencesUI(this);
  dlg_deleteClientResourceUI = new DeleteResourceUI(this, false);
  dlg_deleteServerResourceUI = new DeleteResourceUI(this, true);
  dlg_addAuthorUI =            new AddAuthorUI(this);
  dlg_addKeywordUI =           new AddKeywordUI(this);
  dlg_agreementUI =            new AgreementUI(this);
  dlg_overwriteUI =            new FileOverwriteUI(this);
  dlg_mirrorPickerUI =         new MirrorPickerUI(this);
  dlg_upgradeUI =              new UpgradeUI(this);
  // ------------- Instantiate and setup UI dialogs -------------

  // ------------- Incomplete transfer tab ----------------------
  transferWidget = new IncompleteTransferWidget(this, m_synch);
  incompleteTransfersTab->layout()->addWidget(transferWidget);

  connect(transferWidget, SIGNAL(ActivateActions(bool)),
          this, SLOT(enableActions(bool)));
  connect(transferWidget, SIGNAL(UploadComplete()),
          this, SLOT(updateServerTreeView()));
  connect(transferWidget, SIGNAL(DownloadStarted()),
          this, SLOT(showProgressTab()));
  connect(transferWidget, SIGNAL(UploadStarted()),
          this, SLOT(showProgressTab()));
  // ------------- Incomplete transfer tab ----------------------

  // ------------- Auto Refresh Timer -----------
  refreshTimer = new QTimer(this);
  
  connect(dlg_preferencesUI, SIGNAL(intervalChanged()), this, SLOT(setTimerInterval()));
  connect(dlg_preferencesUI, SIGNAL(settingChanged()), this, SLOT(adjustTimerSettings()));
  // ------------- Auto Refresh Timer -----------

  // ------------- Item info panel -------------
  midasTreeItemInfoTable->horizontalHeader()->setStretchLastSection(false);
  midasTreeItemInfoTable->horizontalHeader()->hide();
  midasTreeItemInfoTable->verticalHeader()->hide();

  connect(midasTreeItemInfoTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(
    resourceEdited(QTableWidgetItem*)));

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

  connectLabel->setAlignment(Qt::AlignCenter);
  connectLabel->setFrameShape(QFrame::Panel);
  connectLabel->setFrameShadow(QFrame::Sunken);
  connectLabel->setMinimumSize(connectLabel->sizeHint());
  connectLabel->clear();

  cancelButton->setText("Cancel");
  cancelButton->setIcon(QPixmap(":icons/delete2.png"));
  cancelButton->setEnabled(false);
  cancelButton->setMaximumHeight(25);

  statusBar()->addWidget(stateLabel, 1);
  statusBar()->addWidget(progressBar, 1);
  statusBar()->addWidget(cancelButton);
  statusBar()->addWidget(connectLabel);
  // ------------- Status bar -------------

  // ------------- signal/slot connections -------------
  connect(actionPush_Resource, SIGNAL(triggered()), this, SLOT(pushResources()));
  connect(actionPull_Resource, SIGNAL(triggered()), dlg_pullUI, SLOT(exec()));
  connect(actionOpenURL,       SIGNAL(triggered()), this, SLOT(viewInBrowser()));

  connect(actionCreate_Profile, SIGNAL(triggered()), dlg_createProfileUI, SLOT(exec()));

  connect(dlg_createProfileUI, SIGNAL(createdProfile(std::string, std::string, std::string, std::string, std::string, std::string)),
    this, SLOT(createProfile(std::string, std::string, std::string, std::string, std::string, std::string)));
  connect(dlg_createProfileUI, SIGNAL(deletedProfile(std::string)),
    dlg_signInUI, SLOT(removeProfile(std::string)));
  connect(dlg_createProfileUI, SIGNAL(deletedProfile(std::string)),
    dynamic_cast<GUILogger*>(this->Log), SLOT(Status(std::string)));

  connect(dlg_signInUI, SIGNAL(createProfileRequest()), dlg_createProfileUI, SLOT(exec()));
  connect(dlg_signInUI, SIGNAL(signingIn()), this, SLOT(signingIn()));
  connect(dlg_signInUI, SIGNAL(signedIn(bool)), this, SLOT(signIn(bool)));

  connect(dlg_deleteClientResourceUI, SIGNAL(deleteResource(bool)), this, SLOT(deleteLocalResource(bool)));
  connect(dlg_deleteServerResourceUI, SIGNAL(deleteResource(bool)), this, SLOT(deleteServerResource(bool)));
  connect(dlg_pushUI, SIGNAL(pushedResources(int)), this, SLOT(pushReturned(int)));
  connect(dlg_pushUI, SIGNAL(enableActions(bool)), this, SLOT(enableActions(bool)));

  connect(dlg_preferencesUI, SIGNAL(unifyingTree()), this, SLOT(unifyingTree()));
  connect(dlg_preferencesUI, SIGNAL(treeUnified()), this, SLOT(treeUnified()));

  connect(dlg_pullUI, SIGNAL(enableActions(bool)), this, SLOT(enableActions(bool)));

  //connect(dlg_createMidasResourceUI, SIGNAL(resourceCreated()), this, SLOT(updateClientTreeView()));

  connect(actionChoose_Local_Database, SIGNAL(triggered()), this, SLOT(chooseLocalDatabase()));
  connect(actionNew_Local_Database, SIGNAL(triggered()), this, SLOT(createLocalDatabase()));

  connect(actionSign_In,      SIGNAL(triggered()), this, SLOT(signInOrOut()));
  connect(actionQuit,         SIGNAL(triggered()), qApp, SLOT(quit()));
  connect(actionAbout,        SIGNAL(triggered()), dlg_aboutUI, SLOT(exec()));
  connect(actionPreferences,  SIGNAL(triggered()), dlg_preferencesUI, SLOT(exec()));

  connect(actionAdd_community,    SIGNAL(triggered()), this, SLOT(addCommunity()));
  connect(actionAdd_subcommunity, SIGNAL(triggered()), this, SLOT(addSubcommunity()));
  connect(actionAdd_collection,   SIGNAL(triggered()), this, SLOT(addCollection()));
  connect(actionAdd_item,         SIGNAL(triggered()), this, SLOT(addItem()));
  connect(actionAdd_bitstream,    SIGNAL(triggered()), this, SLOT(addBitstream()));
  connect(actionAdd_bitstream3,   SIGNAL(triggered()), this, SLOT(addBitstream()));
  connect(actionAdd_community3,   SIGNAL(triggered()), this, SLOT(addCommunity3()));
  connect(actionAdd_Top_Level_Folder, SIGNAL(triggered()), this, SLOT(addTopLevelFolder()));
  connect(actionAdd_Subfolder,    SIGNAL(triggered()), this, SLOT(addSubfolder()));
  connect(actionAdd_item3,        SIGNAL(triggered()), this, SLOT(addItem3()));
  connect(actionDelete_Resource,  SIGNAL(triggered()), dlg_deleteClientResourceUI, SLOT(exec()));
  connect(actionDelete_server,    SIGNAL(triggered()), dlg_deleteServerResourceUI, SLOT(exec()));
  connect(actionView_Directory,   SIGNAL(triggered()), this, SLOT(viewDirectory()));

  connect(searchItemsListWidget, SIGNAL(midasListWidgetItemClicked(QListWidgetItemMidasItem*)),
    this, SLOT(searchItemClicked(QListWidgetItemMidasItem*)));
  connect(searchItemsListWidget, SIGNAL(midasListWidgetContextMenu(QContextMenuEvent*)),
    this, SLOT(searchItemContextMenu(QContextMenuEvent*)));

  connect(push_Button,    SIGNAL(released()), this, SLOT(pushResources()));
  connect(pull_Button,    SIGNAL(released()), dlg_pullUI, SLOT(exec()));
  connect(refreshButton,  SIGNAL(released()), this, SLOT(updateServerTreeView()));
  connect(searchButton,   SIGNAL(released()), this, SLOT(search()));
  connect(cancelButton,   SIGNAL(released()), this, SLOT(cancel()));
  connect(editInfoButton, SIGNAL(released()), this, SLOT(editInfo()));
  connect(showNewResourcesButton, SIGNAL(released()), this, SLOT(decorateServerTree()));

  connect(log, SIGNAL(textChanged()), this, SLOT(showLogTab()));
  connect(logAndSearchTabContainer, SIGNAL(currentChanged(int)),
    this, SLOT(tabChanged(int)));

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
  this->m_mirrorHandler = new GUIMirrorHandler(dlg_mirrorPickerUI);
  this->m_agreementHandler = new GUIAgreement(dlg_agreementUI);
  this->m_overwriteHandler = new GUIFileOverwriteHandler(dlg_overwriteUI);
  this->m_dbUpgradeHandler = new GUIUpgradeHandler(dlg_upgradeUI);
  this->m_dbUpgradeHandler->SetLog(this->Log);
  this->m_progress = new GUIProgress(this->progressBar);
  mds::DatabaseInfo::Instance()->SetLog(this->Log);
  mws::WebAPI::Instance()->SetLog(this->Log);
  mws::WebAPI::Instance()->SetAuthenticator(m_synch->GetAuthenticator());
  mws::WebAPI::Instance()->SetMirrorHandler(m_mirrorHandler);
  this->m_synch->SetOverwriteHandler(this->m_overwriteHandler);
  this->m_synch->SetProgressReporter(m_progress);
  this->m_signIn = false;
  this->m_editMode = false;
  this->m_cancel = false;

  connect(dynamic_cast<GUIAgreement*>(m_agreementHandler), SIGNAL(errorMessage(const QString&)),
          this, SLOT(logError(const QString&)));
  // ------------- setup handlers and logging -------------

  // ------------- Progress bar ------------------------
  connect(dynamic_cast<GUIProgress*>(m_progress), SIGNAL(ProgressMessage(const QString&)), this, SLOT(currentFileMessage(const QString&)));
  connect(dynamic_cast<GUIProgress*>(m_progress), SIGNAL(OverallProgressCount(int, int)), this, SLOT(overallProgressUpdate(int, int)));
  connect(dynamic_cast<GUIProgress*>(m_progress), SIGNAL(CurrentProgress(double, double)), this, SLOT(currentProgressUpdate(double, double)));
  connect(dynamic_cast<GUIProgress*>(m_progress), SIGNAL(Speed(double)), this, SLOT(progressSpeedUpdate(double)));
  connect(dynamic_cast<GUIProgress*>(m_progress), SIGNAL(EstimatedTime(double)), this, SLOT(estimatedTimeUpdate(double)));
  connect(dynamic_cast<GUIProgress*>(m_progress), SIGNAL(OverallProgressTotal(double, double)), this, SLOT(totalProgressUpdate(double, double)));
  connect(dynamic_cast<GUIProgress*>(m_progress), SIGNAL(UpdateProgressMin(int)), progressBar_current, SLOT(setMinimum(int)));
  connect(dynamic_cast<GUIProgress*>(m_progress), SIGNAL(UpdateProgressMax(int)), progressBar_current, SLOT(setMaximum(int)));
  connect(dynamic_cast<GUIProgress*>(m_progress), SIGNAL(UpdateProgressValue(int)), progressBar_current, SLOT(setValue(int)));
  connect(dynamic_cast<GUIProgress*>(m_progress), SIGNAL(UpdateProgressMin(int)), progressBar, SLOT(setMinimum(int)));
  connect(dynamic_cast<GUIProgress*>(m_progress), SIGNAL(UpdateProgressMax(int)), progressBar, SLOT(setMaximum(int)));
  connect(dynamic_cast<GUIProgress*>(m_progress), SIGNAL(UpdateProgressValue(int)), progressBar, SLOT(setValue(int)));
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
  trayIcon->setIcon(QPixmap(":icons/MIDAS_Desktop_LargeIcon.png"));

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
  if(activateAction & ACTION_LOCAL_DATABASE)
    {
    this->treeViewClient->setEnabled(value);
    this->refreshClientButton->setEnabled(value);
    this->clientCollapseAllButton->setEnabled(value);
    this->clientExpandAllButton->setEnabled(value);
    if(DB_IS_MIDAS3)
      {
      this->actionAdd_community3->setEnabled(value);
      this->actionAdd_Top_Level_Folder->setEnabled(value);
      this->menuMIDAS_2->setEnabled(false);
      }
    else
      {
      this->actionAdd_community->setEnabled(value);
      this->menuMIDAS_3->setEnabled(false);
      }
    this->actionCreate_Profile->setEnabled(value);
    this->actionPreferences->setEnabled(value);
    this->midasTreeItemInfoGroupBox->setEnabled(value);
    }

  if(activateAction & ACTION_CONNECTED)
    {
    this->searchTab->setEnabled(value);
    this->treeViewServer->setEnabled(value);
    this->pull_Button->setEnabled(value);
    this->push_Button->setEnabled(value);
    this->actionPush_Resource->setEnabled(value);
    this->searchButton->setEnabled(value);
    this->searchQueryEdit->setEnabled(value);
    this->refreshButton->setEnabled(value);
    this->showNewResourcesButton->setEnabled(value);
    this->transferWidget->SetEnabled(value);
    actionSign_In->setText(value ? tr("Sign Out") : tr("Sign In"));
    }

  if(activateAction & ACTION_COMMUNITY)
    {
    this->actionPull_Resource->setEnabled(value);
    this->actionOpenURL->setEnabled(value);
    this->actionDelete_server->setEnabled(value);
    }

  if(activateAction & ACTION_COLLECTION)
    {
    this->actionPull_Resource->setEnabled(value);
    this->actionOpenURL->setEnabled(value);
    this->actionDelete_server->setEnabled(value);
    }

  if(activateAction & ACTION_ITEM)
    {
    this->actionPull_Resource->setEnabled(value);
    this->actionOpenURL->setEnabled(value);
    this->actionDelete_server->setEnabled(value);
    this->actionDownload_key_files_tgz->setEnabled(value);
    this->actionDownload_key_files_zip->setEnabled(value);
    }

  if(activateAction & ACTION_BITSTREAM)
    {
    this->actionPull_Resource->setEnabled(value);
    this->actionDelete_server->setEnabled(value);
    this->actionDownload_key_file->setEnabled(value);
    }

  if(activateAction & ACTION_CLIENT_COMMUNITY)
    {
    this->actionAdd_subcommunity->setEnabled(value);
    this->actionAdd_collection->setEnabled(value);
    }

  if(activateAction & ACTION_CLIENT_COLLECTION)
    {
    this->actionAdd_item->setEnabled(value);
    }

  if(activateAction & ACTION_CLIENT_ITEM)
    {
    this->actionAdd_bitstream->setEnabled(value);
    }

  if(activateAction & ACTION_CLIENT_BITSTREAM)
    {
    }

  if(activateAction & ACTION_CLIENT_RESOURCE)
    {
    this->actionDelete_Resource->setEnabled(value);
    this->actionView_Directory->setEnabled(value);
    }

  if(activateAction & ACTION_CLIENT_COMMUNITY3)
    {
    this->actionAdd_Subfolder->setEnabled(value);
    }

  if(activateAction & ACTION_CLIENT_FOLDER3)
    {
    this->actionAdd_Subfolder->setEnabled(value);
    this->actionAdd_item3->setEnabled(value);
    }

  if(activateAction & ACTION_CLIENT_ITEM3)
    {
    this->actionAdd_bitstream3->setEnabled(value);
    }

  if(activateAction & ACTION_CLIENT_RESOURCE3)
    {
    this->actionDelete_Resource->setEnabled(value);
    }
}

void MIDASDesktopUI::closeEvent(QCloseEvent* event)
{
  if(trayIcon->isVisible())
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
  switch(reason)
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

void MIDASDesktopUI::updateActionState(const MidasTreeItem* item)
{
  this->activateActions(false, ACTION_ALL_CONNECTED);
  this->dlg_pullUI->setPullId(item->getId());
  this->dlg_pullUI->setResourceType(item->getType());
  this->dlg_pullUI->setResourceName(item->data(0).toString().toStdString());

  if(item->getType() == midasResourceType::COMMUNITY)
    {
    this->activateActions(true, ACTION_COMMUNITY);
    }
  else if(item->getType() == midasResourceType::COLLECTION)
    {
    this->activateActions(true, ACTION_COLLECTION);
    }
  else if(item->getType() == midasResourceType::ITEM)
    {
    this->activateActions(true, ACTION_ITEM | ACTION_BITSTREAM);
    }
  else if(item->getType() == midasResourceType::BITSTREAM)
    {
    this->activateActions(true, ACTION_BITSTREAM);
    }
}

void MIDASDesktopUI::updateActionState(const Midas3TreeItem* item)
{
  this->activateActions(false, ACTION_ALL_CONNECTED);
  this->dlg_pullUI->setPullId(item->getId());
  this->dlg_pullUI->setResourceType(item->getType());
  this->dlg_pullUI->setResourceName(item->data(0).toString().toStdString());

  if(item->getType() == midas3ResourceType::COMMUNITY)
    {
    this->activateActions(true, ACTION_COMMUNITY3);
    }
  else if(item->getType() == midas3ResourceType::FOLDER)
    {
    this->activateActions(true, ACTION_FOLDER3);
    }
  else if(item->getType() == midas3ResourceType::ITEM)
    {
    this->activateActions(true, ACTION_ITEM3);
    }
}

void MIDASDesktopUI::updateActionStateClient(const MidasTreeItem* item)
{
  this->activateActions(false, ACTION_ALL_CONNECTED);
  this->activateActions(false, ACTION_CLIENT_COMMUNITY
                              | ACTION_CLIENT_COLLECTION
                              | ACTION_CLIENT_ITEM
                              | ACTION_CLIENT_BITSTREAM);

  if (item->getType() == midasResourceType::COMMUNITY)
    {
    this->activateActions(true, ACTION_CLIENT_COMMUNITY);
    }
  else if (item->getType() == midasResourceType::COLLECTION)
    {
    this->activateActions(true, ACTION_CLIENT_COLLECTION);
    }
  else if (item->getType() == midasResourceType::ITEM)
    {
    this->activateActions(true, ACTION_CLIENT_ITEM);
    }
  else if (item->getType() == midasResourceType::BITSTREAM)
    {
    this->activateActions(true, ACTION_CLIENT_BITSTREAM);
    }
}

void MIDASDesktopUI::updateActionStateClient(const Midas3TreeItem* item)
{
  this->activateActions(false, ACTION_ALL_CONNECTED);
  this->activateActions(false, ACTION_CLIENT_COMMUNITY3
                              | ACTION_CLIENT_FOLDER3
                              | ACTION_CLIENT_ITEM3);

  if(item->getType() == midas3ResourceType::COMMUNITY)
    {
    this->activateActions(true, ACTION_CLIENT_COMMUNITY3);
    }
  else if(item->getType() == midas3ResourceType::FOLDER)
    {
    this->activateActions(true, ACTION_CLIENT_FOLDER3);
    }
  else if(item->getType() == midas3ResourceType::ITEM)
    {
    this->activateActions(true, ACTION_CLIENT_ITEM3);
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

  connect(m_ReadDatabaseThread, SIGNAL(finished()), this, SLOT(resetStatus()));
  connect(m_ReadDatabaseThread, SIGNAL(enableActions(bool)), this, SLOT(enableClientActions(bool)));

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
  
  connect(m_RefreshThread, SIGNAL(finished()), this, SLOT(resetStatus()));
  connect(m_RefreshThread, SIGNAL(finished()), this, SLOT(clearInfoPanel()));
  connect(m_RefreshThread, SIGNAL(enableActions(bool)), this, SLOT(enableActions(bool)));

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
  m_synch->Cancel();
  if(m_SynchronizerThread)
    {
    m_SynchronizerThread->Cancel();
    }
  if(dlg_pullUI->getSynchronizerThread())
    {
    dlg_pullUI->getSynchronizerThread()->Cancel();
    }
  if(dlg_pushUI->getSynchronizerThread())
    {
    dlg_pushUI->getSynchronizerThread()->Cancel();
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
  disconnect(trayIcon, SIGNAL(messageClicked()), this, SLOT(showNormal()));
  trayIcon->showMessage(tr("MIDASDesktop - New Resources"),
    tr("There are new resources on the MIDAS server.  Click this message "
    "to show the MIDASDesktop window."));
  
  if(this->isHidden())
    {
    trayIcon->setIcon(QPixmap(":icons/MIDAS_Desktop_Icon_Red_v1.png"));
    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(showNormal()));
    }
}

void MIDASDesktopUI::showLogTab()
{
  // Put anything that should happen whenever new text appears in the log.
}

void MIDASDesktopUI::updateInfoPanel(const MidasCommunityTreeItem* communityTreeItem)
{
  this->m_editMode = false;
  infoPanel(const_cast<MidasCommunityTreeItem*>(communityTreeItem), false);
}

void MIDASDesktopUI::updateInfoPanel(const MidasCollectionTreeItem* collectionTreeItem)
{
  this->m_editMode = false;
  infoPanel(const_cast<MidasCollectionTreeItem*>(collectionTreeItem), false);
}

void MIDASDesktopUI::updateInfoPanel(const MidasItemTreeItem* itemTreeItem)
{
  this->m_editMode = false;
  infoPanel(const_cast<MidasItemTreeItem*>(itemTreeItem), false);
}

void MIDASDesktopUI::updateInfoPanel(const MidasBitstreamTreeItem* bitstreamTreeItem)
{
  this->m_editMode = false;
  infoPanel(const_cast<MidasBitstreamTreeItem*>(bitstreamTreeItem), false);
}

void MIDASDesktopUI::updateInfoPanel(const Midas3FolderTreeItem* folderTreeItem)
{
  this->m_editMode = false;
  QTableWidgetDescriptionItem::Options options = QTableWidgetDescriptionItem::Tooltip | QTableWidgetDescriptionItem::AlignLeft;
  bool isComm = folderTreeItem->getFolder()->GetResourceType() == midas3ResourceType::COMMUNITY;

  midasTreeItemInfoGroupBox->setTitle(isComm ? "Community information" : "Folder information"); 
  midasTreeItemInfoTable->setGridStyle(Qt::NoPen);
  midasTreeItemInfoTable->clearSelection();

  m3do::Folder* folder = folderTreeItem->getFolder();

  int i = 0;

  if(folder->GetName() != "") i++;
  if(folder->GetDescription() != "") i++;

  midasTreeItemInfoTable->setRowCount(i);
  i = 0; 
  
  if(folder->GetName() != "")
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i, 0, new QTableWidgetDescriptionItem("Name", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i, 1, new QTableWidgetMidas3FolderDescItem(folder, folder->GetName().c_str(), FOLDER3_NAME, options));
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++; 
    }

  if(folder->GetDescription() != "")
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i, 0, new QTableWidgetDescriptionItem("Description", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i, 1, new QTableWidgetMidas3FolderDescItem(folder, folder->GetDescription().c_str(), FOLDER3_DESCRIPTION, options));
    //textMetadataEditor->setField(FOLDER3_DESCRIPTION);
    //textMetadataEditor->setItem(folderTreeItem);
    //midasTreeItemInfoTable->setItemDelegateForRow(i, textMetadataEditor);
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  midasTreeItemInfoTable->resizeColumnsToContents();
  int leftoverSpace = midasTreeItemInfoTable->width() - midasTreeItemInfoTable->columnWidth(0) - midasTreeItemInfoTable->columnWidth(1);
  midasTreeItemInfoTable->horizontalHeader()->setStretchLastSection(leftoverSpace > 0);
  midasTreeItemInfoTable->resizeColumnsToContents();
  midasTreeItemInfoTable->resizeRowsToContents();
}

void MIDASDesktopUI::updateInfoPanel(const Midas3ItemTreeItem* itemTreeItem)
{
  this->m_editMode = false;
  QTableWidgetDescriptionItem::Options options = QTableWidgetDescriptionItem::Tooltip | QTableWidgetDescriptionItem::AlignLeft;

  midasTreeItemInfoGroupBox->setTitle("Item information"); 
  midasTreeItemInfoTable->setGridStyle(Qt::NoPen);
  midasTreeItemInfoTable->clearSelection();

  m3do::Item* item = itemTreeItem->getItem();

  int i = 0;

  if(item->GetName() != "") i++;
  if(item->GetDescription() != "") i++;

  midasTreeItemInfoTable->setRowCount(i);
  i = 0; 
  
  if(item->GetName() != "")
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i, 0, new QTableWidgetDescriptionItem("Name", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i, 1, new QTableWidgetMidas3ItemDescItem(item, item->GetName().c_str(), ITEM3_NAME, options));
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++; 
    }

  if(item->GetDescription() != "")
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i, 0, new QTableWidgetDescriptionItem("Description", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i, 1, new QTableWidgetMidas3ItemDescItem(item, item->GetDescription().c_str(), ITEM3_DESCRIPTION, options));
    //textMetadataEditor->setField(ITEM3_DESCRIPTION);
    //textMetadataEditor->setItem(itemTreeItem);
    //midasTreeItemInfoTable->setItemDelegateForRow(i, textMetadataEditor);
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  midasTreeItemInfoTable->resizeColumnsToContents();
  int leftoverSpace = midasTreeItemInfoTable->width() - midasTreeItemInfoTable->columnWidth(0) - midasTreeItemInfoTable->columnWidth(1);
  midasTreeItemInfoTable->horizontalHeader()->setStretchLastSection(leftoverSpace > 0);
  midasTreeItemInfoTable->resizeColumnsToContents();
  midasTreeItemInfoTable->resizeRowsToContents();
}

void MIDASDesktopUI::updateInfoPanel(const Midas3BitstreamTreeItem* bitstreamTreeItem)
{
  this->m_editMode = false;
  QTableWidgetDescriptionItem::Options options = QTableWidgetDescriptionItem::Tooltip | QTableWidgetDescriptionItem::AlignLeft;

  midasTreeItemInfoGroupBox->setTitle("Bitstream information"); 
  midasTreeItemInfoTable->setGridStyle(Qt::NoPen);
  midasTreeItemInfoTable->clearSelection();

  m3do::Bitstream* bitstream = bitstreamTreeItem->getBitstream();

  int i = 0;

  if(bitstream->GetName() != "") i++;
  if(bitstream->GetChecksum() != "") i++;
  if(bitstream->GetSize() != "") i++;
  if(bitstream->GetPath() != "") i++;

  midasTreeItemInfoTable->setRowCount(i);
  i = 0; 
  
  if(bitstream->GetName() != "")
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i, 0, new QTableWidgetDescriptionItem("Name", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i, 1, new QTableWidgetMidas3BitstreamDescItem(bitstream, bitstream->GetName().c_str(), BITSTREAM3_NAME, options));
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++; 
    }

  if(bitstream->GetChecksum() != "")
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i, 0, new QTableWidgetDescriptionItem("MD5", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i, 1, new QTableWidgetMidas3BitstreamDescItem(bitstream, bitstream->GetChecksum().c_str(), BITSTREAM3_CHECKSUM, options));
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  if(bitstream->GetSize() != "")
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i, 0, new QTableWidgetDescriptionItem("Size", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i, 1, new QTableWidgetMidas3BitstreamDescItem(bitstream, midasUtils::BytesToString(midasUtils::StringToDouble(bitstream->GetSize())).c_str(), BITSTREAM3_SIZE, options));
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  if(bitstream->GetPath() != "")
    {
    midasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    midasTreeItemInfoTable->setItem(i, 0, new QTableWidgetDescriptionItem("Path", QTableWidgetDescriptionItem::Bold));
    midasTreeItemInfoTable->setItem(i, 1, new QTableWidgetMidas3BitstreamDescItem(bitstream, bitstream->GetPath().c_str(), BITSTREAM3_PATH, options));
    midasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  midasTreeItemInfoTable->resizeColumnsToContents();
  int leftoverSpace = midasTreeItemInfoTable->width() - midasTreeItemInfoTable->columnWidth(0) - midasTreeItemInfoTable->columnWidth(1);
  midasTreeItemInfoTable->horizontalHeader()->setStretchLastSection(leftoverSpace > 0);
  midasTreeItemInfoTable->resizeColumnsToContents();
  midasTreeItemInfoTable->resizeRowsToContents();
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

  midasTreeItemInfoTable->setRowCount(i);
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
  
  midasTreeItemInfoTable->setRowCount(i);
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
  midasTreeItemInfoTable->setRowCount(0);
  enableResourceEditing(false);
}

void MIDASDesktopUI::displayClientResourceContextMenu(QContextMenuEvent* e)
{
  QMenu menu(this);
  QModelIndex index = treeViewClient->indexAt(e->pos());

  if(DB_IS_MIDAS3)
    {
    Midas3FolderTreeItem* folderTreeItem = NULL;
    Midas3ItemTreeItem* itemTreeItem = NULL;

    if(index.isValid())
      {
      Midas3TreeItem* item = const_cast<Midas3TreeItem*>(
        dynamic_cast<Midas3TreeViewClient*>(treeViewClient)->getSelectedMidasTreeItem());

      treeViewClient->selectionModel()->select(index, QItemSelectionModel::SelectCurrent); 

      menu.addAction(this->actionView_Directory);
      menu.addSeparator();

      if((folderTreeItem = dynamic_cast<Midas3FolderTreeItem*>(item)) != NULL)
        {
        menu.addAction(this->actionAdd_Subfolder);
        menu.addAction(this->actionAdd_item3);
        }
      else if((itemTreeItem = dynamic_cast<Midas3ItemTreeItem*>(item)) != NULL)
        {
        menu.addAction(this->actionAdd_bitstream3);
        }
      menu.addAction(this->actionDelete_Resource);
      }
    else
      {
      //treeViewServer->selectionModel()->clearSelection();
      menu.addAction(this->actionAdd_community3);
      menu.addAction(this->actionAdd_Top_Level_Folder);
      }
    }
  else
    {
    MidasCommunityTreeItem* communityTreeItem = NULL;
    MidasCollectionTreeItem* collectionTreeItem = NULL;
    MidasItemTreeItem* itemTreeItem = NULL;
    MidasBitstreamTreeItem* bitstreamTreeItem = NULL;
    
    if(index.isValid())
      {
      MidasTreeItem* item = const_cast<MidasTreeItem*>(dynamic_cast<MidasTreeViewClient*>(treeViewClient)->getSelectedMidasTreeItem());

      treeViewClient->selectionModel()->select(index, QItemSelectionModel::SelectCurrent); 

      menu.addAction(this->actionView_Directory);
      menu.addSeparator();

      if((communityTreeItem = dynamic_cast<MidasCommunityTreeItem*>(item)) != NULL)
        {
        menu.addAction(this->actionAdd_subcommunity);
        menu.addAction(this->actionAdd_collection);
        }
      else if((collectionTreeItem = dynamic_cast<MidasCollectionTreeItem*>(item)) != NULL)
        {
        menu.addAction(this->actionAdd_item);
        }
      else if((itemTreeItem = dynamic_cast<MidasItemTreeItem*>(item)) != NULL)
        {
        menu.addAction(this->actionAdd_bitstream);
        }
      else if((bitstreamTreeItem = dynamic_cast<MidasBitstreamTreeItem*>(item)) != NULL)
        {
        menu.addAction(this->actionSwap_with_MD5_reference);
        }
      menu.addAction(this->actionDelete_Resource);
      }
    else
      {
      treeViewServer->selectionModel()->clearSelection();
      menu.addAction(this->actionAdd_community);
      }
  }
  menu.addAction(this->actionPush_Resource);
  menu.exec(e->globalPos());
}

void MIDASDesktopUI::displayServerResourceContextMenu(QContextMenuEvent* e)
{
  QMenu menu(this);
  if(SERVER_IS_MIDAS3)
    {
    // TODO context menu in midas 3 server tree?
    Midas3TreeViewServer* treeView = dynamic_cast<Midas3TreeViewServer*>(this->treeViewServer);
    }
  else
    {
    MidasTreeViewServer* treeView = dynamic_cast<MidasTreeViewServer*>(this->treeViewServer);
    QModelIndex index = treeView->indexAt(e->pos());
    MidasItemTreeItem* itemTreeItem = NULL;
    MidasBitstreamTreeItem* bitstreamTreeItem = NULL;

    if(index.isValid())
      {
      MidasTreeItem* item = const_cast<MidasTreeItem*>(treeView->getSelectedMidasTreeItem());

      if(!item->resourceIsFetched())
        {
        return;
        }
      menu.addAction(this->actionPull_Resource);
      menu.addAction(this->actionOpenURL);
      menu.addAction(this->actionDelete_server);
      treeViewServer->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);

      if ((itemTreeItem = dynamic_cast<MidasItemTreeItem*>(item)) != NULL)
        {
        menu.addSeparator();
        menu.addAction(this->actionDownload_key_files_tgz);
        menu.addAction(this->actionDownload_key_files_zip);
        }
      else if ((bitstreamTreeItem = dynamic_cast<MidasBitstreamTreeItem*>(item)) != NULL)
        {
        menu.addSeparator();
        menu.addAction(this->actionDownload_key_file);
        }
      }
    else 
      {
      treeViewServer->selectionModel()->clearSelection();
      return;
      }
    }
  menu.exec(e->globalPos());
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
  dlg_createMidasResourceUI->SetParentResource(dynamic_cast<MidasTreeViewClient*>(treeViewClient)->getSelectedMidasTreeItem());
  dlg_createMidasResourceUI->exec();
}

void MIDASDesktopUI::addCollection()
{
  dlg_createMidasResourceUI->SetType(CreateMidasResourceUI::Collection);
  dlg_createMidasResourceUI->SetParentResource(dynamic_cast<MidasTreeViewClient*>(treeViewClient)->getSelectedMidasTreeItem());
  dlg_createMidasResourceUI->exec();
}

void MIDASDesktopUI::addItem()
{
  dlg_createMidasResourceUI->SetType(CreateMidasResourceUI::Item);
  dlg_createMidasResourceUI->SetParentResource(dynamic_cast<MidasTreeViewClient*>(treeViewClient)->getSelectedMidasTreeItem());
  dlg_createMidasResourceUI->exec();
}

void MIDASDesktopUI::addCommunity3()
{
  dlg_createMidasResourceUI->SetType(CreateMidasResourceUI::Community3);
  dlg_createMidasResourceUI->SetParentResource3(NULL);
  dlg_createMidasResourceUI->exec();
}

void MIDASDesktopUI::addTopLevelFolder()
{
  dlg_createMidasResourceUI->SetType(CreateMidasResourceUI::Folder);
  dlg_createMidasResourceUI->SetParentResource3(NULL);
  dlg_createMidasResourceUI->exec();
}

void MIDASDesktopUI::addSubfolder()
{
  dlg_createMidasResourceUI->SetType(CreateMidasResourceUI::SubFolder);
  dlg_createMidasResourceUI->SetParentResource3(dynamic_cast<Midas3TreeViewClient*>(treeViewClient)->getSelectedMidasTreeItem());
  dlg_createMidasResourceUI->exec();
}

void MIDASDesktopUI::addItem3()
{
  dlg_createMidasResourceUI->SetType(CreateMidasResourceUI::Item3);
  dlg_createMidasResourceUI->SetParentResource3(dynamic_cast<Midas3TreeViewClient*>(treeViewClient)->getSelectedMidasTreeItem());
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
    if(DB_IS_MIDAS3)
      {
      addBitstreams(reinterpret_cast<Midas3ItemTreeItem*>(
        const_cast<Midas3TreeItem*>(dynamic_cast<Midas3TreeViewClient*>(
        treeViewClient)->getSelectedMidasTreeItem())), files);
      }
    else
      {
      addBitstreams(reinterpret_cast<MidasItemTreeItem*>(
        const_cast<MidasTreeItem*>(
        dynamic_cast<MidasTreeViewClient*>(treeViewClient)->getSelectedMidasTreeItem())), files);
      }
    }
}

void MIDASDesktopUI::addBitstreams(const MidasItemTreeItem* parentItem,
                                   const QStringList& files)
{
  if(!this->addBitstreamsCommon(files))
    {
    return;
    }

  m_AddBitstreamsThread->SetParentItem(
    const_cast<MidasItemTreeItem*>(parentItem));
  m_AddBitstreamsThread->start();
}

void MIDASDesktopUI::addBitstreams(const Midas3ItemTreeItem* parentItem,
                                   const QStringList& files)
{
  if(!this->addBitstreamsCommon(files))
    {
    return;
    }

  m_AddBitstreamsThread->SetParentItem(
    const_cast<Midas3ItemTreeItem*>(parentItem));
  m_AddBitstreamsThread->start();
}

/**
 * Extracted the common code between MIDAS 2 and MIDAS 3 for adding
 * bitstreams to the client tree into this helper method
 */
bool MIDASDesktopUI::addBitstreamsCommon(const QStringList& files)
{
  if(m_AddBitstreamsThread && m_AddBitstreamsThread->isRunning())
    {
    return false; //already busy adding bitstreams
    }
  m_PollFilesystemThread->Pause();
  delete m_AddBitstreamsThread;
  m_AddBitstreamsThread = new AddBitstreamsThread;
  m_AddBitstreamsThread->SetFiles(files);
  
  connect(m_AddBitstreamsThread, SIGNAL(finished()),
          m_PollFilesystemThread, SLOT(Resume()));
  connect(m_AddBitstreamsThread, SIGNAL(finished()),
          this, SLOT(resetStatus()));
  connect(m_AddBitstreamsThread, SIGNAL(enableActions(bool)),
          this, SLOT(enableClientActions(bool)));
  connect(m_AddBitstreamsThread, SIGNAL(progress(int, int, const QString&)),
          this, SLOT(addBitstreamsProgress(int, int, const QString&)));
  m_progress->ResetProgress();
  m_progress->ResetOverall();
  m_progress->SetUnit(" files");
  return true;
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
    dynamic_cast<MidasTreeViewClient*>(treeViewClient)->getSelectedMidasTreeItem());
  mds::DatabaseAPI db;
  midasResourceRecord record = db.GetRecordByUuid(resource->getUuid());

  QFileInfo fileInfo(record.Path.c_str());
  std::string path = record.Type == midasResourceType::BITSTREAM ?
    fileInfo.dir().path().toStdString() : record.Path;

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
    dynamic_cast<MidasTreeViewClient*>(treeViewClient)->getSelectedMidasTreeItem());
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
  QString baseUrl = mws::WebAPI::Instance()->GetServerUrl();
  baseUrl = baseUrl.replace("/api/rest", "");
  std::stringstream path;
  path << baseUrl.toStdString();

  if(SERVER_IS_MIDAS3)
    {
    // TODO view in browser: MIDAS 3 version
    }
  else
    {
    MidasTreeItem* resource = const_cast<MidasTreeItem*>(
      dynamic_cast<MidasTreeViewServer*>(
      this->treeViewServer)
      ->getSelectedMidasTreeItem());
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
  if (!this->m_signIn)
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

    // start the refresh timer here if our setting = 1
    mds::DatabaseAPI db;
    if(atoi(db.GetSetting(mds::DatabaseAPI::AUTO_REFRESH_SETTING).c_str()) == 1)
      {
      refreshTimer->start();
      }

    // Satus bar
    std::string connectStr = "  Connected to " + std::string(mws::WebAPI::Instance()->GetServerUrl()) + "  "; 
    connectLabel->setText(connectStr.c_str());
    connectLabel->show();

    std::stringstream text;
    text << "Signed in with profile " << m_synch->GetAuthenticator()->GetProfile();
    GetLog()->Message(text.str());
    m_signIn = true;
    displayStatus(tr(""));

    delete this->treeViewServer;
    if(SERVER_IS_MIDAS3)
      {
      this->treeViewServer = new Midas3TreeViewServer(this);

      connect(treeViewServer, SIGNAL(midasTreeItemSelected(const Midas3TreeItem*)),
              this, SLOT(updateActionState(const Midas3TreeItem*)));
      connect(treeViewServer, SIGNAL(midas3FolderTreeItemSelected(const Midas3FolderTreeItem*)),
              this, SLOT(updateInfoPanel(const Midas3FolderTreeItem*)));
      connect(treeViewServer, SIGNAL(midas3ItemTreeItemSelected(const Midas3ItemTreeItem*)),
              this, SLOT(updateInfoPanel(const Midas3ItemTreeItem*)));
      connect(treeViewServer, SIGNAL(midas3BitstreamTreeItemSelected(const Midas3BitstreamTreeItem*)),
              this, SLOT(updateInfoPanel(const Midas3BitstreamTreeItem*)));
      }
    else
      {
      this->treeViewServer = new MidasTreeViewServer(this);

      connect(treeViewServer, SIGNAL(midasTreeItemSelected(const MidasTreeItem*)),
            this, SLOT(updateActionState(const MidasTreeItem*)));
      connect(treeViewServer, SIGNAL(midasCommunityTreeItemSelected(const MidasCommunityTreeItem*)),
            this, SLOT(updateInfoPanel(const MidasCommunityTreeItem*)));
      connect(treeViewServer, SIGNAL(midasCollectionTreeItemSelected(const MidasCollectionTreeItem*)),
              this, SLOT(updateInfoPanel(const MidasCollectionTreeItem*)));
      connect(treeViewServer, SIGNAL(midasItemTreeItemSelected(const MidasItemTreeItem*)),
              this, SLOT(updateInfoPanel(const MidasItemTreeItem*)));
      connect(treeViewServer, SIGNAL(midasBitstreamTreeItemSelected(const MidasBitstreamTreeItem*)),
            this, SLOT(updateInfoPanel(const MidasBitstreamTreeItem*)));
      }
    delete this->treeViewServerPlaceholder;
    this->treeViewServerPlaceholder = NULL;
    this->serverTreeViewContainer->layout()->addWidget(this->treeViewServer);
    activateActions(true, MIDASDesktopUI::ACTION_CONNECTED);
    treeViewServer->SetSynchronizer(m_synch);
    connect(refreshTimer, SIGNAL(timeout()), treeViewServer, SLOT(Update()));
    connect(treeViewServer, SIGNAL(resourceDropped(int, int)),
            this, SLOT(dragNDropPush(int, int)));
    connect(treeViewServer, SIGNAL(midasNoTreeItemSelected()),
            this, SLOT(clearInfoPanel()));
    connect(treeViewServer, SIGNAL(midasNoTreeItemSelected()),
            dlg_pullUI, SLOT(resetState()));
    connect(treeViewServer, SIGNAL(midasTreeViewContextMenu(QContextMenuEvent*)),
            this, SLOT(displayServerResourceContextMenu(QContextMenuEvent*)));
    connect(treeViewServer->model(), SIGNAL(serverPolled()), this, SLOT(storeLastPollTime()));

    connect(treeViewServer, SIGNAL(startedExpandingTree()), this, SLOT(startedExpandingTree()));
    connect(treeViewServer, SIGNAL(finishedExpandingTree()), this, SLOT(finishedExpandingTree()));

    connect(treeViewServer, SIGNAL(enableActions(bool)), this, SLOT(enableActions(bool)));
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

  QFileInfo fileInfo(file.c_str());
  if(fileInfo.exists())
    {
    std::stringstream text;
    text << "Error: " << file << " already exists.  Choose a new file name.";
    this->Log->Error(text.str());
    return;
    }

  if(file != "")
    {
    QMessageBox msgBox;
    msgBox.setText("Do you want to create a MIDAS 2 or MIDAS 3 database?");
    msgBox.setInformativeText("");
    QAbstractButton* midas2Button = msgBox.addButton("MIDAS 2", QMessageBox::YesRole);
    QAbstractButton* midas3Button = msgBox.addButton("MIDAS 3", QMessageBox::YesRole);
    QAbstractButton* cancelButton = msgBox.addButton("Cancel", QMessageBox::NoRole);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.exec();
 
    if(msgBox.clickedButton() != cancelButton)
      {
      this->Log->Message("Creating new local database at " + file);
      this->actionNew_Local_Database->setEnabled(false);
      this->setProgressIndeterminate();
      bool midas3 = msgBox.clickedButton() == midas3Button;
      QFuture<bool> future = QtConcurrent::run(midasUtils::CreateNewDatabase, file, midas3);
      m_CreateDBWatcher.setFuture(future);
      }
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
    std::string path = QDir::currentPath().toStdString() + "/midas.db";
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

    delete this->treeViewClient;
    if(DB_IS_MIDAS3)
      {
      this->treeViewClient = new Midas3TreeViewClient(this);

      connect(treeViewClient, SIGNAL(midas3FolderTreeItemSelected(const Midas3FolderTreeItem*)),
        this, SLOT(updateInfoPanel(const Midas3FolderTreeItem*)));
      connect(treeViewClient, SIGNAL(midas3ItemTreeItemSelected(const Midas3ItemTreeItem*)),
        this, SLOT(updateInfoPanel(const Midas3ItemTreeItem*)));
      connect(treeViewClient, SIGNAL(midas3BitstreamTreeItemSelected(const Midas3BitstreamTreeItem*)),
        this, SLOT(updateInfoPanel(const Midas3BitstreamTreeItem*)));
      connect(treeViewClient, SIGNAL(midasTreeItemSelected(const Midas3TreeItem*)),
        this, SLOT(updateActionStateClient(const Midas3TreeItem*)));
      connect(treeViewClient, SIGNAL(bitstreamsDropped(const Midas3ItemTreeItem*, const QStringList&)),
        this, SLOT(addBitstreams(const Midas3ItemTreeItem*, const QStringList&)));
      }
    else
      {
      this->treeViewClient = new MidasTreeViewClient(this);

      connect(treeViewClient, SIGNAL(midasCommunityTreeItemSelected(const MidasCommunityTreeItem*)),
        this, SLOT(updateInfoPanel(const MidasCommunityTreeItem*)));
      connect(treeViewClient, SIGNAL(midasCollectionTreeItemSelected(const MidasCollectionTreeItem*)),
        this, SLOT(updateInfoPanel(const MidasCollectionTreeItem*)));
      connect(treeViewClient, SIGNAL(midasItemTreeItemSelected(const MidasItemTreeItem*)),
        this, SLOT(updateInfoPanel(const MidasItemTreeItem*)));
      connect(treeViewClient, SIGNAL(midasBitstreamTreeItemSelected(const MidasBitstreamTreeItem*)),
        this, SLOT(updateInfoPanel(const MidasBitstreamTreeItem*)));
      connect(treeViewClient, SIGNAL(midasTreeItemSelected(const MidasTreeItem*)),
        this, SLOT(updateActionStateClient(const MidasTreeItem*)));

      connect(treeViewClient, SIGNAL(bitstreamsDropped(const MidasItemTreeItem*, const QStringList&)),
        this, SLOT(addBitstreams(const MidasItemTreeItem*, const QStringList&)));
      connect(treeViewClient, SIGNAL(bitstreamOpenRequest()), this, SLOT(openBitstream()));
      }
    delete m_resourceUpdateHandler;
    m_resourceUpdateHandler = new TreeViewUpdateHandler(treeViewClient);
    mds::DatabaseInfo::Instance()->SetResourceUpdateHandler(m_resourceUpdateHandler);
    delete this->treeViewClientPlaceholder;
    this->treeViewClientPlaceholder = NULL;
    this->clientTreeViewContainer->layout()->addWidget(this->treeViewClient);
    this->activateActions(true, MIDASDesktopUI::ACTION_LOCAL_DATABASE);

    treeViewClient->SetSynchronizer(m_synch);
    
    connect(treeViewClient, SIGNAL(resourceDropped(int, int)),
      this, SLOT(pullRecursive(int, int)));

    connect(treeViewClient, SIGNAL(midasNoTreeItemSelected()),
      this, SLOT(clearInfoPanel()));

    connect(refreshClientButton, SIGNAL(released()), this, SLOT(updateClientTreeView()));

    connect(treeViewClient, SIGNAL(midasTreeViewContextMenu(QContextMenuEvent*)),
      this, SLOT(displayClientResourceContextMenu(QContextMenuEvent*)));

    this->treeViewClient->collapseAll();
    this->updateClientTreeView();
    setTimerInterval();
    adjustTimerSettings();

    //start the filesystem monitoring thread
    m_PollFilesystemThread = new PollFilesystemThread;
    
    connect(m_PollFilesystemThread, SIGNAL(needToRefresh()), this, SLOT(updateClientTreeView()), Qt::BlockingQueuedConnection);
    connect(dlg_pullUI, SIGNAL(startingSynchronizer()), m_PollFilesystemThread, SLOT(Pause()));
    connect(dlg_pullUI, SIGNAL(pulledResources()), m_PollFilesystemThread, SLOT(Resume()));

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

void MIDASDesktopUI::createProfile(const std::string& name, const std::string& email,
                                   const std::string& apiName, const std::string& password,
                                   const std::string& rootDir, const std::string& url)
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
  if(m_synch->GetAuthenticator()->AddAuthProfile(email, apiName, password, rootDir, name))
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
  this->m_synch->GetAuthenticator()->ClearToken();
  mws::WebAPI::Instance()->Logout();
  m_signIn = false;

  refreshTimer->stop();
}

void MIDASDesktopUI::pushResources()
{
  this->displayStatus(tr("Pushing locally added resources..."));
  this->setProgressIndeterminate();

  const MidasTreeItem* resource =
    dynamic_cast<MidasTreeViewClient*>(treeViewClient)->getSelectedMidasTreeItem();
  dlg_pushUI->setObject(resource ? resource->getObject() : NULL);
  dlg_pushUI->exec();
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
  midasUtils::Tokenize(searchQueryEdit->text().toStdString(), words);

  if(m_SearchThread)
    {
    disconnect(m_SearchThread);
    }
  delete m_SearchThread;
  
  m_SearchResults.clear();

  m_SearchThread = new SearchThread;
  m_SearchThread->SetWords(words);
  m_SearchThread->SetResults(&this->m_SearchResults);
  
  connect(m_SearchThread, SIGNAL(finished()),
    this, SLOT(showSearchResults()));

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

void MIDASDesktopUI::searchItemClicked(QListWidgetItemMidasItem* listItem)
{
  // TODO MIDAS3ify
  dynamic_cast<MidasTreeViewServer*>(this->treeViewServer)
    ->selectByUuid(listItem->getObject()->GetUuid(), true);
}

void MIDASDesktopUI::searchItemContextMenu(QContextMenuEvent* e)
{
  QMenu menu(this);
  QModelIndex index = searchItemsListWidget->indexAt(e->pos());

  if (index.isValid())
    {
    menu.addAction(this->actionOpenURL);
    menu.addAction(this->actionPull_Resource);
    menu.exec(e->globalPos());
    }
}

void MIDASDesktopUI::storeLastPollTime()
{
  if(SERVER_IS_MIDAS3)
    {
    return; //no new resources call for MIDAS 3 yet.
    }
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
}

void MIDASDesktopUI::decorateServerTree()
{
  if(SERVER_IS_MIDAS3)
    {
    // TODO midas 3 server tree decoration
    }
  else
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
      connect(treeViewServer, SIGNAL(finishedExpandingTree()),
        this, SLOT(decorateCallback()));
      dynamic_cast<MidasTreeViewServer*>(this->treeViewServer)->selectByUuid(m_dirtyUuids[0]);
      }
    }
}

void MIDASDesktopUI::decorateCallback()
{
  if(SERVER_IS_MIDAS3)
    {
    dynamic_cast<Midas3TreeViewServer*>(this->treeViewServer)->decorateByUuid(m_dirtyUuids[0]);
    }
  else
    {
    dynamic_cast<MidasTreeViewServer*>(this->treeViewServer)->decorateByUuid(m_dirtyUuids[0]);
    }
  m_dirtyUuids.erase(m_dirtyUuids.begin());
  disconnect(treeViewServer, SIGNAL(finishedExpandingTree()),
    this, SLOT(decorateCallback()));
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
  if(m_DeleteThread && m_DeleteThread->isRunning())
    {
    return;
    }
  delete m_DeleteThread;
  m_DeleteThread = new DeleteThread;
  m_DeleteThread->SetDeleteOnDisk(deleteFiles);

  connect(m_DeleteThread, SIGNAL(enableActions(bool)), this, SLOT(enableClientActions(bool)));
  connect(m_DeleteThread, SIGNAL(finished()), this, SLOT(resetStatus()));
  connect(m_DeleteThread, SIGNAL(finished()), this, SLOT(updateClientTreeView()));

  if(DB_IS_MIDAS3)
    {
    const Midas3TreeItem* treeItem = dynamic_cast<Midas3TreeViewClient*>(treeViewClient)->getSelectedMidasTreeItem();
    m_DeleteThread->SetResource3(const_cast<Midas3TreeItem*>(treeItem));
    }
  else
    {
    const MidasTreeItem* treeItem = dynamic_cast<MidasTreeViewClient*>(treeViewClient)->getSelectedMidasTreeItem();
    m_DeleteThread->SetResource(const_cast<MidasTreeItem*>(treeItem));
    }

  this->Log->Status("Deleting local resources...");
  m_DeleteThread->start();
  setProgressIndeterminate();
}

// Controller for deleting server resources
void MIDASDesktopUI::deleteServerResource(bool val)
{
  if(SERVER_IS_MIDAS3)
    {
    // TODO delete server resource
    }
  else
    {
    const MidasTreeItem* resource = dynamic_cast<MidasTreeViewServer*>(this->treeViewServer)->getSelectedMidasTreeItem();
    int id = resource->getId();
    std::string typeName = QString(midasUtils::GetTypeName(resource->getType()).c_str()).toStdString();

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
}

void MIDASDesktopUI::alertErrorInLog()
{
  if(this->logAndSearchTabContainer->currentIndex() != MIDAS_TAB_LOG)
    {
    this->logAndSearchTabContainer->setTabIcon(
      MIDAS_TAB_LOG, QPixmap(":icons/exclamation.png"));
    }
}

void MIDASDesktopUI::tabChanged(int index)
{
  if(index == MIDAS_TAB_LOG) //log tab
    {
    this->logAndSearchTabContainer->setTabIcon(2, QPixmap());
    }
  else if(index == MIDAS_TAB_INCOMPLETE_TRANSFERS)
    {
    this->transferWidget->Populate();
    }
}

void MIDASDesktopUI::showProgressTab()
{
  this->logAndSearchTabContainer->setCurrentIndex(MIDAS_TAB_PROGRESS);
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

void MIDASDesktopUI::dragNDropPush(int type, int id)
{
  mdo::Community* comm = NULL;
  mdo::Collection* coll = NULL;
  mdo::Item* item = NULL;
  mdo::Bitstream* bitstream = NULL;
  mdo::Object* obj = NULL;

  mds::DatabaseAPI db;
  std::string uuid = db.GetUuid(type, id);

  switch(type)
    {
    case midasResourceType::COMMUNITY:
      comm = new mdo::Community;
      obj = comm;
      break;
    case midasResourceType::COLLECTION:
      coll = new mdo::Collection;
      obj = coll;
      break;
    case midasResourceType::ITEM:
      item = new mdo::Item;
      obj = item;
      break;
    case midasResourceType::BITSTREAM:
      bitstream = new mdo::Bitstream;
      obj = bitstream;
      break;
    default:
      return;
    }
  obj->SetId(id);
  obj->SetUuid(uuid.c_str());

  dlg_pushUI->setObject(obj);
  dlg_pushUI->setDelete(true); //will delete obj when done
  dlg_pushUI->exec();
}

void MIDASDesktopUI::enableResourceEditing(bool val)
{
  editInfoButton->setEnabled(val);
}

void MIDASDesktopUI::editInfo()
{
  MidasTreeItem* node = const_cast<MidasTreeItem*>(
    dynamic_cast<MidasTreeViewClient*>(treeViewClient)->getSelectedMidasTreeItem());

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
      SLOT(decorateByUuid(std::string)));

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
  progressBar->setMaximum(100);
  progressBar->setValue(percent);
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

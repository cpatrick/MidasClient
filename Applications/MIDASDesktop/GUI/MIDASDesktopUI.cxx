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
#include "mwsWebAPI.h"
#include "TreeViewUpdateHandler.h"

#include <QDesktopWidget>

// ------------- TreeModel / TreeView -------------
MIDASDesktopUI::MIDASDesktopUI()
{
  this->setupUi(this); // this sets up GUI
  unsigned int currTime = static_cast<unsigned int>(midasUtils::CurrentTime() * 1000);
  srand(currTime); // init random number generator
  this->setWindowTitle(QString("MIDAS Desktop ") + MIDAS_CLIENT_VERSION);

  m_TreeViewServer = NULL;
  m_TreeViewClient = NULL;
  m_ResourceUpdateHandler = NULL;

  // center the main window
  int   scrn = QApplication::desktop()->screenNumber(this);
  QRect desk(QApplication::desktop()->availableGeometry(scrn) );
  move( (desk.width() - frameGeometry().width() ) / 2,
        (desk.height() - frameGeometry().height() ) / 2);
  // center the main window

  // ------------- Synchronizer --------------
  m_Synch = new midasSynchronizer();
  this->Log = new GUILogger(this);
  // ------------- Synchronizer --------------

  // ------------- Instantiate and setup tray icon -------------
  m_ShowAction = new QAction(tr("&Show MIDASDesktop"), this);
  connect(m_ShowAction, SIGNAL(triggered() ), this, SLOT(showNormal() ) );

  m_TrayIconMenu = new QMenu(this);
  m_TrayIconMenu->addAction(m_ShowAction);
  m_TrayIconMenu->addSeparator();
  m_TrayIconMenu->addAction(m_ActionQuit);

  m_TrayIcon = new QSystemTrayIcon(this);
  m_TrayIcon->setContextMenu(m_TrayIconMenu);
  m_TrayIcon->setIcon(QPixmap(":icons/MIDAS_Desktop_LargeIcon.png") );
  m_TrayIcon->setToolTip(QString("MIDAS Desktop ") + MIDAS_CLIENT_VERSION );
  m_TrayIcon->setVisible(true);

  connect(m_TrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason) ),
          this, SLOT(IconActivated(QSystemTrayIcon::ActivationReason) ) );
  // ------------- Instantiate and setup tray icon -------------

  // ------------- Instantiate and setup UI dialogs -------------
  m_CreateMidasResourceUI =  new CreateMidasResourceUI(this, m_Synch);
  m_SignInUI =               new SignInUI(this, m_Synch);
  m_PullUI =                 new PullUI(this, m_Synch);
  m_PushUI =                 new PushUI(this, m_Synch);
  m_CreateProfileUI =        new CreateProfileUI(this);
  m_AboutUI =                new AboutUI(this);
  m_PreferencesUI =          new PreferencesUI(this);
  m_DeleteClientResourceUI = new DeleteResourceUI(this, false);
  m_DeleteServerResourceUI = new DeleteResourceUI(this, true);
  m_AddAuthorUI =            new AddAuthorUI(this);
  m_AddKeywordUI =           new AddKeywordUI(this);
  m_AgreementUI =            new AgreementUI(this);
  m_OverwriteUI =            new FileOverwriteUI(this);
  m_MirrorPickerUI =         new MirrorPickerUI(this);
  m_UpgradeUI =              new UpgradeUI(this);
  // ------------- Instantiate and setup UI dialogs -------------

  // ------------- Incomplete transfer tab ----------------------
  m_TransferWidget = new IncompleteTransferWidget(this, m_Synch);
  m_IncompleteTransfersTab->layout()->addWidget(m_TransferWidget);

  connect(m_TransferWidget, SIGNAL(ActivateActions(bool) ),
          this, SLOT(EnableActions(bool) ) );
  connect(m_TransferWidget, SIGNAL(UploadComplete() ),
          this, SLOT(UpdateServerTreeView() ) );
  connect(m_TransferWidget, SIGNAL(DownloadStarted() ),
          this, SLOT(ShowProgressTab() ) );
  connect(m_TransferWidget, SIGNAL(UploadStarted() ),
          this, SLOT(ShowProgressTab() ) );
  // ------------- Incomplete transfer tab ----------------------

  // ------------- Auto Refresh Timer -----------
  m_RefreshTimer = new QTimer(this);

  connect(m_PreferencesUI, SIGNAL(intervalChanged() ), this, SLOT(SetTimerInterval() ) );
  connect(m_PreferencesUI, SIGNAL(settingChanged() ), this, SLOT(AdjustTimerSettings() ) );
  // ------------- Auto Refresh Timer -----------

  // ------------- Item info panel -------------
  m_MidasTreeItemInfoTable->horizontalHeader()->setStretchLastSection(false);
  m_MidasTreeItemInfoTable->horizontalHeader()->hide();
  m_MidasTreeItemInfoTable->verticalHeader()->hide();

  connect(m_MidasTreeItemInfoTable, SIGNAL(itemChanged(QTableWidgetItem *) ), this, SLOT(
            ResourceEdited(QTableWidgetItem *) ) );

  m_TextMetadataEditor = new TextEditDelegate(this);

  m_AuthorsEditor = new ButtonDelegate(this);
  m_AuthorsEditor->setField(ITEM_AUTHORS);
  m_AuthorsEditor->setEditUI(m_AddAuthorUI);

  m_KeywordsEditor = new ButtonDelegate(this);
  m_KeywordsEditor->setField(ITEM_KEYWORDS);
  m_KeywordsEditor->setEditUI(m_AddKeywordUI);
  // ------------- Item info panel -------------

  // ------------- Status bar -------------
  m_StateLabel    = new QLabel();
  m_ProgressBar   = new QProgressBar();
  m_ConnectLabel  = new QLabel();
  m_CancelButton  = new QPushButton();

  m_StateLabel->setWordWrap(true);

  m_ProgressBar->setTextVisible(false);

  m_ConnectLabel->setAlignment(Qt::AlignCenter);
  m_ConnectLabel->setFrameShape(QFrame::Panel);
  m_ConnectLabel->setFrameShadow(QFrame::Sunken);
  m_ConnectLabel->setMinimumSize(m_ConnectLabel->sizeHint() );
  m_ConnectLabel->clear();

  m_CancelButton->setText("Cancel");
  m_CancelButton->setIcon(QPixmap(":icons/delete2.png") );
  m_CancelButton->setEnabled(false);
  m_CancelButton->setMaximumHeight(25);

  m_StatusBar->addWidget(m_StateLabel, 1);
  m_StatusBar->addWidget(m_ProgressBar, 1);
  m_StatusBar->addWidget(m_CancelButton);
  m_StatusBar->addWidget(m_ConnectLabel);
  // ------------- Status bar -------------

  // ------------- signal/slot connections -------------
  connect(m_ActionPushResource, SIGNAL(triggered() ), this, SLOT(PushResources() ) );
  connect(m_ActionPullResource, SIGNAL(triggered() ), m_PullUI, SLOT(exec() ) );
  connect(m_ActionOpenUrl,      SIGNAL(triggered() ), this, SLOT(ViewInBrowser() ) );

  connect(m_ActionCreateProfile, SIGNAL(triggered() ), m_CreateProfileUI, SLOT(exec() ) );

  connect(m_CreateProfileUI,
          SIGNAL(createdProfile(std::string, std::string, std::string, std::string, std::string, std::string) ),
          this, SLOT(CreateProfile(std::string, std::string, std::string, std::string, std::string, std::string) ) );
  connect(m_CreateProfileUI, SIGNAL(deletedProfile(std::string) ),
          m_SignInUI, SLOT(removeProfile(std::string) ) );
  connect(m_CreateProfileUI, SIGNAL(deletedProfile(std::string) ),
          dynamic_cast<GUILogger *>(this->Log), SLOT(Status(std::string) ) );

  connect(m_SignInUI, SIGNAL(createProfileRequest() ), m_CreateProfileUI, SLOT(exec() ) );
  connect(m_SignInUI, SIGNAL(signingIn() ), this, SLOT(SigningIn() ) );
  connect(m_SignInUI, SIGNAL(signedIn(bool) ), this, SLOT(SignIn(bool) ) );

  connect(m_DeleteClientResourceUI, SIGNAL(deleteResource(bool) ), this, SLOT(DeleteLocalResource(bool) ) );
  connect(m_DeleteServerResourceUI, SIGNAL(deleteResource(bool) ), this, SLOT(DeleteServerResource(bool) ) );
  connect(m_PushUI, SIGNAL(pushedResources(int) ), this, SLOT(PushReturned(int) ) );
  connect(m_PushUI, SIGNAL(enableActions(bool) ), this, SLOT(EnableActions(bool) ) );

  connect(m_PreferencesUI, SIGNAL(unifyingTree() ), this, SLOT(UnifyingTree() ) );
  connect(m_PreferencesUI, SIGNAL(treeUnified() ), this, SLOT(TreeUnified() ) );

  connect(m_PullUI, SIGNAL(enableActions(bool) ), this, SLOT(EnableActions(bool) ) );

  connect(m_ActionChooseLocalDatabase, SIGNAL(triggered() ), this, SLOT(ChooseLocalDatabase() ) );
  connect(m_ActionNewLocalDatabase, SIGNAL(triggered() ), this, SLOT(CreateLocalDatabase() ) );

  connect(m_ActionSignIn,      SIGNAL(triggered() ), this, SLOT(SignInOrOut() ) );
  connect(m_ActionQuit,        SIGNAL(triggered() ), qApp, SLOT(quit() ) );
  connect(m_ActionAbout,       SIGNAL(triggered() ), m_AboutUI, SLOT(exec() ) );
  connect(m_ActionPreferences, SIGNAL(triggered() ), m_PreferencesUI, SLOT(exec() ) );

  connect(m_ActionAddCommunity,    SIGNAL(triggered() ), this, SLOT(AddCommunity() ) );
  connect(m_ActionAddSubcommunity, SIGNAL(triggered() ), this, SLOT(AddSubcommunity() ) );
  connect(m_ActionAddCollection,   SIGNAL(triggered() ), this, SLOT(AddCollection() ) );
  connect(m_ActionAddItem,         SIGNAL(triggered() ), this, SLOT(AddItem() ) );
  connect(m_ActionAddBitstream,    SIGNAL(triggered() ), this, SLOT(AddBitstream() ) );
  connect(m_ActionAddBitstream3,   SIGNAL(triggered() ), this, SLOT(AddBitstream() ) );
  connect(m_ActionAddCommunity3,   SIGNAL(triggered() ), this, SLOT(AddCommunity3() ) );
  connect(m_ActionAddTopLevelFolder, SIGNAL(triggered() ), this, SLOT(AddTopLevelFolder() ) );
  connect(m_ActionAddSubfolder,    SIGNAL(triggered() ), this, SLOT(AddSubfolder() ) );
  connect(m_ActionAddItem3,        SIGNAL(triggered() ), this, SLOT(AddItem3() ) );
  connect(m_ActionDeleteResource,  SIGNAL(triggered() ), m_DeleteClientResourceUI, SLOT(exec() ) );
  connect(m_ActionDeleteServer,    SIGNAL(triggered() ), m_DeleteServerResourceUI, SLOT(exec() ) );
  connect(m_ActionViewDirectory,   SIGNAL(triggered() ), this, SLOT(ViewDirectory() ) );

  connect(m_SearchItemsListWidget, SIGNAL(midasListWidgetItemClicked(QListWidgetItemMidasItem *) ),
          this, SLOT(SearchItemClicked(QListWidgetItemMidasItem *) ) );
  connect(m_SearchItemsListWidget, SIGNAL(midasListWidgetContextMenu(QContextMenuEvent *) ),
          this, SLOT(SearchItemContextMenu(QContextMenuEvent *) ) );

  connect(m_PushButton,    SIGNAL(released() ), this, SLOT(PushResources() ) );
  connect(m_PullButton,    SIGNAL(released() ), m_PullUI, SLOT(exec() ) );
  connect(m_RefreshButton, SIGNAL(released() ), this, SLOT(UpdateServerTreeView() ) );
  connect(m_SearchButton,   SIGNAL(released() ), this, SLOT(Search() ) );
  connect(m_CancelButton,   SIGNAL(released() ), this, SLOT(Cancel() ) );
  connect(m_EditInfoButton, SIGNAL(released() ), this, SLOT(EditInfo() ) );
  connect(m_ShowNewResourcesButton, SIGNAL(released() ), this, SLOT(DecorateServerTree() ) );

  connect(m_Log, SIGNAL(textChanged() ), this, SLOT(ShowLogTab() ) );
  connect(m_LogAndSearchTabContainer, SIGNAL(currentChanged(int) ),
          this, SLOT(TabChanged(int) ) );

  // ------------- signal/slot connections -------------

  // ------------- thread init -----------------
  m_RefreshThread = NULL;
  m_SynchronizerThread = NULL;
  m_SearchThread = NULL;
  m_ReadDatabaseThread = NULL;
  m_PollFilesystemThread = NULL;
  m_AddBitstreamsThread = NULL;
  m_DeleteThread = NULL;
  connect(&m_CreateDBWatcher, SIGNAL(finished() ), this, SLOT(NewDatabaseFinished() ) );
  // ------------- thread init -----------------

  // ------------- setup client members and logging ----
  m_Synch->SetLog(this->Log);
  m_MirrorHandler = new GUIMirrorHandler(m_MirrorPickerUI);
  m_AgreementHandler = new GUIAgreement(m_AgreementUI);
  m_OverwriteHandler = new GUIFileOverwriteHandler(m_OverwriteUI);
  m_DatabaseUpgradeHandler = new GUIUpgradeHandler(m_UpgradeUI);
  m_DatabaseUpgradeHandler->SetLog(this->Log);
  m_Progress = new GUIProgress(m_ProgressBar);
  mds::DatabaseInfo::Instance()->SetLog(this->Log);
  mws::WebAPI::Instance()->SetLog(this->Log);
  mws::WebAPI::Instance()->SetAuthenticator(m_Synch->GetAuthenticator() );
  mws::WebAPI::Instance()->SetMirrorHandler(m_MirrorHandler);
  m_Synch->SetAgreementHandler(m_AgreementHandler);
  m_Synch->SetOverwriteHandler(m_OverwriteHandler);
  m_Synch->SetProgressReporter(m_Progress);
  m_SignIn = false;
  m_EditMode = false;
  m_Cancel = false;

  connect(dynamic_cast<GUIAgreement *>(m_AgreementHandler), SIGNAL(errorMessage(const QString &) ),
          this, SLOT(LogError(const QString &) ) );
  // ------------- setup handlers and logging -------------

  // ------------- Progress bar ------------------------
  connect(dynamic_cast<GUIProgress *>(m_Progress), SIGNAL(ProgressMessage(const QString &) ), this,
          SLOT(currentFileMessage(const QString &) ) );
  connect(dynamic_cast<GUIProgress *>(m_Progress),
          SIGNAL(OverallProgressCount(int, int) ), this, SLOT(OverallProgressUpdate(int, int) ) );
  connect(dynamic_cast<GUIProgress *>(m_Progress),
          SIGNAL(CurrentProgress(double, double) ), this, SLOT(CurrentProgressUpdate(double, double) ) );
  connect(dynamic_cast<GUIProgress *>(m_Progress), SIGNAL(Speed(double) ), this, SLOT(progressSpeedUpdate(double) ) );
  connect(dynamic_cast<GUIProgress *>(m_Progress), SIGNAL(EstimatedTime(double) ), this,
          SLOT(EstimatedTimeUpdate(double) ) );
  connect(dynamic_cast<GUIProgress *>(m_Progress),
          SIGNAL(OverallProgressTotal(double, double) ), this, SLOT(totalProgressUpdate(double, double) ) );
  connect(dynamic_cast<GUIProgress *>(m_Progress), SIGNAL(UpdateProgressMin(int) ), m_ProgressBarCurrent,
          SLOT(setMinimum(int) ) );
  connect(dynamic_cast<GUIProgress *>(m_Progress), SIGNAL(UpdateProgressMax(int) ), m_ProgressBarCurrent,
          SLOT(setMaximum(int) ) );
  connect(dynamic_cast<GUIProgress *>(m_Progress), SIGNAL(UpdateProgressValue(int) ), m_ProgressBarCurrent,
          SLOT(setValue(int) ) );
  connect(dynamic_cast<GUIProgress *>(m_Progress), SIGNAL(UpdateProgressMin(int) ), m_ProgressBar, SLOT(setMinimum(int) ) );
  connect(dynamic_cast<GUIProgress *>(m_Progress), SIGNAL(UpdateProgressMax(int) ), m_ProgressBar, SLOT(setMaximum(int) ) );
  connect(dynamic_cast<GUIProgress *>(m_Progress), SIGNAL(UpdateProgressValue(int) ), m_ProgressBar, SLOT(setValue(int) ) );
  // ------------- Progress bar ------------------------

  // ------------- Handle stored settings -------------
  QSettings   settings("Kitware", "MIDASDesktop");
  std::string lastDatabase =
    settings.value("lastDatabase", "").toString().toStdString();
  this->SetLocalDatabase(lastDatabase);
  // ------------- Handle stored settings -------------
}

/** Destructor */
MIDASDesktopUI::~MIDASDesktopUI()
{
  if( m_SynchronizerThread && m_SynchronizerThread->isRunning() )
    {
    m_SynchronizerThread->Cancel();
    m_SynchronizerThread->wait();
    }
  delete m_SynchronizerThread;
  delete m_PullUI;
  delete m_TrayIconMenu;
  delete m_ShowAction;
  delete m_TrayIcon;
  delete m_AboutUI;
  delete m_SignInUI;
  delete m_CreateProfileUI;
  delete m_PreferencesUI;
  delete m_CreateMidasResourceUI;
  delete m_AddAuthorUI;
  delete m_AddKeywordUI;
  delete m_AgreementUI;
  delete m_OverwriteUI;
  delete m_DeleteClientResourceUI;
  delete m_DeleteServerResourceUI;
  delete m_MirrorPickerUI;
  delete m_StateLabel;
  delete m_ConnectLabel;
  delete m_CancelButton;
  delete m_RefreshTimer;
  delete this->Log;
  delete m_Progress;
  delete m_Synch;
  delete m_AgreementHandler;
  delete m_OverwriteHandler;
  delete m_MirrorHandler;
  if( m_RefreshThread && m_RefreshThread->isRunning() )
    {
    m_RefreshThread->terminate();
    m_RefreshThread->wait();
    }
  delete m_RefreshThread;
  delete m_SearchThread;
  if( m_ReadDatabaseThread && m_ReadDatabaseThread->isRunning() )
    {
    m_ReadDatabaseThread->terminate();
    m_ReadDatabaseThread->wait();
    }
  if( m_DeleteThread && m_DeleteThread->isRunning() )
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
  delete m_AuthorsEditor;
  delete m_KeywordsEditor;
  delete m_TextMetadataEditor;
}

mds::ResourceUpdateHandler * MIDASDesktopUI::GetResourceUpdateHandler()
{
  return m_ResourceUpdateHandler;
}

void MIDASDesktopUI::showNormal()
{
  m_TrayIcon->setIcon(QPixmap(":icons/MIDAS_Desktop_LargeIcon.png") );

  if( mds::DatabaseInfo::Instance()->GetPath() != "" )
    {
    mds::DatabaseAPI db;
    if( atoi(db.GetSetting(mds::DatabaseAPI::AUTO_REFRESH_SETTING).c_str() ) == 0 )
      {
      m_RefreshTimer->stop();
      }
    }
  if( m_PollFilesystemThread )
    {
    m_PollFilesystemThread->Resume();
    }
  QMainWindow::showNormal();
  QMainWindow::activateWindow();
}

void MIDASDesktopUI::ActivateActions(bool value, UIActions activateAction)
{
  if( activateAction & ACTION_LOCAL_DATABASE )
    {
    m_TreeViewClient->setEnabled(value);
    m_RefreshClientButton->setEnabled(value);
    m_ClientCollapseAllButton->setEnabled(value);
    m_ClientExpandAllButton->setEnabled(value);
    if( DB_IS_MIDAS3 )
      {
      m_ActionAddCommunity3->setEnabled(value);
      m_ActionAddTopLevelFolder->setEnabled(value);
      m_MenuMIDAS2->setEnabled(false);
      }
    else
      {
      m_ActionAddCommunity->setEnabled(value);
      m_MenuMIDAS3->setEnabled(false);
      }
    m_ActionCreateProfile->setEnabled(value);
    m_ActionPreferences->setEnabled(value);
    m_MidasTreeItemInfoGroupBox->setEnabled(value);
    }

  if( activateAction & ACTION_CONNECTED )
    {
    m_SearchTab->setEnabled(value);
    m_TreeViewServer->setEnabled(value);
    m_PullButton->setEnabled(value);
    m_PushButton->setEnabled(value);
    m_ActionPushResource->setEnabled(value);
    m_SearchButton->setEnabled(value);
    m_SearchQueryEdit->setEnabled(value);
    m_RefreshButton->setEnabled(value);
    m_ShowNewResourcesButton->setEnabled(value);
    m_TransferWidget->SetEnabled(value);
    m_ActionSignIn->setText(value ? tr("Sign Out") : tr("Sign In") );
    }

  if( activateAction & ACTION_COMMUNITY )
    {
    m_ActionPullResource->setEnabled(value);
    m_ActionOpenUrl->setEnabled(value);
    m_ActionDeleteServer->setEnabled(value);
    }

  if( activateAction & ACTION_COLLECTION )
    {
    m_ActionPullResource->setEnabled(value);
    m_ActionOpenUrl->setEnabled(value);
    m_ActionDeleteServer->setEnabled(value);
    }

  if( activateAction & ACTION_ITEM )
    {
    m_ActionPullResource->setEnabled(value);
    m_ActionOpenUrl->setEnabled(value);
    m_ActionDeleteServer->setEnabled(value);
    m_ActionDownloadKeyFilesTgz->setEnabled(value);
    m_ActionDownloadKeyFilesZip->setEnabled(value);
    }

  if( activateAction & ACTION_BITSTREAM )
    {
    m_ActionPullResource->setEnabled(value);
    m_ActionDeleteServer->setEnabled(value);
    m_ActionDownloadKeyFile->setEnabled(value);
    }

  if( activateAction & ACTION_CLIENT_COMMUNITY )
    {
    m_ActionAddSubcommunity->setEnabled(value);
    m_ActionAddCollection->setEnabled(value);
    }

  if( activateAction & ACTION_CLIENT_COLLECTION )
    {
    m_ActionAddItem->setEnabled(value);
    }

  if( activateAction & ACTION_CLIENT_ITEM )
    {
    m_ActionAddBitstream->setEnabled(value);
    }

  if( activateAction & ACTION_CLIENT_BITSTREAM )
    {
    }

  if( activateAction & ACTION_CLIENT_RESOURCE )
    {
    m_ActionDeleteResource->setEnabled(value);
    m_ActionViewDirectory->setEnabled(value);
    }

  if( activateAction & ACTION_CLIENT_COMMUNITY3 )
    {
    m_ActionAddSubfolder->setEnabled(value);
    }

  if( activateAction & ACTION_CLIENT_FOLDER3 )
    {
    m_ActionAddSubfolder->setEnabled(value);
    m_ActionAddItem3->setEnabled(value);
    }

  if( activateAction & ACTION_CLIENT_ITEM3 )
    {
    m_ActionAddBitstream3->setEnabled(value);
    }

  if( activateAction & ACTION_CLIENT_RESOURCE3 )
    {
    m_ActionDeleteResource->setEnabled(value);
    m_ActionViewDirectory->setEnabled(value);
    }
}

void MIDASDesktopUI::closeEvent(QCloseEvent* event)
{
  if( m_TrayIcon->isVisible() )
    {
    m_TrayIcon->showMessage(tr("MIDASDesktop"),
                          tr("The program will keep running in the system tray.  To terminate "
                             "the program, choose Quit in the menu ") );
    this->hide();
    event->ignore();

    if( m_SignIn )
      {
      mds::DatabaseAPI db;
      if( atoi(db.GetSetting(mds::DatabaseAPI::AUTO_REFRESH_SETTING).c_str() ) == 0 )
        {
        m_RefreshTimer->start();
        }
      }
    }

  // stop filesystem polling (we don't care about updating the UI)
  if( m_PollFilesystemThread )
    {
    m_PollFilesystemThread->Pause();
    }
}

void MIDASDesktopUI::IconActivated(QSystemTrayIcon::ActivationReason reason)
{
  switch( reason )
    {
    case QSystemTrayIcon::Trigger:
      break;
    case QSystemTrayIcon::DoubleClick:
      this->showNormal();
      break;
    case QSystemTrayIcon::MiddleClick:
      break;
    default:
      break;
    }
}

void MIDASDesktopUI::UpdateActionState(const MidasTreeItem* item)
{
  this->ActivateActions(false, ACTION_ALL_CONNECTED);
  m_PullUI->setPullId(item->getId() );
  m_PullUI->setResourceType(item->getType() );
  m_PullUI->setResourceName(item->data(0).toString().toStdString() );

  if( item->getType() == midasResourceType::COMMUNITY )
    {
    this->ActivateActions(true, ACTION_COMMUNITY);
    }
  else if( item->getType() == midasResourceType::COLLECTION )
    {
    this->ActivateActions(true, ACTION_COLLECTION);
    }
  else if( item->getType() == midasResourceType::ITEM )
    {
    this->ActivateActions(true, ACTION_ITEM | ACTION_BITSTREAM);
    }
  else if( item->getType() == midasResourceType::BITSTREAM )
    {
    this->ActivateActions(true, ACTION_BITSTREAM);
    }
}

void MIDASDesktopUI::UpdateActionState(const Midas3TreeItem* item)
{
  this->ActivateActions(false, ACTION_ALL_CONNECTED);
  m_PullUI->setPullId(item->getId() );
  m_PullUI->setResourceType(item->getType() );
  m_PullUI->setResourceName(item->data(0).toString().toStdString() );

  if( item->getType() == midas3ResourceType::COMMUNITY )
    {
    this->ActivateActions(true, ACTION_COMMUNITY3);
    }
  else if( item->getType() == midas3ResourceType::FOLDER )
    {
    this->ActivateActions(true, ACTION_FOLDER3);
    }
  else if( item->getType() == midas3ResourceType::ITEM )
    {
    this->ActivateActions(true, ACTION_ITEM3);
    }
}

void MIDASDesktopUI::UpdateActionStateClient(const MidasTreeItem* item)
{
  this->ActivateActions(false, ACTION_ALL_CONNECTED);
  this->ActivateActions(false, ACTION_CLIENT_COMMUNITY
                        | ACTION_CLIENT_COLLECTION
                        | ACTION_CLIENT_ITEM
                        | ACTION_CLIENT_BITSTREAM);

  if( item->getType() == midasResourceType::COMMUNITY )
    {
    this->ActivateActions(true, ACTION_CLIENT_COMMUNITY);
    }
  else if( item->getType() == midasResourceType::COLLECTION )
    {
    this->ActivateActions(true, ACTION_CLIENT_COLLECTION);
    }
  else if( item->getType() == midasResourceType::ITEM )
    {
    this->ActivateActions(true, ACTION_CLIENT_ITEM);
    }
  else if( item->getType() == midasResourceType::BITSTREAM )
    {
    this->ActivateActions(true, ACTION_CLIENT_BITSTREAM);
    }
}

void MIDASDesktopUI::UpdateActionStateClient(const Midas3TreeItem* item)
{
  this->ActivateActions(false, ACTION_ALL_CONNECTED);
  this->ActivateActions(false, ACTION_CLIENT_COMMUNITY3
                        | ACTION_CLIENT_FOLDER3
                        | ACTION_CLIENT_ITEM3);

  if( item->getType() == midas3ResourceType::COMMUNITY )
    {
    this->ActivateActions(true, ACTION_CLIENT_COMMUNITY3);
    }
  else if( item->getType() == midas3ResourceType::FOLDER )
    {
    this->ActivateActions(true, ACTION_CLIENT_FOLDER3);
    }
  else if( item->getType() == midas3ResourceType::ITEM )
    {
    this->ActivateActions(true, ACTION_CLIENT_ITEM3);
    }
  else if( item->getType() == midas3ResourceType::BITSTREAM )
    {
    this->ActivateActions(true, ACTION_CLIENT_BITSTREAM3);
    }
}

void MIDASDesktopUI::UpdateClientTreeView()
{
  if( m_ReadDatabaseThread && m_ReadDatabaseThread->isRunning() )
    {
    return;
    }
  if( m_ReadDatabaseThread )
    {
    disconnect(m_ReadDatabaseThread);
    }
  delete m_ReadDatabaseThread;

  m_ReadDatabaseThread = new UpdateTreeViewThread(m_TreeViewClient);

  connect(m_ReadDatabaseThread, SIGNAL(finished() ), this, SLOT(resetStatus() ) );
  connect(m_ReadDatabaseThread, SIGNAL(enableActions(bool) ), this, SLOT(enableClientActions(bool) ) );

  this->DisplayStatus("Reading local database...");
  this->SetProgressIndeterminate();

  m_ReadDatabaseThread->start();
}

void MIDASDesktopUI::UpdateServerTreeView()
{
  if( m_RefreshThread )
    {
    disconnect(m_RefreshThread);
    }
  delete m_RefreshThread;

  m_RefreshThread = new UpdateTreeViewThread(m_TreeViewServer);

  connect(m_RefreshThread, SIGNAL(finished() ), this, SLOT(resetStatus() ) );
  connect(m_RefreshThread, SIGNAL(finished() ), this, SLOT(clearInfoPanel() ) );
  connect(m_RefreshThread, SIGNAL(enableActions(bool) ), this, SLOT(enableActions(bool) ) );

  this->DisplayStatus("Refreshing server tree...");
  this->SetProgressIndeterminate();

  m_RefreshThread->start();
}

void MIDASDesktopUI::EnableActions(bool val)
{
  this->ActivateActions(val, MIDASDesktopUI::ACTION_CONNECTED);
  m_CancelButton->setEnabled(!val);
  m_RefreshClientButton->setEnabled(val);
}

void MIDASDesktopUI::EnableClientActions(bool val)
{
  this->ActivateActions(val, MIDASDesktopUI::ACTION_LOCAL_DATABASE);
}

// Send cancel signals to any active push or pull operation
void MIDASDesktopUI::Cancel()
{
  m_Synch->Cancel();
  if( m_SynchronizerThread )
    {
    m_SynchronizerThread->Cancel();
    }
  if( m_PullUI->getSynchronizerThread() )
    {
    m_PullUI->getSynchronizerThread()->Cancel();
    }
  if( m_PushUI->getSynchronizerThread() )
    {
    m_PushUI->getSynchronizerThread()->Cancel();
    }
  if( m_DirtyUuids.size() )
    {
    m_Cancel = true;
    }
}

void MIDASDesktopUI::ResetStatus()
{
  this->SetProgressEmpty();
  this->DisplayStatus("");
}

void MIDASDesktopUI::AlertNewResources()
{
  disconnect(m_TrayIcon, SIGNAL(messageClicked() ), this, SLOT(showNormal() ) );
  m_TrayIcon->showMessage(tr("MIDASDesktop - New Resources"),
                        tr("There are new resources on the MIDAS server.  Click this message "
                           "to show the MIDASDesktop window.") );

  if( this->isHidden() )
    {
    m_TrayIcon->setIcon(QPixmap(":icons/MIDAS_Desktop_Icon_Red_v1.png") );
    connect(m_TrayIcon, SIGNAL(messageClicked() ), this, SLOT(showNormal() ) );
    }
}

void MIDASDesktopUI::ShowLogTab()
{
  // Put anything that should happen whenever new text appears in the log.
}

void MIDASDesktopUI::UpdateInfoPanel(const MidasCommunityTreeItem* communityTreeItem)
{
  m_EditMode = false;
  this->InfoPanel(const_cast<MidasCommunityTreeItem *>(communityTreeItem), false);
}

void MIDASDesktopUI::UpdateInfoPanel(const MidasCollectionTreeItem* collectionTreeItem)
{
  m_EditMode = false;
  this->InfoPanel(const_cast<MidasCollectionTreeItem *>(collectionTreeItem), false);
}

void MIDASDesktopUI::UpdateInfoPanel(const MidasItemTreeItem* itemTreeItem)
{
  m_EditMode = false;
  this->InfoPanel(const_cast<MidasItemTreeItem *>(itemTreeItem), false);
}

void MIDASDesktopUI::UpdateInfoPanel(const MidasBitstreamTreeItem* bitstreamTreeItem)
{
  m_EditMode = false;
  this->InfoPanel(const_cast<MidasBitstreamTreeItem *>(bitstreamTreeItem), false);
}

void MIDASDesktopUI::UpdateInfoPanel(const Midas3FolderTreeItem* folderTreeItem)
{
  m_EditMode = false;
  QTableWidgetDescriptionItem::Options options = QTableWidgetDescriptionItem::Tooltip
    | QTableWidgetDescriptionItem::AlignLeft;
  bool isComm = folderTreeItem->getFolder()->GetResourceType() ==
    midas3ResourceType::COMMUNITY;

  m_MidasTreeItemInfoGroupBox->setTitle(isComm ? "Community information" : "Folder information");
  m_MidasTreeItemInfoTable->setGridStyle(Qt::NoPen);
  m_MidasTreeItemInfoTable->clearSelection();

  m3do::Folder* folder = folderTreeItem->getFolder();

  int i = 0;

  if( folder->GetName() != "" )
    {
    i++;
    }
  if( folder->GetDescription() != "" )
    {
    i++;
    }

  m_MidasTreeItemInfoTable->setRowCount(i);
  i = 0;

  if( folder->GetName() != "" )
    {
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0, new QTableWidgetDescriptionItem("Name", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                      new QTableWidgetMidas3FolderDescItem(folder, folder->GetName().c_str(),
                                                                         FOLDER3_NAME, options) );
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  if( folder->GetDescription() != "" )
    {
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0,
                                    new QTableWidgetDescriptionItem("Description", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidas3FolderDescItem(folder, folder->GetDescription().c_str(),
                                                                         FOLDER3_DESCRIPTION, options) );
    // m_TextMetadataEditor->setField(FOLDER3_DESCRIPTION);
    // m_TextMetadataEditor->setItem(folderTreeItem);
    // m_MidasTreeItemInfoTable->setItemDelegateForRow(i, m_TextMetadataEditor);
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  m_MidasTreeItemInfoTable->resizeColumnsToContents();
  int leftoverSpace = m_MidasTreeItemInfoTable->width() - m_MidasTreeItemInfoTable->columnWidth(0)
    - m_MidasTreeItemInfoTable->columnWidth(1);
  m_MidasTreeItemInfoTable->horizontalHeader()->setStretchLastSection(leftoverSpace > 0);
  m_MidasTreeItemInfoTable->resizeColumnsToContents();
  m_MidasTreeItemInfoTable->resizeRowsToContents();
}

void MIDASDesktopUI::UpdateInfoPanel(const Midas3ItemTreeItem* itemTreeItem)
{
  m_EditMode = false;
  QTableWidgetDescriptionItem::Options options = QTableWidgetDescriptionItem::Tooltip
    | QTableWidgetDescriptionItem::AlignLeft;

  m_MidasTreeItemInfoGroupBox->setTitle("Item information");
  m_MidasTreeItemInfoTable->setGridStyle(Qt::NoPen);
  m_MidasTreeItemInfoTable->clearSelection();

  m3do::Item* item = itemTreeItem->getItem();

  int i = 0;

  if( item->GetName() != "" )
    {
    i++;
    }
  if( item->GetDescription() != "" )
    {
    i++;
    }

  m_MidasTreeItemInfoTable->setRowCount(i);
  i = 0;

  if( item->GetName() != "" )
    {
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0, new QTableWidgetDescriptionItem("Name", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidas3ItemDescItem(item, item->GetName().c_str(), ITEM3_NAME,
                                                                       options) );
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  if( item->GetDescription() != "" )
    {
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0,
                                    new QTableWidgetDescriptionItem("Description", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidas3ItemDescItem(item, item->GetDescription().c_str(),
                                                                       ITEM3_DESCRIPTION, options) );
    // m_TextMetadataEditor->setField(ITEM3_DESCRIPTION);
    // m_TextMetadataEditor->setItem(itemTreeItem);
    // m_MidasTreeItemInfoTable->setItemDelegateForRow(i, m_TextMetadataEditor);
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  m_MidasTreeItemInfoTable->resizeColumnsToContents();
  int leftoverSpace = m_MidasTreeItemInfoTable->width() - m_MidasTreeItemInfoTable->columnWidth(0)
    - m_MidasTreeItemInfoTable->columnWidth(1);
  m_MidasTreeItemInfoTable->horizontalHeader()->setStretchLastSection(leftoverSpace > 0);
  m_MidasTreeItemInfoTable->resizeColumnsToContents();
  m_MidasTreeItemInfoTable->resizeRowsToContents();
}

void MIDASDesktopUI::UpdateInfoPanel(const Midas3BitstreamTreeItem* bitstreamTreeItem)
{
  m_EditMode = false;
  QTableWidgetDescriptionItem::Options options = QTableWidgetDescriptionItem::Tooltip
    | QTableWidgetDescriptionItem::AlignLeft;

  m_MidasTreeItemInfoGroupBox->setTitle("Bitstream information");
  m_MidasTreeItemInfoTable->setGridStyle(Qt::NoPen);
  m_MidasTreeItemInfoTable->clearSelection();

  m3do::Bitstream* bitstream = bitstreamTreeItem->getBitstream();

  int i = 0;

  if( bitstream->GetName() != "" )
    {
    i++;
    }
  if( bitstream->GetChecksum() != "" )
    {
    i++;
    }
  if( bitstream->GetSize() != "" )
    {
    i++;
    }
  if( bitstream->GetPath() != "" )
    {
    i++;
    }

  m_MidasTreeItemInfoTable->setRowCount(i);
  i = 0;

  if( bitstream->GetName() != "" )
    {
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0, new QTableWidgetDescriptionItem("Name", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidas3BitstreamDescItem(bitstream, bitstream->GetName().c_str(),
                                                                            BITSTREAM3_NAME, options) );
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  if( bitstream->GetChecksum() != "" )
    {
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0, new QTableWidgetDescriptionItem("MD5", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidas3BitstreamDescItem(bitstream, bitstream->GetChecksum().c_str(),
                                                                            BITSTREAM3_CHECKSUM, options) );
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  if( bitstream->GetSize() != "" )
    {
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0, new QTableWidgetDescriptionItem("Size", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidas3BitstreamDescItem(bitstream,
                                                                            midasUtils::BytesToString(midasUtils::
                                                                                                      StringToDouble(
                                                                                                        bitstream->GetSize() ) ).c_str(), BITSTREAM3_SIZE, options) );
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  if( bitstream->GetPath() != "" )
    {
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0, new QTableWidgetDescriptionItem("Path", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidas3BitstreamDescItem(bitstream, bitstream->GetPath().c_str(),
                                                                            BITSTREAM3_PATH, options) );
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  m_MidasTreeItemInfoTable->resizeColumnsToContents();
  int leftoverSpace = m_MidasTreeItemInfoTable->width() - m_MidasTreeItemInfoTable->columnWidth(0)
    - m_MidasTreeItemInfoTable->columnWidth(1);
  m_MidasTreeItemInfoTable->horizontalHeader()->setStretchLastSection(leftoverSpace > 0);
  m_MidasTreeItemInfoTable->resizeColumnsToContents();
  m_MidasTreeItemInfoTable->resizeRowsToContents();
}

/** Show the community information */
void MIDASDesktopUI::InfoPanel(MidasCommunityTreeItem* communityTreeItem, bool edit)
{
  QTableWidgetDescriptionItem::Options options = QTableWidgetDescriptionItem::Tooltip
    | QTableWidgetDescriptionItem::AlignLeft;

  if( edit )
    {
    options |= QTableWidgetDescriptionItem::Editable;
    }

  m_MidasTreeItemInfoGroupBox->setTitle(edit ? " Edit community info " : " Community description ");
  m_MidasTreeItemInfoTable->setGridStyle(edit ? Qt::DashDotLine : Qt::NoPen);
  m_MidasTreeItemInfoTable->clearSelection();

  mdo::Community* community = communityTreeItem->getCommunity();

  this->EnableResourceEditing(communityTreeItem->isClientResource() && !edit);

  int i = 0;

  if( community->GetName() != "" || edit )
    {
    i++;
    }
  if( community->GetDescription() != "" || edit )
    {
    i++;
    }
  if( community->GetIntroductoryText() != "" || edit )
    {
    i++;
    }
  if( community->GetCopyright() != "" || edit )
    {
    i++;
    }
  if( community->GetLinks() != "" || edit )
    {
    i++;
    }
  if( community->GetSize() != "" || communityTreeItem->isClientResource() )
    {
    i++;
    }

  m_MidasTreeItemInfoTable->setRowCount(i);
  i = 0;

  if( community->GetName() != "" || edit )
    {
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0, new QTableWidgetDescriptionItem("Name", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidasCommunityDescItem(community, community->GetName().c_str(),
                                                                           COMMUNITY_NAME, options) );
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  if( community->GetDescription() != "" || edit )
    {
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0,
                                    new QTableWidgetDescriptionItem("Description", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidasCommunityDescItem(community, community->GetDescription().c_str(),
                                                                           COMMUNITY_DESCRIPTION,
                                                                           options) );
    m_TextMetadataEditor->setField(COMMUNITY_DESCRIPTION);
    m_TextMetadataEditor->setItem(communityTreeItem);
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, m_TextMetadataEditor);
    i++;
    }

  if( community->GetIntroductoryText() != "" || edit )
    {
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0,
                                    new QTableWidgetDescriptionItem("Introductory", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidasCommunityDescItem(community,
                                                                           community->GetIntroductoryText().c_str(),
                                                                           COMMUNITY_INTRODUCTORY,
                                                                           options) );
    m_TextMetadataEditor->setField(COMMUNITY_INTRODUCTORY);
    m_TextMetadataEditor->setItem(communityTreeItem);
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, m_TextMetadataEditor);
    i++;
    }

  if( community->GetCopyright() != "" || edit )
    {
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0,
                                    new QTableWidgetDescriptionItem("Copyright", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidasCommunityDescItem(community, community->GetCopyright().c_str(),
                                                                           COMMUNITY_COPYRIGHT, options) );
    m_TextMetadataEditor->setField(COMMUNITY_COPYRIGHT);
    m_TextMetadataEditor->setItem(communityTreeItem);
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, m_TextMetadataEditor);
    i++;
    }

  if( community->GetLinks() != "" || edit )
    {
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0, new QTableWidgetDescriptionItem("Links", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidasCommunityDescItem(community, community->GetLinks().c_str(),
                                                                           COMMUNITY_LINKS, options) );
    m_TextMetadataEditor->setField(COMMUNITY_LINKS);
    m_TextMetadataEditor->setItem(communityTreeItem);
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, m_TextMetadataEditor);
    i++;
    }

  if( community->GetSize() != "" )
    {
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0,
                                    new QTableWidgetDescriptionItem("Total Size", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidasCommunityDescItem(community,
                                                                           midasUtils::BytesToString(midasUtils::
                                                                                                     StringToDouble(
                                                                                                       community->GetSize() ) ).c_str(), COMMUNITY_SIZE,
                                                                           QTableWidgetDescriptionItem::Tooltip) );
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }
  else if( communityTreeItem->isClientResource() )
    {
    mds::Community mdsComm;
    mdsComm.SetObject(community);
    mdsComm.FetchSize();
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0,
                                    new QTableWidgetDescriptionItem("Total Size", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidasCommunityDescItem(community,
                                                                           midasUtils::BytesToString(midasUtils::
                                                                                                     StringToDouble(
                                                                                                       community->GetSize() ) ).c_str(), COMMUNITY_SIZE,
                                                                           QTableWidgetDescriptionItem::Tooltip) );
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  m_MidasTreeItemInfoTable->resizeColumnsToContents();
  int leftoverSpace = m_MidasTreeItemInfoTable->width() - m_MidasTreeItemInfoTable->columnWidth(0)
    - m_MidasTreeItemInfoTable->columnWidth(1);
  m_MidasTreeItemInfoTable->horizontalHeader()->setStretchLastSection(leftoverSpace > 0);
  m_MidasTreeItemInfoTable->resizeColumnsToContents();
  m_MidasTreeItemInfoTable->resizeRowsToContents();
}

void MIDASDesktopUI::InfoPanel(MidasCollectionTreeItem* collectionTreeItem, bool edit)
{
  QTableWidgetDescriptionItem::Options options = QTableWidgetDescriptionItem::Tooltip
    | QTableWidgetDescriptionItem::AlignLeft;

  if( edit )
    {
    options |= QTableWidgetDescriptionItem::Editable;
    }

  mdo::Collection* collection = collectionTreeItem->getCollection();

  m_MidasTreeItemInfoGroupBox->setTitle(edit ? " Edit collection info " : " Collection description ");
  m_MidasTreeItemInfoTable->setGridStyle(edit ? Qt::DashDotLine : Qt::NoPen);
  m_MidasTreeItemInfoTable->clearSelection();

  this->EnableResourceEditing(collectionTreeItem->isClientResource() && !edit);

  int i = 0;
  if( collection->GetName() != "" || edit )
    {
    i++;
    }
  if( collection->GetDescription() != "" || edit )
    {
    i++;
    }
  if( collection->GetCopyright() != "" || edit )
    {
    i++;
    }
  if( collection->GetIntroductoryText() != "" || edit )
    {
    i++;
    }
  if( collection->GetSize() != "" || collectionTreeItem->isClientResource() )
    {
    i++;
    }

  m_MidasTreeItemInfoTable->setRowCount(i);
  i = 0;

  if( collection->GetName() != "" || edit )
    {
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0, new QTableWidgetDescriptionItem("Name", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidasCollectionDescItem(collection, collection->GetName().c_str(),
                                                                            COLLECTION_NAME, options) );
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  if( collection->GetDescription() != "" || edit )
    {
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0,
                                    new QTableWidgetDescriptionItem("Description", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidasCollectionDescItem(collection,
                                                                            collection->GetDescription().c_str(),
                                                                            COLLECTION_DESCRIPTION,
                                                                            options) );
    m_TextMetadataEditor->setItem(collectionTreeItem);
    m_TextMetadataEditor->setField(COLLECTION_DESCRIPTION);
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, m_TextMetadataEditor);
    i++;
    }

  if( collection->GetIntroductoryText() != "" || edit )
    {
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0,
                                    new QTableWidgetDescriptionItem("Introductory Text",
                                                                    QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidasCollectionDescItem(collection,
                                                                            collection->GetIntroductoryText().c_str(),
                                                                            COLLECTION_INTRODUCTORY, options) );
    m_TextMetadataEditor->setItem(collectionTreeItem);
    m_TextMetadataEditor->setField(COLLECTION_INTRODUCTORY);
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, m_TextMetadataEditor);
    i++;
    }

  if( collection->GetCopyright() != "" || edit )
    {
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0,
                                    new QTableWidgetDescriptionItem("Copyright", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidasCollectionDescItem(collection,
                                                                            collection->GetCopyright().c_str(),
                                                                            COLLECTION_COPYRIGHT,
                                                                            options) );
    m_TextMetadataEditor->setItem(collectionTreeItem);
    m_TextMetadataEditor->setField(COLLECTION_COPYRIGHT);
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, m_TextMetadataEditor);
    i++;
    }

  if( collection->GetSize() != "" )
    {
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0,
                                    new QTableWidgetDescriptionItem("Total Size", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidasCollectionDescItem(collection,
                                                                            midasUtils::BytesToString(midasUtils::
                                                                                                      StringToDouble(
                                                                                                        collection->GetSize() ) ).c_str(), COLLECTION_SIZE,
                                                                            QTableWidgetDescriptionItem::Tooltip) );
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }
  else if( collectionTreeItem->isClientResource() )
    {
    mds::Collection mdsColl;
    mdsColl.SetObject(collection);
    mdsColl.FetchSize();
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0,
                                    new QTableWidgetDescriptionItem("Total Size", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidasCollectionDescItem(collection,
                                                                            midasUtils::BytesToString(midasUtils::
                                                                                                      StringToDouble(
                                                                                                        collection->GetSize() ) ).c_str(), COLLECTION_SIZE,
                                                                            QTableWidgetDescriptionItem::Tooltip) );
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  m_MidasTreeItemInfoTable->resizeColumnsToContents();
  int leftoverSpace = m_MidasTreeItemInfoTable->width() - m_MidasTreeItemInfoTable->columnWidth(0)
    - m_MidasTreeItemInfoTable->columnWidth(1);
  m_MidasTreeItemInfoTable->horizontalHeader()->setStretchLastSection(leftoverSpace > 0);
  m_MidasTreeItemInfoTable->resizeColumnsToContents();
  m_MidasTreeItemInfoTable->resizeRowsToContents();
}

void MIDASDesktopUI::InfoPanel(MidasItemTreeItem* itemTreeItem, bool edit)
{
  QTableWidgetDescriptionItem::Options options = QTableWidgetDescriptionItem::Tooltip
    | QTableWidgetDescriptionItem::AlignLeft;

  if( edit )
    {
    options |= QTableWidgetDescriptionItem::Editable;
    }

  mdo::Item* item = itemTreeItem->getItem();

  m_MidasTreeItemInfoGroupBox->setTitle(edit ? " Edit item info " : " Item description ");
  m_MidasTreeItemInfoTable->setGridStyle(edit ? Qt::DashDotLine : Qt::NoPen);
  m_MidasTreeItemInfoTable->clearSelection();

  this->EnableResourceEditing(itemTreeItem->isClientResource() && !edit);

  int i = 0;

  if( item->GetTitle() != "" || edit )
    {
    i++;
    }
  if( item->GetAuthors().size() || edit )
    {
    i++;
    }
  if( item->GetKeywords().size() || edit )
    {
    i++;
    }
  if( item->GetAbstract() != "" || edit )
    {
    i++;
    }
  if( item->GetDescription() != "" || edit )
    {
    i++;
    }
  if( item->GetSize() != "" || itemTreeItem->isClientResource() )
    {
    i++;
    }

  m_MidasTreeItemInfoTable->setRowCount(i);
  i = 0;

  if( item->GetTitle() != "" || edit )
    {
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0, new QTableWidgetDescriptionItem("Title", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidasItemDescItem(item, item->GetTitle().c_str(), ITEM_TITLE,
                                                                      options) );
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  if( item->GetAuthors().size() || edit )
    {
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0, new QTableWidgetDescriptionItem("Authors", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidasItemDescItem(item, item->GetAuthorsString().c_str(),
                                                                      ITEM_AUTHORS, options) );
    m_AuthorsEditor->setItem(itemTreeItem);
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, m_AuthorsEditor);
    i++;
    }

  if( item->GetKeywords().size() || edit )
    {
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0, new QTableWidgetDescriptionItem("Keywords", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidasItemDescItem(item, item->GetKeywordsString().c_str(),
                                                                      ITEM_KEYWORDS, options) );
    m_KeywordsEditor->setItem(itemTreeItem);
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, m_KeywordsEditor);
    i++;
    }

  if( item->GetAbstract() != "" || edit )
    {
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0, new QTableWidgetDescriptionItem("Abstract", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidasItemDescItem(item, item->GetAbstract().c_str(), ITEM_ABSTRACT,
                                                                      options) );
    m_TextMetadataEditor->setField(ITEM_ABSTRACT);
    m_TextMetadataEditor->setItem(itemTreeItem);
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, m_TextMetadataEditor);
    i++;
    }

  if( item->GetDescription() != "" || edit )
    {
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0,
                                    new QTableWidgetDescriptionItem("Description", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidasItemDescItem(item, item->GetDescription().c_str(),
                                                                      ITEM_DESCRIPTION, options) );
    m_TextMetadataEditor->setField(ITEM_DESCRIPTION);
    m_TextMetadataEditor->setItem(itemTreeItem);
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, m_TextMetadataEditor);
    i++;
    }

  if( item->GetSize() != "" )
    {
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0,
                                    new QTableWidgetDescriptionItem("Total Size", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidasItemDescItem(item,
                                                                      midasUtils::BytesToString(midasUtils::
                                                                                                StringToDouble(item->
                                                                                                               GetSize() ) ).c_str(), ITEM_SIZE,
                                                                      QTableWidgetDescriptionItem::Tooltip) );
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }
  else if( itemTreeItem->isClientResource() )
    {
    mds::Item mdsItem;
    mdsItem.SetObject(item);
    mdsItem.FetchSize();
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0,
                                    new QTableWidgetDescriptionItem("Total Size", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidasItemDescItem(item,
                                                                      midasUtils::BytesToString(midasUtils::
                                                                                                StringToDouble(item->
                                                                                                               GetSize() ) ).c_str(), ITEM_SIZE,
                                                                      QTableWidgetDescriptionItem::Tooltip) );
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  m_MidasTreeItemInfoTable->resizeColumnsToContents();
  int leftoverSpace = m_MidasTreeItemInfoTable->width() - m_MidasTreeItemInfoTable->columnWidth(0)
    - m_MidasTreeItemInfoTable->columnWidth(1);
  m_MidasTreeItemInfoTable->horizontalHeader()->setStretchLastSection(leftoverSpace > 0);
  m_MidasTreeItemInfoTable->resizeColumnsToContents();
  m_MidasTreeItemInfoTable->resizeRowsToContents();
}

void MIDASDesktopUI::InfoPanel(MidasBitstreamTreeItem* bitstreamTreeItem, bool edit)
{
  QTableWidgetDescriptionItem::Options options = QTableWidgetDescriptionItem::Tooltip
    | QTableWidgetDescriptionItem::AlignLeft;

  if( edit )
    {
    options |= QTableWidgetDescriptionItem::Editable;
    }

  // enableResourceEditing(bitstreamTreeItem->isClientResource() && !edit);
  this->EnableResourceEditing(false); // false for now (nothing to edit about a
                                // bitstream)

  mdo::Bitstream* bitstream = bitstreamTreeItem->getBitstream();

  m_MidasTreeItemInfoGroupBox->setTitle(tr(" Bitstream description ") );
  m_MidasTreeItemInfoTable->setGridStyle(edit ? Qt::DashDotLine : Qt::NoPen);

  m_MidasTreeItemInfoTable->clearSelection();
  int i = 2;

  if( bitstream->GetPath() != "" || edit )
    {
    i++;
    }
  m_MidasTreeItemInfoTable->setRowCount(i);
  i = 0;

  m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
  m_MidasTreeItemInfoTable->setItem(i, 0, new QTableWidgetDescriptionItem("Filename", QTableWidgetDescriptionItem::Bold) );
  m_MidasTreeItemInfoTable->setItem(i, 1,
                                  new QTableWidgetMidasBitstreamDescItem(bitstream, bitstream->GetName().c_str(),
                                                                         BITSTREAM_NAME, options) );
  m_MidasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
  i++;

  m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
  m_MidasTreeItemInfoTable->setItem(i, 0, new QTableWidgetDescriptionItem("Size", QTableWidgetDescriptionItem::Bold) );
  m_MidasTreeItemInfoTable->setItem(i, 1,
                                  new QTableWidgetMidasBitstreamDescItem(bitstream,
                                                                         midasUtils::BytesToString(strtod(bitstream->
                                                                                                          GetSize().
                                                                                                          c_str(),
                                                                                                          0) ).c_str(),
                                                                         BITSTREAM_SIZE,
                                                                         QTableWidgetDescriptionItem::Tooltip) );
  m_MidasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
  i++;

  if( bitstream->GetPath() != "" )
    {
    m_MidasTreeItemInfoTable->setRowHeight(i, QTableWidgetDescriptionItem::rowHeight);
    m_MidasTreeItemInfoTable->setItem(i, 0, new QTableWidgetDescriptionItem("Location", QTableWidgetDescriptionItem::Bold) );
    m_MidasTreeItemInfoTable->setItem(i, 1,
                                    new QTableWidgetMidasBitstreamDescItem(bitstream, bitstream->GetPath().c_str(),
                                                                           BITSTREAM_PATH,
                                                                           QTableWidgetDescriptionItem::Tooltip
                                                                           | QTableWidgetDescriptionItem::AlignLeft) );
    m_MidasTreeItemInfoTable->setItemDelegateForRow(i, NULL);
    i++;
    }

  m_MidasTreeItemInfoTable->resizeColumnsToContents();
  int leftoverSpace = m_MidasTreeItemInfoTable->width() - m_MidasTreeItemInfoTable->columnWidth(0)
    - m_MidasTreeItemInfoTable->columnWidth(1);
  m_MidasTreeItemInfoTable->horizontalHeader()->setStretchLastSection(leftoverSpace > 0);
  m_MidasTreeItemInfoTable->resizeColumnsToContents();
  m_MidasTreeItemInfoTable->resizeRowsToContents();
}

void MIDASDesktopUI::ClearInfoPanel()
{
  m_MidasTreeItemInfoTable->clear();
  m_MidasTreeItemInfoTable->setRowCount(0);
  this->EnableResourceEditing(false);
}

void MIDASDesktopUI::DisplayClientResourceContextMenu(QContextMenuEvent* e)
{
  QMenu       menu(this);
  QModelIndex index = m_TreeViewClient->indexAt(e->pos() );

  if( DB_IS_MIDAS3 )
    {
    Midas3FolderTreeItem* folderTreeItem = NULL;
    Midas3ItemTreeItem*   itemTreeItem = NULL;

    if( index.isValid() )
      {
      Midas3TreeItem* item = const_cast<Midas3TreeItem *>(
          dynamic_cast<Midas3TreeViewClient *>(m_TreeViewClient)->getSelectedMidasTreeItem() );

      m_TreeViewClient->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);

      menu.addAction(m_ActionViewDirectory);
      menu.addSeparator();

      if( (folderTreeItem = dynamic_cast<Midas3FolderTreeItem *>(item) ) != NULL )
        {
        menu.addAction(m_ActionAddSubfolder);
        menu.addAction(m_ActionAddItem3);
        }
      else if( (itemTreeItem = dynamic_cast<Midas3ItemTreeItem *>(item) ) != NULL )
        {
        menu.addAction(m_ActionAddBitstream3);
        }
      menu.addAction(this->m_ActionDeleteResource);
      }
    else
      {
      // m_TreeViewServer->selectionModel()->clearSelection();
      menu.addAction(m_ActionAddCommunity3);
      menu.addAction(m_ActionAddTopLevelFolder);
      }
    }
  else
    {
    MidasCommunityTreeItem*  communityTreeItem = NULL;
    MidasCollectionTreeItem* collectionTreeItem = NULL;
    MidasItemTreeItem*       itemTreeItem = NULL;
    MidasBitstreamTreeItem*  bitstreamTreeItem = NULL;

    if( index.isValid() )
      {
      MidasTreeItem* item =
        const_cast<MidasTreeItem *>(dynamic_cast<MidasTreeViewClient *>(m_TreeViewClient)->getSelectedMidasTreeItem() );

      m_TreeViewClient->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);

      menu.addAction(m_ActionViewDirectory);
      menu.addSeparator();

      if( (communityTreeItem = dynamic_cast<MidasCommunityTreeItem *>(item) ) != NULL )
        {
        menu.addAction(m_ActionAddSubcommunity);
        menu.addAction(m_ActionAddCollection);
        }
      else if( (collectionTreeItem = dynamic_cast<MidasCollectionTreeItem *>(item) ) != NULL )
        {
        menu.addAction(m_ActionAddItem);
        }
      else if( (itemTreeItem = dynamic_cast<MidasItemTreeItem *>(item) ) != NULL )
        {
        menu.addAction(m_ActionAddBitstream);
        }
      else if( (bitstreamTreeItem = dynamic_cast<MidasBitstreamTreeItem *>(item) ) != NULL )
        {
        menu.addAction(m_ActionSwapWithMD5Reference);
        }
      menu.addAction(m_ActionDeleteResource);
      }
    else
      {
      m_TreeViewServer->selectionModel()->clearSelection();
      menu.addAction(m_ActionAddCommunity);
      }
    }
  menu.addAction(m_ActionPushResource);
  menu.exec(e->globalPos() );
}

void MIDASDesktopUI::DisplayServerResourceContextMenu(QContextMenuEvent* e)
{
  QMenu menu(this);

  if( SERVER_IS_MIDAS3 )
    {
    // TODO context menu in midas 3 server tree?
    Midas3TreeViewServer* treeView = dynamic_cast<Midas3TreeViewServer *>(m_TreeViewServer);
    }
  else
    {
    MidasTreeViewServer*    treeView = dynamic_cast<MidasTreeViewServer *>(m_TreeViewServer);
    QModelIndex             index = treeView->indexAt(e->pos() );
    MidasItemTreeItem*      itemTreeItem = NULL;
    MidasBitstreamTreeItem* bitstreamTreeItem = NULL;

    if( index.isValid() )
      {
      MidasTreeItem* item = const_cast<MidasTreeItem *>(treeView->getSelectedMidasTreeItem() );

      if( !item->resourceIsFetched() )
        {
        return;
        }
      menu.addAction(m_ActionPullResource);
      menu.addAction(m_ActionOpenUrl);
      menu.addAction(m_ActionDeleteServer);
      m_TreeViewServer->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);

      if( (itemTreeItem = dynamic_cast<MidasItemTreeItem *>(item) ) != NULL )
        {
        menu.addSeparator();
        menu.addAction(m_ActionDownloadKeyFilesTgz);
        menu.addAction(m_ActionDownloadKeyFilesZip);
        }
      else if( (bitstreamTreeItem = dynamic_cast<MidasBitstreamTreeItem *>(item) ) != NULL )
        {
        menu.addSeparator();
        menu.addAction(m_ActionDownloadKeyFile);
        }
      }
    else
      {
      m_TreeViewServer->selectionModel()->clearSelection();
      return;
      }
    }
  menu.exec(e->globalPos() );
}

void MIDASDesktopUI::AddCommunity()
{
  m_CreateMidasResourceUI->SetType(CreateMidasResourceUI::Community);
  m_CreateMidasResourceUI->SetParentResource(NULL);
  m_CreateMidasResourceUI->exec();
}

void MIDASDesktopUI::AddSubcommunity()
{
  m_CreateMidasResourceUI->SetType(CreateMidasResourceUI::SubCommunity);
  m_CreateMidasResourceUI->SetParentResource(
    dynamic_cast<MidasTreeViewClient *>(m_TreeViewClient)->getSelectedMidasTreeItem() );
  m_CreateMidasResourceUI->exec();
}

void MIDASDesktopUI::AddCollection()
{
  m_CreateMidasResourceUI->SetType(CreateMidasResourceUI::Collection);
  m_CreateMidasResourceUI->SetParentResource(
    dynamic_cast<MidasTreeViewClient *>(m_TreeViewClient)->getSelectedMidasTreeItem() );
  m_CreateMidasResourceUI->exec();
}

void MIDASDesktopUI::AddItem()
{
  m_CreateMidasResourceUI->SetType(CreateMidasResourceUI::Item);
  m_CreateMidasResourceUI->SetParentResource(
    dynamic_cast<MidasTreeViewClient *>(m_TreeViewClient)->getSelectedMidasTreeItem() );
  m_CreateMidasResourceUI->exec();
}

void MIDASDesktopUI::AddCommunity3()
{
  m_CreateMidasResourceUI->SetType(CreateMidasResourceUI::Community3);
  m_CreateMidasResourceUI->SetParentResource3(NULL);
  m_CreateMidasResourceUI->exec();
}

void MIDASDesktopUI::AddTopLevelFolder()
{
  m_CreateMidasResourceUI->SetType(CreateMidasResourceUI::Folder);
  m_CreateMidasResourceUI->SetParentResource3(NULL);
  m_CreateMidasResourceUI->exec();
}

void MIDASDesktopUI::AddSubfolder()
{
  m_CreateMidasResourceUI->SetType(CreateMidasResourceUI::SubFolder);
  m_CreateMidasResourceUI->SetParentResource3(
    dynamic_cast<Midas3TreeViewClient *>(m_TreeViewClient)->getSelectedMidasTreeItem() );
  m_CreateMidasResourceUI->exec();
}

void MIDASDesktopUI::AddItem3()
{
  m_CreateMidasResourceUI->SetType(CreateMidasResourceUI::Item3);
  m_CreateMidasResourceUI->SetParentResource3(
    dynamic_cast<Midas3TreeViewClient *>(m_TreeViewClient)->getSelectedMidasTreeItem() );
  m_CreateMidasResourceUI->exec();
}

void MIDASDesktopUI::AddBitstream()
{
  QStringList files = QFileDialog::getOpenFileNames(
      this, tr("Load Files"),
      QDir::current().absolutePath(),
      tr("All Files (*)") );

  if( files.size() )
    {
    if( DB_IS_MIDAS3 )
      {
      this->AddBitstreams(reinterpret_cast<Midas3ItemTreeItem *>(
                      const_cast<Midas3TreeItem *>(dynamic_cast<Midas3TreeViewClient *>(
                                                     m_TreeViewClient)->getSelectedMidasTreeItem() ) ), files);
      }
    else
      {
      this->AddBitstreams(reinterpret_cast<MidasItemTreeItem *>(
                      const_cast<MidasTreeItem *>(
                        dynamic_cast<MidasTreeViewClient *>(m_TreeViewClient)->getSelectedMidasTreeItem() ) ), files);
      }
    }
}

void MIDASDesktopUI::AddBitstreams(const MidasItemTreeItem* parentItem,
                                   const QStringList& files)
{
  if( !this->AddBitstreamsCommon(files) )
    {
    return;
    }

  m_AddBitstreamsThread->SetParentItem(
    const_cast<MidasItemTreeItem *>(parentItem) );
  m_AddBitstreamsThread->start();
}

void MIDASDesktopUI::AddBitstreams(const Midas3ItemTreeItem* parentItem,
                                   const QStringList& files)
{
  if( !this->AddBitstreamsCommon(files) )
    {
    return;
    }

  m_AddBitstreamsThread->SetParentItem(
    const_cast<Midas3ItemTreeItem *>(parentItem) );
  m_AddBitstreamsThread->start();
}

/**
 * Extracted the common code between MIDAS 2 and MIDAS 3 for adding
 * bitstreams to the client tree into this helper method
 */
bool MIDASDesktopUI::AddBitstreamsCommon(const QStringList& files)
{
  if( m_AddBitstreamsThread && m_AddBitstreamsThread->isRunning() )
    {
    return false; // already busy adding bitstreams
    }
  m_PollFilesystemThread->Pause();
  delete m_AddBitstreamsThread;
  m_AddBitstreamsThread = new AddBitstreamsThread;
  m_AddBitstreamsThread->SetFiles(files);

  connect(m_AddBitstreamsThread, SIGNAL(finished() ),
          m_PollFilesystemThread, SLOT(Resume() ) );
  connect(m_AddBitstreamsThread, SIGNAL(finished() ),
          this, SLOT(resetStatus() ) );
  connect(m_AddBitstreamsThread, SIGNAL(enableActions(bool) ),
          this, SLOT(enableClientActions(bool) ) );
  connect(m_AddBitstreamsThread, SIGNAL(progress(int, int, const QString &) ),
          this, SLOT(addBitstreamsProgress(int, int, const QString &) ) );
  m_Progress->ResetProgress();
  m_Progress->ResetOverall();
  m_Progress->SetUnit(" files");
  return true;
}

void MIDASDesktopUI::AddBitstreamsProgress(int current, int total,
                                           const QString& message)
{
  m_Progress->SetMessage(message.toStdString() );
  m_Progress->SetMaxCount(total);
  m_Progress->SetMaxTotal(total);
  m_Progress->UpdateProgress(current, total);
  m_Progress->UpdateOverallCount(current);
  this->Log->Status("Adding bitstream " + message.toStdString() );
}

void MIDASDesktopUI::ViewDirectory()
{
  std::string path;

  if( DB_IS_MIDAS3 )
    {
    Midas3TreeItem* resource = const_cast<Midas3TreeItem *>(
        dynamic_cast<Midas3TreeViewClient *>(m_TreeViewClient)->getSelectedMidasTreeItem() );
    if( resource->getType() == midas3ResourceType::BITSTREAM )
      {
      QFileInfo fileInfo(resource->getPath().c_str() );
      path = fileInfo.dir().path().toStdString();
      }
    else
      {
      path = resource->getPath();
      }
    }
  else
    {
    MidasTreeItem* resource = const_cast<MidasTreeItem *>(
        dynamic_cast<MidasTreeViewClient *>(m_TreeViewClient)->getSelectedMidasTreeItem() );
    mds::DatabaseAPI    db;
    midasResourceRecord record = db.GetRecordByUuid(resource->getUuid() );

    QFileInfo fileInfo(record.Path.c_str() );
    path = record.Type == midasResourceType::BITSTREAM ?
      fileInfo.dir().path().toStdString() : record.Path;
    }

  path = "file:" + path;
  QUrl url(path.c_str() );
  if( !QDesktopServices::openUrl(url) )
    {
    std::stringstream text;
    text << "The operating system does not know how to open "
         << path << std::endl;
    this->GetLog()->Error(text.str() );
    }
}

void MIDASDesktopUI::OpenBitstream()
{
  std::string path;

  if( DB_IS_MIDAS3 )
    {
    path = dynamic_cast<Midas3BitstreamTreeItem *>(const_cast<Midas3TreeItem *>(
                                                     dynamic_cast<Midas3TreeViewClient *>(m_TreeViewClient)->
                                                     getSelectedMidasTreeItem() ) )
      ->getBitstream()->GetPath();
    }
  else
    {
    MidasTreeItem* resource = const_cast<MidasTreeItem *>(
        dynamic_cast<MidasTreeViewClient *>(m_TreeViewClient)->getSelectedMidasTreeItem() );
    mds::DatabaseAPI db;
    path = db.GetRecordByUuid(resource->getUuid() ).Path;
    }

  path = "file:" + path;
  QUrl url(path.c_str() );
  if( !QDesktopServices::openUrl(url) )
    {
    std::stringstream text;
    text << "The operating system does not know how to open "
         << path << std::endl;
    GetLog()->Error(text.str() );
    }
}

void MIDASDesktopUI::ViewInBrowser()
{
  QString baseUrl = mws::WebAPI::Instance()->GetServerUrl();

  baseUrl = baseUrl.replace("/api/rest", "");
  std::stringstream path;
  path << baseUrl.toStdString();

  if( SERVER_IS_MIDAS3 )
    {
    // TODO view in browser: MIDAS 3 version
    }
  else
    {
    MidasTreeItem* resource = const_cast<MidasTreeItem *>(
        dynamic_cast<MidasTreeViewServer *>(
          m_TreeViewServer)
        ->getSelectedMidasTreeItem() );
    MidasCommunityTreeItem*  comm = NULL;
    MidasCollectionTreeItem* coll = NULL;
    MidasItemTreeItem*       item = NULL;

    if( (comm = dynamic_cast<MidasCommunityTreeItem *>(resource) ) != NULL )
      {
      path << "/community/view/" << comm->getCommunity()->GetId();
      }
    else if( (coll = dynamic_cast<MidasCollectionTreeItem *>(resource) ) != NULL )
      {
      path << "/collection/view/" << coll->getCollection()->GetId();
      }
    else if( (item = dynamic_cast<MidasItemTreeItem *>(resource) ) != NULL )
      {
      path << "/item/view/" << item->getItem()->GetId();
      }

    QUrl url(path.str().c_str() );
    if( !QDesktopServices::openUrl(url) )
      {
      std::stringstream text;
      text << "The operating system does not know how to open "
           << path.str() << std::endl;
      GetLog()->Error(text.str() );
      }
    }
}

void MIDASDesktopUI::DisplayStatus(const QString& message)
{
  m_StateLabel->setText(message);
}

void MIDASDesktopUI::SigningIn()
{
  this->DisplayStatus("Connecting to server...");
  this->SetProgressIndeterminate();
}

void MIDASDesktopUI::SignInOrOut()
{
  if( !m_SignIn )
    {
    m_SignInUI->exec();
    }
  else
    {
    this->SignOut();
    }
}

void MIDASDesktopUI::SetTimerInterval()
{
  mds::DatabaseAPI db;
  int              minutes = atoi(db.GetSetting(mds::DatabaseAPI::AUTO_REFRESH_INTERVAL).c_str() );

  m_RefreshTimer->setInterval(minutes * 60 * 1000);
}

void MIDASDesktopUI::AdjustTimerSettings()
{
  mds::DatabaseAPI db;
  int              setting = atoi(db.GetSetting(mds::DatabaseAPI::AUTO_REFRESH_SETTING).c_str() );

  m_RefreshTimer->stop();

  switch( setting )
    {
    case 0:
      if( this->isHidden() && m_SignIn )
        {
        m_RefreshTimer->start();
        }
      break;
    case 1:
      if( m_SignIn )
        {
        m_RefreshTimer->start();
        }
      break;
    default:
      break;
    }
}

void MIDASDesktopUI::SignIn(bool ok)
{
  if( ok )
    {
    m_ConnectLabel->hide();

    // start the refresh timer here if our setting = 1
    mds::DatabaseAPI db;
    if( atoi(db.GetSetting(mds::DatabaseAPI::AUTO_REFRESH_SETTING).c_str() ) == 1 )
      {
      m_RefreshTimer->start();
      }

    // Satus bar
    std::string connectStr = "  Connected to " + std::string(mws::WebAPI::Instance()->GetServerUrl() ) + "  ";
    m_ConnectLabel->setText(connectStr.c_str() );
    m_ConnectLabel->show();

    std::stringstream text;
    text << "Signed in with profile " << m_Synch->GetAuthenticator()->GetProfile();
    GetLog()->Message(text.str() );
    m_SignIn = true;
    this->DisplayStatus(tr("") );

    delete m_TreeViewServer;
    if( SERVER_IS_MIDAS3 )
      {
      m_TreeViewServer = new Midas3TreeViewServer(this);

      connect(m_TreeViewServer, SIGNAL(midasTreeItemSelected(const Midas3TreeItem *) ),
              this, SLOT(updateActionState(const Midas3TreeItem *) ) );
      connect(m_TreeViewServer, SIGNAL(midas3FolderTreeItemSelected(const Midas3FolderTreeItem *) ),
              this, SLOT(updateInfoPanel(const Midas3FolderTreeItem *) ) );
      connect(m_TreeViewServer, SIGNAL(midas3ItemTreeItemSelected(const Midas3ItemTreeItem *) ),
              this, SLOT(updateInfoPanel(const Midas3ItemTreeItem *) ) );
      connect(m_TreeViewServer, SIGNAL(midas3BitstreamTreeItemSelected(const Midas3BitstreamTreeItem *) ),
              this, SLOT(updateInfoPanel(const Midas3BitstreamTreeItem *) ) );
      }
    else
      {
      m_TreeViewServer = new MidasTreeViewServer(this);

      connect(m_TreeViewServer, SIGNAL(midasTreeItemSelected(const MidasTreeItem *) ),
              this, SLOT(UpdateActionState(const MidasTreeItem *) ) );
      connect(m_TreeViewServer, SIGNAL(midasCommunityTreeItemSelected(const MidasCommunityTreeItem *) ),
              this, SLOT(UpdateInfoPanel(const MidasCommunityTreeItem *) ) );
      connect(m_TreeViewServer, SIGNAL(midasCollectionTreeItemSelected(const MidasCollectionTreeItem *) ),
              this, SLOT(UpdateInfoPanel(const MidasCollectionTreeItem *) ) );
      connect(m_TreeViewServer, SIGNAL(midasItemTreeItemSelected(const MidasItemTreeItem *) ),
              this, SLOT(UpdateInfoPanel(const MidasItemTreeItem *) ) );
      connect(m_TreeViewServer, SIGNAL(midasBitstreamTreeItemSelected(const MidasBitstreamTreeItem *) ),
              this, SLOT(UpdateInfoPanel(const MidasBitstreamTreeItem *) ) );
      }
    delete m_TreeViewServerPlaceholder;
    m_TreeViewServerPlaceholder = NULL;
    m_ServerTreeViewContainer->layout()->addWidget(m_TreeViewServer);
    this->ActivateActions(true, MIDASDesktopUI::ACTION_CONNECTED);
    m_TreeViewServer->SetSynchronizer(m_Synch);
    connect(m_RefreshTimer, SIGNAL(timeout() ), m_TreeViewServer, SLOT(Update() ) );
    connect(m_TreeViewServer, SIGNAL(resourceDropped(int, int) ),
            this, SLOT(DragNDropPush(int, int) ) );
    connect(m_TreeViewServer, SIGNAL(midasNoTreeItemSelected() ),
            this, SLOT(ClearInfoPanel() ) );
    connect(m_TreeViewServer, SIGNAL(midasNoTreeItemSelected() ),
            m_PullUI, SLOT(ResetState() ) );
    connect(m_TreeViewServer, SIGNAL(midasTreeViewContextMenu(QContextMenuEvent *) ),
            this, SLOT(DisplayServerResourceContextMenu(QContextMenuEvent *) ) );
    connect(m_TreeViewServer->model(), SIGNAL(ServerPolled() ), this, SLOT(StoreLastPollTime() ) );

    connect(m_TreeViewServer, SIGNAL(StartedExpandingTree() ), this, SLOT(StartedExpandingTree() ) );
    connect(m_TreeViewServer, SIGNAL(FinishedExpandingTree() ), this, SLOT(FinishedExpandingTree() ) );

    connect(m_TreeViewServer, SIGNAL(EnableActions(bool) ), this, SLOT(enableActions(bool) ) );
    m_TreeViewServer->Initialize();
    }
  else
    {
    this->GetLog()->Error("The URL provided is not a valid MIDAS server Web API.");
    this->DisplayStatus(tr("Failed to connect") );
    }
  this->SetProgressEmpty();
}

void MIDASDesktopUI::ChooseLocalDatabase()
{
  // ------------- display FileDialog -------------
  QString file = QFileDialog::getOpenFileName(
      this, tr("Choose Database File"),
      QDir::current().absolutePath(),
      tr("All Files (*)") );

  this->SetLocalDatabase(file.toStdString() );
}

void MIDASDesktopUI::CreateLocalDatabase()
{
  std::string file = QFileDialog::getSaveFileName(this,
                                                  tr("New Database File"),
                                                  QDir::current().absolutePath(),
                                                  tr("Database Files (*.db)"), 0,
                                                  QFileDialog::ShowDirsOnly).toStdString();

  QFileInfo fileInfo(file.c_str() );

  if( fileInfo.exists() )
    {
    std::stringstream text;
    text << "Error: " << file << " already exists.  Choose a new file name.";
    this->Log->Error(text.str() );
    return;
    }

  if( file != "" )
    {
    QMessageBox msgBox;
    msgBox.setText("Do you want to create a MIDAS 2 or MIDAS 3 database?");
    msgBox.setInformativeText("");
    QAbstractButton* midas2Button = msgBox.addButton("MIDAS 2", QMessageBox::YesRole);
    QAbstractButton* midas3Button = msgBox.addButton("MIDAS 3", QMessageBox::YesRole);
    QAbstractButton* cancelButton = msgBox.addButton("Cancel", QMessageBox::NoRole);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.exec();

    if( msgBox.clickedButton() != cancelButton )
      {
      this->Log->Message("Creating new local database at " + file);
      m_ActionNewLocalDatabase->setEnabled(false);
      this->SetProgressIndeterminate();
      bool          midas3 = msgBox.clickedButton() == midas3Button;
      QFuture<bool> future = QtConcurrent::run(midasUtils::CreateNewDatabase, file, midas3);
      m_CreateDBWatcher.setFuture(future);
      }
    }
}

void MIDASDesktopUI::SetLocalDatabase(std::string file)
{
  if( file == "" || !midasUtils::IsDatabaseValid(file, m_DatabaseUpgradeHandler) )
    {
    std::stringstream text;
    text << file << " is not a valid MIDAS SQLite database. Defaulting "
    " to midas.db.";
    this->GetLog()->Message(text.str() );
    std::string path = QDir::currentPath().toStdString() + "/midas.db";
    if( midasUtils::IsDatabaseValid(path, m_DatabaseUpgradeHandler) )
      {
      this->SetLocalDatabase(path);
      return;
      }
    this->GetLog()->Error("No suitable database file found!");
    return;
    }

  if( midasUtils::IsDatabaseValid(file, m_DatabaseUpgradeHandler) )
    {
    mds::DatabaseInfo::Instance()->SetPath(file);
    QSettings settings("Kitware", "MIDASDesktop");
    settings.setValue("lastDatabase", file.c_str() );
    settings.sync();
    this->DisplayStatus(tr("Opened database successfully.") );
    this->Log->Message("Opened database " + file);

    delete m_TreeViewClient;
    if( DB_IS_MIDAS3 )
      {
      m_TreeViewClient = new Midas3TreeViewClient(this);

      connect(m_TreeViewClient, SIGNAL(midas3FolderTreeItemSelected(const Midas3FolderTreeItem *) ),
              this, SLOT(UpdateInfoPanel(const Midas3FolderTreeItem *) ) );
      connect(m_TreeViewClient, SIGNAL(midas3ItemTreeItemSelected(const Midas3ItemTreeItem *) ),
              this, SLOT(UpdateInfoPanel(const Midas3ItemTreeItem *) ) );
      connect(m_TreeViewClient, SIGNAL(midas3BitstreamTreeItemSelected(const Midas3BitstreamTreeItem *) ),
              this, SLOT(UpdateInfoPanel(const Midas3BitstreamTreeItem *) ) );
      connect(m_TreeViewClient, SIGNAL(midasTreeItemSelected(const Midas3TreeItem *) ),
              this, SLOT(UpdateActionStateClient(const Midas3TreeItem *) ) );
      connect(m_TreeViewClient, SIGNAL(bitstreamsDropped(const Midas3ItemTreeItem *, const QStringList &) ),
              this, SLOT(AddBitstreams(const Midas3ItemTreeItem *, const QStringList &) ) );
      connect(m_TreeViewClient, SIGNAL(bitstreamOpenRequest() ), this, SLOT(openBitstream() ) );
      }
    else
      {
      m_TreeViewClient = new MidasTreeViewClient(this);

      connect(m_TreeViewClient, SIGNAL(midasCommunityTreeItemSelected(const MidasCommunityTreeItem *) ),
              this, SLOT(UpdateInfoPanel(const MidasCommunityTreeItem *) ) );
      connect(m_TreeViewClient, SIGNAL(midasCollectionTreeItemSelected(const MidasCollectionTreeItem *) ),
              this, SLOT(UpdateInfoPanel(const MidasCollectionTreeItem *) ) );
      connect(m_TreeViewClient, SIGNAL(midasItemTreeItemSelected(const MidasItemTreeItem *) ),
              this, SLOT(UpdateInfoPanel(const MidasItemTreeItem *) ) );
      connect(m_TreeViewClient, SIGNAL(midasBitstreamTreeItemSelected(const MidasBitstreamTreeItem *) ),
              this, SLOT(UpdateInfoPanel(const MidasBitstreamTreeItem *) ) );
      connect(m_TreeViewClient, SIGNAL(midasTreeItemSelected(const MidasTreeItem *) ),
              this, SLOT(UpdateActionStateClient(const MidasTreeItem *) ) );

      connect(m_TreeViewClient, SIGNAL(bitstreamsDropped(const MidasItemTreeItem *, const QStringList &) ),
              this, SLOT(AddBitstreams(const MidasItemTreeItem *, const QStringList &) ) );
      connect(m_TreeViewClient, SIGNAL(bitstreamOpenRequest() ), this, SLOT(openBitstream() ) );
      }
    delete m_ResourceUpdateHandler;
    m_ResourceUpdateHandler = new TreeViewUpdateHandler(m_TreeViewClient);
    mds::DatabaseInfo::Instance()->SetResourceUpdateHandler(m_ResourceUpdateHandler);
    delete m_TreeViewClientPlaceholder;
    m_TreeViewClientPlaceholder = NULL;
    m_ClientTreeViewContainer->layout()->addWidget(m_TreeViewClient);
    this->ActivateActions(true, MIDASDesktopUI::ACTION_LOCAL_DATABASE);

    m_TreeViewClient->SetSynchronizer(m_Synch);

    connect(m_TreeViewClient, SIGNAL(resourceDropped(int, int) ),
            this, SLOT(PullRecursive(int, int) ) );

    connect(m_TreeViewClient, SIGNAL(midasNoTreeItemSelected() ),
            this, SLOT(ClearInfoPanel() ) );

    connect(m_RefreshClientButton, SIGNAL(released() ), this, SLOT(UpdateClientTreeView() ) );

    connect(m_TreeViewClient, SIGNAL(midasTreeViewContextMenu(QContextMenuEvent *) ),
            this, SLOT(DisplayClientResourceContextMenu(QContextMenuEvent *) ) );

    m_TreeViewClient->collapseAll();
    this->UpdateClientTreeView();
    this->SetTimerInterval();
    this->AdjustTimerSettings();

    // start the filesystem monitoring thread
    m_PollFilesystemThread = new PollFilesystemThread;

    connect(m_PollFilesystemThread, SIGNAL(needToRefresh() ), this, SLOT(
              UpdateClientTreeView() ), Qt::BlockingQueuedConnection);
    connect(m_PullUI, SIGNAL(startingSynchronizer() ), m_PollFilesystemThread, SLOT(Pause() ) );
    connect(m_PullUI, SIGNAL(pulledResources() ), m_PollFilesystemThread, SLOT(Resume() ) );

    m_PollFilesystemThread->start();
    }
  else
    {
    std::stringstream text;
    text << "The path " << file << " does not refer to a valid MIDASDesktop "
    "database.";
    this->GetLog()->Error(text.str() );
    }
}

void MIDASDesktopUI::CreateProfile(const std::string& name, const std::string& email,
                                   const std::string& apiName, const std::string& password,
                                   const std::string& rootDir, const std::string& url)
{
  if( mds::DatabaseInfo::Instance()->GetPath() == "" )
    {
    this->DisplayStatus(tr("Please choose a local database first.") );
    return;
    }

  mds::DatabaseAPI db;
  db.SetSetting(mds::DatabaseAPI::LAST_URL, url);
  std::string oldUrl = mws::WebAPI::Instance()->GetServerUrl();
  mws::WebAPI::Instance()->SetServerUrl(url.c_str() );
  std::string msg;
  if( m_Synch->GetAuthenticator()->AddAuthProfile(email, apiName, password, rootDir, name) )
    {
    msg = "Successfully created profile \"" + name + "\".";
    m_SignInUI->profileCreated(name);
    }
  else
    {
    msg = "Failed to create authentication profile.";
    }
  mws::WebAPI::Instance()->SetServerUrl(oldUrl.c_str() );
  this->DisplayStatus(tr(msg.c_str() ) );
}

/** Signing out */
void MIDASDesktopUI::SignOut()
{
  this->ActivateActions(false, ACTION_ALL);
  m_TreeViewServer->Clear();
  this->ClearInfoPanel();
  m_ConnectLabel->hide();
  this->DisplayStatus(tr("Logout") );
  m_SearchItemsListWidget->clear();
  m_Synch->GetAuthenticator()->ClearToken();
  mws::WebAPI::Instance()->Logout();
  m_SignIn = false;

  m_RefreshTimer->stop();
}

void MIDASDesktopUI::PushResources()
{
  this->DisplayStatus(tr("Pushing locally added resources...") );
  this->SetProgressIndeterminate();

  if( DB_IS_MIDAS3 )
    {
    const Midas3TreeItem* resource =
      dynamic_cast<Midas3TreeViewClient *>(m_TreeViewClient)->getSelectedMidasTreeItem();
    m_PushUI->setObject(resource ? resource->getObject() : NULL);
    }
  else
    {
    const MidasTreeItem* resource =
      dynamic_cast<MidasTreeViewClient *>(m_TreeViewClient)->getSelectedMidasTreeItem();
    m_PushUI->setObject(resource ? resource->getObject() : NULL);
    }
  m_PushUI->exec();
}

void MIDASDesktopUI::PushReturned(int rc)
{
  this->UpdateClientTreeView();
  this->UpdateServerTreeView();

  std::stringstream text;
  if( rc == MIDAS_OK )
    {
    text << "Finished pushing locally added resources.";
    this->GetLog()->Message(text.str() );
    }
  else if( rc == MIDAS_CANCELED )
    {
    text << "Push canceled by user.";
    this->GetLog()->Message(text.str() );
    }
  else
    {
    text << "Failed to push resources to the server. Error code = " << rc;
    this->GetLog()->Error(text.str() );
    }
  this->DisplayStatus(text.str().c_str() );
  this->SetProgressEmpty();
}

void MIDASDesktopUI::Search()
{
  this->DisplayStatus(tr("Searching...") );
  m_SearchItemsListWidget->clear();
  std::vector<std::string> words;
  midasUtils::Tokenize(m_SearchQueryEdit->text().toStdString(), words);

  if( m_SearchThread )
    {
    disconnect(m_SearchThread);
    }
  delete m_SearchThread;

  m_SearchResults.clear();

  m_SearchThread = new SearchThread;
  m_SearchThread->SetWords(words);
  m_SearchThread->SetResults(&m_SearchResults);

  connect(m_SearchThread, SIGNAL(finished() ),
          this, SLOT(showSearchResults() ) );

  this->ActivateActions(false, ACTION_CONNECTED);
  this->DisplayStatus("Performing search...");
  this->SetProgressIndeterminate();

  m_SearchThread->start();
}

void MIDASDesktopUI::ShowSearchResults()
{
  for( std::vector<mdo::Object *>::iterator i = m_SearchResults.begin();
       i != m_SearchResults.end(); ++i )
    {
    new QListWidgetItemMidasItem(m_SearchItemsListWidget, *i);
    }
  this->ActivateActions(true, ACTION_CONNECTED);
  this->ResetStatus();
}

void MIDASDesktopUI::SearchItemClicked(QListWidgetItemMidasItem* listItem)
{
  // TODO MIDAS3ify
  dynamic_cast<MidasTreeViewServer *>(m_TreeViewServer)
  ->selectByUuid(listItem->getObject()->GetUuid(), true);
}

void MIDASDesktopUI::SearchItemContextMenu(QContextMenuEvent* e)
{
  QMenu menu(this);
  QModelIndex index = m_SearchItemsListWidget->indexAt(e->pos() );

  if( index.isValid() )
    {
    menu.addAction(m_ActionOpenUrl);
    menu.addAction(m_ActionPullResource);
    menu.exec(e->globalPos() );
    }
}

void MIDASDesktopUI::StoreLastPollTime()
{
  if( SERVER_IS_MIDAS3 )
    {
    return; // no new resources call for MIDAS 3 yet.
    }
  mds::DatabaseAPI db;
  this->EnableActions(false);
  mws::NewResources newResources;
  newResources.SetAuthenticator(m_Synch->GetAuthenticator() );
  newResources.SetSince(db.GetSetting(mds::DatabaseAPI::LAST_FETCH_TIME) );
  newResources.Fetch();
  this->EnableActions(true);

  m_DirtyUuids = newResources.GetUuids();
  std::reverse(m_DirtyUuids.begin(), m_DirtyUuids.end() );

  if( m_DirtyUuids.size() )
    {
    this->AlertNewResources();
    }

  db.SetSetting(mds::DatabaseAPI::LAST_FETCH_TIME, newResources.GetTimestamp() );
}

void MIDASDesktopUI::DecorateServerTree()
{
  if( SERVER_IS_MIDAS3 )
    {
    // TODO midas 3 server tree decoration
    }
  else
    {
    if( m_Cancel )
      {
      m_Cancel = false;
      m_DirtyUuids.clear();
      return;
      }
    if( m_DirtyUuids.size() )
      {
      this->EnableActions(false);
      connect(m_TreeViewServer, SIGNAL(FinishedExpandingTree() ),
              this, SLOT(DecorateCallback() ) );
      dynamic_cast<MidasTreeViewServer *>(m_TreeViewServer)->selectByUuid(m_DirtyUuids[0]);
      }
    }
}

void MIDASDesktopUI::DecorateCallback()
{
  if( SERVER_IS_MIDAS3 )
    {
    dynamic_cast<Midas3TreeViewServer *>(m_TreeViewServer)->decorateByUuid(m_DirtyUuids[0]);
    }
  else
    {
    dynamic_cast<MidasTreeViewServer *>(m_TreeViewServer)->decorateByUuid(m_DirtyUuids[0]);
    }
  m_DirtyUuids.erase(m_DirtyUuids.begin() );
  disconnect(m_TreeViewServer, SIGNAL(FinishedExpandingTree() ),
             this, SLOT(DecorateCallback() ) );
  this->DecorateServerTree();
}

void MIDASDesktopUI::SetProgressEmpty()
{
  m_Progress->ResetProgress();
}

void MIDASDesktopUI::SetProgressIndeterminate()
{
  m_Progress->SetIndeterminate();
}

void MIDASDesktopUI::StartedExpandingTree()
{
  this->SetProgressIndeterminate();
  this->DisplayStatus("Finding resource in the server tree...");
  this->ActivateActions(false, MIDASDesktopUI::ACTION_CONNECTED);
}

void MIDASDesktopUI::FinishedExpandingTree()
{
  this->SetProgressEmpty();
  this->DisplayStatus("");
  this->ActivateActions(true, MIDASDesktopUI::ACTION_CONNECTED);
  m_TreeViewServer->setFocus();
}

void MIDASDesktopUI::DeleteLocalResource(bool deleteFiles)
{
  m_PollFilesystemThread->Pause();
  if( m_DeleteThread && m_DeleteThread->isRunning() )
    {
    return;
    }
  delete m_DeleteThread;
  m_DeleteThread = new DeleteThread;
  m_DeleteThread->SetDeleteOnDisk(deleteFiles);

  connect(m_DeleteThread, SIGNAL(enableActions(bool) ), this, SLOT(EnableClientActions(bool) ) );
  connect(m_DeleteThread, SIGNAL(finished() ), this, SLOT(ResetStatus() ) );
  connect(m_DeleteThread, SIGNAL(finished() ), this, SLOT(UpdateClientTreeView() ) );

  if( DB_IS_MIDAS3 )
    {
    const Midas3TreeItem* treeItem = dynamic_cast<Midas3TreeViewClient *>(m_TreeViewClient)->getSelectedMidasTreeItem();
    m_DeleteThread->SetResource3(const_cast<Midas3TreeItem *>(treeItem) );
    }
  else
    {
    const MidasTreeItem* treeItem = dynamic_cast<MidasTreeViewClient *>(m_TreeViewClient)->getSelectedMidasTreeItem();
    m_DeleteThread->SetResource(const_cast<MidasTreeItem *>(treeItem) );
    }

  this->Log->Status("Deleting local resources...");
  m_DeleteThread->start();
  this->SetProgressIndeterminate();
}

// Controller for deleting server resources
void MIDASDesktopUI::DeleteServerResource(bool val)
{
  if( SERVER_IS_MIDAS3 )
    {
    // TODO delete server resource
    }
  else
    {
    const MidasTreeItem* resource = dynamic_cast<MidasTreeViewServer *>(m_TreeViewServer)->getSelectedMidasTreeItem();
    int                  id = resource->getId();
    std::string          typeName = QString(midasUtils::GetTypeName(resource->getType() ).c_str() ).toStdString();

    std::stringstream text;
    if( mws::WebAPI::Instance()->DeleteResource(typeName, id) )
      {
      text << "Successfully deleted " << typeName
           << " with id=" << id << " from the server.";
      this->GetLog()->Message(text.str() );
      this->UpdateServerTreeView();
      }
    else
      {
      text << "Failed to delete " << typeName << " with id=" << id
           << " from the server";
      this->GetLog()->Error(text.str() );
      }
    }
}

void MIDASDesktopUI::AlertErrorInLog()
{
  if( m_LogAndSearchTabContainer->currentIndex() != MIDAS_TAB_LOG )
    {
    m_LogAndSearchTabContainer->setTabIcon(
      MIDAS_TAB_LOG, QPixmap(":icons/exclamation.png") );
    }
}

void MIDASDesktopUI::TabChanged(int index)
{
  if( index == MIDAS_TAB_LOG ) // log tab
    {
    m_LogAndSearchTabContainer->setTabIcon(2, QPixmap() );
    }
  else if( index == MIDAS_TAB_INCOMPLETE_TRANSFERS )
    {
    m_TransferWidget->Populate();
    }
}

void MIDASDesktopUI::ShowProgressTab()
{
  m_LogAndSearchTabContainer->setCurrentIndex(MIDAS_TAB_PROGRESS);
}

void MIDASDesktopUI::PullRecursive(int type, int id)
{
  if( QMessageBox::question(this, "Confirm Pull",
                            "Are you sure you want to pull the selected resource and all of its children?",
                            QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok )
    {
    m_PullUI->setPull();
    m_PullUI->setRecursive(true);
    m_PullUI->setResourceType(type);
    m_PullUI->setPullId(id);
    m_PullUI->accept();
    }
}

void MIDASDesktopUI::DragNDropPush(int type, int id)
{
  mdo::Community*  comm = NULL;
  mdo::Collection* coll = NULL;
  mdo::Item*       item = NULL;
  mdo::Bitstream*  bitstream = NULL;
  mdo::Object*     obj = NULL;

  mds::DatabaseAPI db;
  std::string      uuid = db.GetUuid(type, id);

  switch( type )
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
  obj->SetUuid(uuid.c_str() );

  m_PushUI->setObject(obj);
  m_PushUI->setDelete(true); // will delete obj when done
  m_PushUI->exec();
}

void MIDASDesktopUI::EnableResourceEditing(bool val)
{
  m_EditInfoButton->setEnabled(val);
}

void MIDASDesktopUI::EditInfo()
{
  MidasTreeItem* node = const_cast<MidasTreeItem *>(
      dynamic_cast<MidasTreeViewClient *>(m_TreeViewClient)->getSelectedMidasTreeItem() );

  MidasCommunityTreeItem*  comm = NULL;
  MidasCollectionTreeItem* coll = NULL;
  MidasItemTreeItem*       item = NULL;
  MidasBitstreamTreeItem*  bitstream = NULL;

  if( (comm = dynamic_cast<MidasCommunityTreeItem *>(node) ) != NULL )
    {
    this->InfoPanel(comm, true);
    }
  else if( (coll = dynamic_cast<MidasCollectionTreeItem *>(node) ) != NULL )
    {
    this->InfoPanel(coll, true);
    }
  else if( (item = dynamic_cast<MidasItemTreeItem *>(node) ) != NULL )
    {
    this->InfoPanel(item, true);
    }
  else if( (bitstream = dynamic_cast<MidasBitstreamTreeItem *>(node) ) != NULL )
    {
    this->InfoPanel(bitstream, true);
    }
  m_EditMode = true;
}

void MIDASDesktopUI::ResourceEdited(QTableWidgetItem* row)
{
  if( m_EditMode )
    {
    ResourceEdit editor;
    editor.SetLog(this->Log);

    connect(&editor, SIGNAL(DataModified(std::string) ), m_TreeViewClient,
            SLOT(decorateByUuid(std::string) ) );

    editor.Save(row);
    disconnect(&editor);
    }
}

void MIDASDesktopUI::NewDatabaseFinished()
{
  m_ActionNewLocalDatabase->setEnabled(true);
  this->SetProgressEmpty();
  if( m_CreateDBWatcher.result() )
    {
    this->Log->Message("New local database created successfully.");
    }
  else
    {
    this->Log->Error("Failed to create new local database.");
    }
}

void MIDASDesktopUI::CurrentFileMessage(const QString& message)
{
  std::string labelText = "Current File: " + message.toStdString();

  m_CurrentFilenameLabel->setText(labelText.c_str() );
}

void MIDASDesktopUI::OverallProgressUpdate(int current, int max)
{
  std::stringstream text;

  text << "File Count: " << current << " / " << max << " files";
  m_FileCountLabel->setText(text.str().c_str() );
}

void MIDASDesktopUI::CurrentProgressUpdate(double current, double max)
{
  std::string       currentText = midasUtils::BytesToString(current, m_Progress->GetUnit() );
  std::string       maxText = midasUtils::BytesToString(max, m_Progress->GetUnit() );
  std::stringstream text;

  if( max == 0 )
    {
    text << "Progress: Calculating...";
    }
  else
    {
    text << "Progress: " << currentText << " / " << maxText;
    }
  m_CurrentProgressLabel->setText(text.str().c_str() );

  if( max == 0 )
    {
    m_ProgressBarCurrent->setMaximum(100);
    m_ProgressBarCurrent->setValue(0);
    return;
    }
  double fraction = current / max;
  int    percent = static_cast<int>(fraction * 100.0);
  m_ProgressBarCurrent->setMaximum(100);
  m_ProgressBarCurrent->setValue(percent);
  m_ProgressBar->setMaximum(100);
  m_ProgressBar->setValue(percent);
}

void MIDASDesktopUI::TotalProgressUpdate(double current, double max)
{
  std::string       currentText = midasUtils::BytesToString(current, m_Progress->GetUnit() );
  std::string       maxText = midasUtils::BytesToString(max, m_Progress->GetUnit() );
  std::stringstream text;

  if( max == 0 )
    {
    text << "Overall Progress: Calculating...";
    }
  else
    {
    text << "Overall Progress: " << currentText << " / " << maxText;
    }
  m_OverallProgressLabel->setText(text.str().c_str() );

  if( max == 0 )
    {
    m_ProgressBarOverall->setMaximum(100);
    m_ProgressBarOverall->setValue(0);
    return;
    }
  double fraction = current / max;
  int    percent = static_cast<int>(fraction * 100.0);
  m_ProgressBarOverall->setMaximum(100);
  m_ProgressBarOverall->setValue(percent);
}

void MIDASDesktopUI::ProgressSpeedUpdate(double bytesPerSec)
{
  std::stringstream text;

  if( bytesPerSec == 0 )
    {
    text << "Speed: Calculating...";
    }
  else
    {
    text << "Speed: " << midasUtils::BytesToString(bytesPerSec, m_Progress->GetUnit() )
         << " / sec";
    }
  m_SpeedLabel->setText(text.str().c_str() );
}

void MIDASDesktopUI::EstimatedTimeUpdate(double seconds)
{
  std::stringstream text;

  if( seconds == 0 )
    {
    text << "Estimated Time Remaining: Calculating...";
    }
  else
    {
    text << "Estimated Time Remaining: "
         << midasUtils::FormatTimeString(seconds);
    }
  m_EstimatedTimeLabel->setText(text.str().c_str() );
}

void MIDASDesktopUI::LogError(const QString& text)
{
  this->GetLog()->Error(text.toStdString() );
}

void MIDASDesktopUI::LogMessage(const QString& text)
{
  this->GetLog()->Message(text.toStdString() );
}

void MIDASDesktopUI::UnifyingTree()
{
  this->DisplayStatus("Copying resources into a single tree...");
  this->SetProgressIndeterminate();
  m_PollFilesystemThread->Pause();
}

void MIDASDesktopUI::TreeUnified()
{
  this->DisplayStatus("Finished unifying resources on disk.");
  this->GetLog()->Message("Finished unifying resources on disk.");
  this->SetProgressEmpty();
  this->UpdateClientTreeView();

  m_PollFilesystemThread->Resume();
}


/******************************************************************************
 * Copyright 2011 Kitware Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
#ifndef __MIDASDesktopUI_H
#define __MIDASDesktopUI_H

#define MIDAS_TAB_SEARCH               0
#define MIDAS_TAB_PROGRESS             1
#define MIDAS_TAB_LOG                  2
#define MIDAS_TAB_INCOMPLETE_TRANSFERS 3

#include <QFlags>
#include <QProgressBar>
#include <QPushButton>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QTimer>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrentRun>

#include "ui_MIDASDesktopUI.h"
#include "midasLogAware.h"

class CreateMidasResourceUI;
class SignInUI;
class PullUI;
class PushUI;
class CreateProfileUI;
class DeleteResourceUI;
class AboutUI;
class PreferencesUI;
class AddAuthorUI;
class AddKeywordUI;
class AgreementUI;
class UpgradeUI;
class FileOverwriteUI;
class MirrorPickerUI;

class midasSynchronizer;
class midasProgressReporter;
class midasAgreementHandler;
class midasFileOverwriteHandler;
class ButtonDelegate;
class TextEditDelegate;

class AddBitstreamsThread;
class SynchronizerThread;
class SearchThread;
class UpdateTreeViewThread;
class PollFilesystemThread;
class DeleteThread;

class QContextMenuEvent;
class MidasTreeItem;
class MidasCommunityTreeItem;
class MidasCollectionTreeItem;
class MidasItemTreeItem;
class MidasBitstreamTreeItem;
class Midas3TreeItem;
class Midas3FolderTreeItem;
class Midas3ItemTreeItem;
class Midas3BitstreamTreeItem;
class IncompleteTransferWidget;

class MidasTreeViewBase;

namespace mdo
{
class Object;
}

namespace mds
{
class ResourceUpdateHandler;
class UpgradeHandler;
}

namespace mws
{
class MirrorHandler;
}

class MIDASDesktopUI : public QMainWindow, public midasLogAware, private Ui::MIDASDesktopWindow
{
  Q_OBJECT
public:

  enum UIAction
    {
    ACTION_ALL                   = 0xFF,
    ACTION_CONNECTED             = 0x1,
    ACTION_ALL_CONNECTED         = 0xFF - 0x1,
    ACTION_COMMUNITY             = 0x2,
    ACTION_COLLECTION            = 0x4,
    ACTION_ITEM                  = 0x8,
    ACTION_BITSTREAM             = 0x30, // 0x30 = 0x10 + 0x20 = (110000)b
    ACTION_BITSTREAM_LIST        = 0x10,
    ACTION_BITSTREAM_COMPUTER    = 0x20,
    ACTION_BITSTREAM_TRANSFERING = 0x40,
    ACTION_CLIENT_COMMUNITY      = 0x80,
    ACTION_CLIENT_COLLECTION     = 0x100,
    ACTION_CLIENT_ITEM           = 0x200,
    ACTION_CLIENT_BITSTREAM      = 0x400,
    ACTION_CLIENT_RESOURCE       = 0x780, // 0x80 + 0x100 + 0x200 + 0x400 (any
                                          // client side resource)
    ACTION_LOCAL_DATABASE        = 0x800,

    ACTION_COMMUNITY3            = 0x01000,
    ACTION_FOLDER3               = 0x02000,
    ACTION_ITEM3                 = 0x04000,
    ACTION_CLIENT_COMMUNITY3     = 0x08000,
    ACTION_CLIENT_FOLDER3        = 0x10000,
    ACTION_CLIENT_ITEM3          = 0x20000,
    ACTION_CLIENT_BITSTREAM3     = 0x40000,
    ACTION_CLIENT_RESOURCE3      = 0x78000
    };
  Q_DECLARE_FLAGS(UIActions, UIAction) MIDASDesktopUI();
  ~MIDASDesktopUI();

  void ActivateActions(bool value, UIActions activateAction);

  MidasTreeViewBase * GetTreeViewServer()
  {
    return m_TreeViewServer;
  }
  MidasTreeViewBase * GetTreeViewClient()
  {
    return m_TreeViewClient;
  }
  midasSynchronizer * GetSynchronizer()
  {
    return m_Synch;
  }
  midasProgressReporter * GetProgress()
  {
    return m_Progress;
  }
  PollFilesystemThread * GetPollFilesystemThread()
  {
    return m_PollFilesystemThread;
  }
  midasAgreementHandler * GetAgreementHandler()
  {
    return m_AgreementHandler;
  }
  midasFileOverwriteHandler * GetFileOverwriteHandler()
  {
    return m_OverwriteHandler;
  }
  QTextEdit * GetLogTextEdit()
  {
    return m_Log;
  }
protected:
  // inherited
  void closeEvent(QCloseEvent *event);

public slots:
  void showNormal();

  void Cancel();

  void ResourceEdited(QTableWidgetItem* item);

  // ------------- status bar -------------
  void DisplayStatus(const QString& message);

  void ResetStatus();

  // ------------- status bar -------------

  // -------------- progress tab -----------
  void CurrentFileMessage(const QString& message);

  void OverallProgressUpdate(int current, int max);

  void TotalProgressUpdate(double current, double max);

  void CurrentProgressUpdate(double current, double max);

  void ProgressSpeedUpdate(double bytesPerSec);

  void EstimatedTimeUpdate(double seconds);

  // -------------- progress tab -----------
  void SigningIn();

  void SignInOrOut();

  void SignIn(bool ok);

  void SignOut();

  void CreateProfile(const std::string& name, const std::string& email, const std::string& apiName,
                     const std::string& password, const std::string& rootDir,
                     const std::string& url);

  void ChooseLocalDatabase();

  void CreateLocalDatabase();

  void SetLocalDatabase(std::string file);

  void DeleteLocalResource(bool deleteFiles);

  void DeleteServerResource(bool val);

  void UpdateClientTreeView();

  void UpdateServerTreeView();

  void DecorateServerTree();

  void DecorateCallback();

  void StartedExpandingTree();

  void FinishedExpandingTree();

  void EnableActions(bool val);

  void EnableClientActions(bool val);

  void EnableResourceEditing(bool val);

  // ------------- settings -------------
  void SetTimerInterval();

  void AdjustTimerSettings();

  void UnifyingTree();

  void TreeUnified();

  // ------------- settings -------------

  // -------------- progress bar ----------
  void SetProgressIndeterminate();

  void SetProgressEmpty();

  void ShowProgressTab();

  // -------------- progress bar ----------

  // ------------- tray icon -------------
  void IconActivated(QSystemTrayIcon::ActivationReason reason);

  void AlertNewResources();

  // ------------- tray icon -------------

  // ------------- UI updates -------------
  void UpdateInfoPanel(const MidasCommunityTreeItem* communityTreeItem);

  void UpdateInfoPanel(const MidasCollectionTreeItem* collectionTreeItem);

  void UpdateInfoPanel(const MidasItemTreeItem* itemTreeItem);

  void UpdateInfoPanel(const MidasBitstreamTreeItem* bitstreamTreeItem);

  void UpdateInfoPanel(const Midas3FolderTreeItem* folderTreeItem);

  void UpdateInfoPanel(const Midas3ItemTreeItem* itemTreeItem);

  void UpdateInfoPanel(const Midas3BitstreamTreeItem* bitstreamTreeItem);

  void ClearInfoPanel();

  void EditInfo();

  void UpdateActionState(const MidasTreeItem* item);

  void UpdateActionState(const Midas3TreeItem* item);

  void UpdateActionStateClient(const MidasTreeItem* item);

  void UpdateActionStateClient(const Midas3TreeItem* item);

  void DisplayServerResourceContextMenu(QContextMenuEvent* e);

  void DisplayClientResourceContextMenu(QContextMenuEvent* e);

  // ------------- UI updates -------------

  // ------------- resource manipulation -------------

  void AddCommunity();

  void AddSubcommunity();

  void AddCollection();

  void AddItem();

  void AddBitstream();

  void AddCommunity3();

  void AddTopLevelFolder();

  void AddSubfolder();

  void AddItem3();

  void AddBitstreams(const MidasItemTreeItem* parentItem, const QStringList& files);

  void AddBitstreams(const Midas3ItemTreeItem* parentItem, const QStringList& files);

  void AddBitstreamsProgress(int current, int total, const QString& message);

  void PullRecursive(int type, int id);

  void DragNDropPush(int type, int id);

  void ViewInBrowser();

  void ViewDirectory();

  void OpenBitstream();

  void StoreLastPollTime();

  // ------------- resource manipulation -------------

  // ------------- synchronizer ----------------------
  void PushResources();

  void PushReturned(int rc);

  // ------------- synchronizer ----------------------

  // ------------- search -------------
  void Search();

  void ShowSearchResults();

  void SearchItemClicked(QListWidgetItemMidasItem * item);

  void SearchItemContextMenu(QContextMenuEvent * e);

  // ------------- search -------------

  // ------------- log ----------------
  void ShowLogTab();

  void LogError(const QString& text);

  void LogMessage(const QString& text);

  void AlertErrorInLog();

  void TabChanged(int index);

  // ------------- log ----------------

  void NewDatabaseFinished();

  mds::ResourceUpdateHandler * GetResourceUpdateHandler();

private:

  void InfoPanel(MidasCommunityTreeItem* node, bool editable);

  void InfoPanel(MidasCollectionTreeItem* node, bool editable);

  void InfoPanel(MidasItemTreeItem* node, bool editable);

  void InfoPanel(MidasBitstreamTreeItem* node, bool editable);

  /** Common code for adding bitstreams to the client tree */
  bool AddBitstreamsCommon(const QStringList& files);

  // ------------- UI Dialogs -------------
  CreateMidasResourceUI* m_CreateMidasResourceUI;
  CreateProfileUI*       m_CreateProfileUI;
  SignInUI*              m_SignInUI;
  AboutUI*               m_AboutUI;
  PreferencesUI*         m_PreferencesUI;
  PullUI*                m_PullUI;
  PushUI*                m_PushUI;
  DeleteResourceUI*      m_DeleteClientResourceUI;
  DeleteResourceUI*      m_DeleteServerResourceUI;
  AddAuthorUI*           m_AddAuthorUI;
  AddKeywordUI*          m_AddKeywordUI;
  AgreementUI*           m_AgreementUI;
  UpgradeUI*             m_UpgradeUI;
  FileOverwriteUI*       m_OverwriteUI;
  MirrorPickerUI*        m_MirrorPickerUI;
  // ------------- UI Dialogs -------------

  IncompleteTransferWidget* m_TransferWidget;

  // ------------- status bar -------------
  QLabel*       m_StateLabel;
  QLabel*       m_ConnectLabel;
  QProgressBar* m_ProgressBar;
  QPushButton*  m_CancelButton;
  // ------------- status bar -------------

  ButtonDelegate*   m_AuthorsEditor;
  ButtonDelegate*   m_KeywordsEditor;
  TextEditDelegate* m_TextMetadataEditor;

  // ------------- tray ----------------
  QAction*         m_ShowAction;
  QSystemTrayIcon* m_TrayIcon;
  QMenu*           m_TrayIconMenu;
  // ------------- tray ----------------

  // ------------- auto-refresh -----------
  QTimer* m_RefreshTimer;
  // ------------- auto-refresh -----------

  bool                        m_SignIn;
  bool                        m_EditMode;
  bool                        m_Cancel;
  midasSynchronizer*          m_Synch;
  midasProgressReporter*      m_Progress;
  midasAgreementHandler*      m_AgreementHandler;
  midasFileOverwriteHandler*  m_OverwriteHandler;
  std::vector<std::string>    m_DirtyUuids;
  std::vector<mdo::Object *>  m_SearchResults;
  mds::ResourceUpdateHandler* m_ResourceUpdateHandler;
  mds::UpgradeHandler*        m_DatabaseUpgradeHandler;
  mws::MirrorHandler*         m_MirrorHandler;

  // ----------- threads -----------------
  SynchronizerThread*   m_SynchronizerThread;
  SearchThread*         m_SearchThread;
  UpdateTreeViewThread* m_ReadDatabaseThread;
  UpdateTreeViewThread* m_RefreshThread;
  PollFilesystemThread* m_PollFilesystemThread;
  AddBitstreamsThread*  m_AddBitstreamsThread;
  DeleteThread*         m_DeleteThread;
  QFutureWatcher<bool>  m_CreateDBWatcher;
  // ----------- threads -----------------

  MidasTreeViewBase* m_TreeViewServer;
  MidasTreeViewBase* m_TreeViewClient;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(MIDASDesktopUI::UIActions)

#endif // __MIDASDesktopUI_H

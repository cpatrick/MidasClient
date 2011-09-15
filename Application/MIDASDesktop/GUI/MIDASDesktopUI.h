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
class IncompleteTransferWidget;

class MidasTreeViewBase;

namespace mdo {
  class Object;
}

namespace mds {
  class ResourceUpdateHandler;
  class UpgradeHandler;
}

namespace mws {
  class MirrorHandler;
}

extern "C" {
  static int progress_transfer_callback(void* data, double dltotal, double dlnow, double ultotal, double ulnow); 
}

class MIDASDesktopUI :  public QMainWindow, public midasLogAware, private Ui::MIDASDesktopWindow
{
  Q_OBJECT

public:

  enum ActivateAction
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
    ACTION_CLIENT_RESOURCE       = 0x780, //0x80 + 0x100 + 0x200 + 0x400 (any client side resource)
    ACTION_LOCAL_DATABASE        = 0x800,

    ACTION_COMMUNITY3            = 0x1000,
    ACTION_FOLDER3               = 0x2000,
    ACTION_ITEM3                 = 0x4000,
    ACTION_CLIENT_COMMUNITY3     = 0x8000,
    ACTION_CLIENT_FOLDER3        = 0x10000,
    ACTION_CLIENT_ITEM3          = 0x20000
    }; 
  Q_DECLARE_FLAGS(ActivateActions, ActivateAction)
     
  MIDASDesktopUI();
  ~MIDASDesktopUI();

  void activateActions(bool value, ActivateActions activateAction); 

  MidasTreeViewBase* getTreeViewServer() { return treeViewServer; }
  MidasTreeViewBase* getTreeViewClient() { return treeViewClient; }
  midasSynchronizer* getSynchronizer() { return m_synch; }
  midasProgressReporter* getProgress() { return m_progress; }
  PollFilesystemThread* getPollFilesystemThread() { return m_PollFilesystemThread; }
  midasAgreementHandler* getAgreementHandler() { return m_agreementHandler; }
  midasFileOverwriteHandler* getFileOverwriteHandler() { return m_overwriteHandler; }
  QTextEdit* getLogTextEdit() { return log; }

protected:
  void closeEvent(QCloseEvent *event);

public slots:
  void showNormal();
  void cancel();

  void resourceEdited(QTableWidgetItem* item);

  // ------------- status bar -------------
  void displayStatus(const QString& message);
  void resetStatus();
  // ------------- status bar -------------

  //-------------- progress tab -----------
  void currentFileMessage(const QString& message);
  void overallProgressUpdate(int current, int max);
  void totalProgressUpdate(double current, double max);
  void currentProgressUpdate(double current, double max);
  void progressSpeedUpdate(double bytesPerSec);
  void estimatedTimeUpdate(double seconds);
  //-------------- progress tab -----------
  void signingIn();
  void signInOrOut();
  void signIn(bool ok);
  void signOut();
  void createProfile(const std::string& name, const std::string& email,
                     const std::string& apiName, const std::string& password,
                     const std::string& rootDir, const std::string& url);
  void chooseLocalDatabase();
  void createLocalDatabase();
  void setLocalDatabase(std::string file);
  void deleteLocalResource(bool deleteFiles);
  void deleteServerResource(bool val);
  void updateClientTreeView();
  void updateServerTreeView();
  void decorateServerTree();
  void decorateCallback();

  void startedExpandingTree();
  void finishedExpandingTree();

  void enableActions(bool val);
  void enableClientActions(bool val);

  void enableResourceEditing(bool val);

  // ------------- settings -------------
  void setTimerInterval();
  void adjustTimerSettings();
  void unifyingTree();
  void treeUnified();
  // ------------- settings -------------

  // -------------- progress bar ----------
  void setProgressIndeterminate();
  void setProgressEmpty();
  void showProgressTab();
  // -------------- progress bar ----------

  // ------------- tray icon -------------
  void iconActivated(QSystemTrayIcon::ActivationReason reason);
  void alertNewResources();
  // ------------- tray icon -------------

  // ------------- UI updates -------------
  void updateInfoPanel(const MidasCommunityTreeItem* communityTreeItem);
  void updateInfoPanel(const MidasCollectionTreeItem* collectionTreeItem);
  void updateInfoPanel(const MidasItemTreeItem* itemTreeItem);
  void updateInfoPanel(const MidasBitstreamTreeItem* bitstreamTreeItem);
  void updateInfoPanel(const Midas3FolderTreeItem* folderTreeItem);
  void updateInfoPanel(const Midas3ItemTreeItem* itemTreeItem);
  void clearInfoPanel();
  void editInfo();

  void updateActionState(const MidasTreeItem* item);
  void updateActionState(const Midas3TreeItem* item);
  void updateActionStateClient(const MidasTreeItem* item);
  void updateActionStateClient(const Midas3TreeItem* item);

  void displayServerResourceContextMenu(QContextMenuEvent* e);
  void displayClientResourceContextMenu(QContextMenuEvent* e);
  // ------------- UI updates -------------

  // ------------- resource manipulation -------------

  void addCommunity();
  void addSubcommunity();
  void addCollection();
  void addItem();
  void addBitstream();
  void addCommunity3();
  void addTopLevelFolder();
  void addSubfolder();
  void addItem3();
  void addBitstreams(const MidasItemTreeItem* parentItem,
                     const QStringList & files);
  void addBitstreamsProgress(int current, int total, const QString& message);
  void pullRecursive(int type, int id);
  void dragNDropPush(int type, int id);
  void viewInBrowser();
  void viewDirectory();
  void openBitstream();
  void storeLastPollTime();
  // ------------- resource manipulation -------------

  // ------------- synchronizer ----------------------
  void pushResources();
  void pushReturned(int rc);
  // ------------- synchronizer ----------------------

  // ------------- search -------------
  void search();
  void showSearchResults();
  void searchItemClicked(QListWidgetItemMidasItem * item);
  void searchItemContextMenu(QContextMenuEvent * e);
  // ------------- search -------------

  // ------------- log ----------------
  void showLogTab();
  void logError(const QString& text);
  void logMessage(const QString& text);
  void alertErrorInLog();
  void tabChanged(int index);
  // ------------- log ----------------

  void newDBFinished();

  mds::ResourceUpdateHandler* getResourceUpdateHandler();
private:
  
  void infoPanel(MidasCommunityTreeItem* node, bool editable);
  void infoPanel(MidasCollectionTreeItem* node, bool editable);
  void infoPanel(MidasItemTreeItem* node, bool editable);
  void infoPanel(MidasBitstreamTreeItem* node, bool editable);

  // ------------- UI Dialogs -------------
  CreateMidasResourceUI*      dlg_createMidasResourceUI;
  CreateProfileUI*            dlg_createProfileUI;
  SignInUI*                   dlg_signInUI;
  AboutUI*                    dlg_aboutUI;
  PreferencesUI*              dlg_preferencesUI;
  PullUI*                     dlg_pullUI;
  PushUI*                     dlg_pushUI;
  DeleteResourceUI*           dlg_deleteClientResourceUI;
  DeleteResourceUI*           dlg_deleteServerResourceUI;
  AddAuthorUI*                dlg_addAuthorUI;
  AddKeywordUI*               dlg_addKeywordUI;
  AgreementUI*                dlg_agreementUI;
  UpgradeUI*                  dlg_upgradeUI;
  FileOverwriteUI*            dlg_overwriteUI;
  MirrorPickerUI*             dlg_mirrorPickerUI;
  // ------------- UI Dialogs -------------

  IncompleteTransferWidget*   transferWidget;

  // ------------- status bar -------------
  QLabel*                     stateLabel;
  QLabel*                     connectLabel;
  QProgressBar*               progressBar;
  QPushButton*                cancelButton;
  // ------------- status bar -------------

  ButtonDelegate*             authorsEditor;
  ButtonDelegate*             keywordsEditor;
  TextEditDelegate*           textMetadataEditor;

  // ------------- tray ----------------
  QAction*                    showAction;
  QSystemTrayIcon*            trayIcon;
  QMenu*                      trayIconMenu;
  // ------------- tray ----------------

  // ------------- auto-refresh -----------
  QTimer*                     refreshTimer;
  // ------------- auto-refresh -----------

  bool                        m_signIn;
  bool                        m_editMode;
  bool                        m_cancel;
  midasSynchronizer*          m_synch;
  midasProgressReporter*      m_progress;
  midasAgreementHandler*      m_agreementHandler;
  midasFileOverwriteHandler*  m_overwriteHandler;
  std::vector<std::string>    m_dirtyUuids;
  std::vector<mdo::Object*>   m_SearchResults;
  mds::ResourceUpdateHandler* m_resourceUpdateHandler;
  mds::UpgradeHandler*        m_dbUpgradeHandler;
  mws::MirrorHandler*         m_mirrorHandler;

  // ----------- threads -----------------
  SynchronizerThread*         m_SynchronizerThread;
  SearchThread*               m_SearchThread;
  UpdateTreeViewThread*       m_ReadDatabaseThread;
  UpdateTreeViewThread*       m_RefreshThread;
  PollFilesystemThread*       m_PollFilesystemThread;
  AddBitstreamsThread*        m_AddBitstreamsThread;
  DeleteThread*               m_DeleteThread;
  QFutureWatcher<bool>        m_CreateDBWatcher;
  // ----------- threads -----------------

  MidasTreeViewBase*          treeViewServer;
  MidasTreeViewBase*          treeViewClient;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(MIDASDesktopUI::ActivateActions)

#endif //__MIDASDesktopUI_H

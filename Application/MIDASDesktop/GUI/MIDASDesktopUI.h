#ifndef __MIDASDesktopUI_H
#define __MIDASDesktopUI_H

#include <QFlags>
#include <QProgressBar>
#include <QPushButton>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QTimer>

#include "ui_MIDASDesktopUI.h"
#include "midasLogAware.h"

class UploadAgreementUI;
class CreateMidasCommunityUI; 
class CreateMidasCollectionUI; 
class CreateMidasItemUI; 
class SignInUI;
class PullUI;
class CreateProfileUI;
class DeleteResourceUI;
class AboutUI;
class PreferencesUI;
class midasSynchronizer;
class midasAuthenticator;
class midasDatabaseProxy;
class midasProgressReporter;

class RefreshServerTreeThread;
class SynchronizerThread;
class SearchThread;

class QContextMenuEvent;
class MidasTreeItem;

namespace mdo {
  class Object;
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
    ACTION_LOCAL_DATABASE        = 0x800
    }; 
  Q_DECLARE_FLAGS(ActivateActions, ActivateAction)
     
  MIDASDesktopUI();
  ~MIDASDesktopUI();

  void activateActions(bool value, ActivateActions activateAction); 

  MidasTreeViewServer * getTreeViewServer() { return treeViewServer; }
  MidasTreeViewClient * getTreeViewClient() { return treeViewClient; }
  midasDatabaseProxy* getDatabaseProxy() { return m_database; }
  midasAuthenticator* getAuthenticator() { return m_auth; }
  midasSynchronizer* getSynchronizer() { return m_synch; }
  QTextEdit* getLogTextEdit() { return log; }

protected:
  void closeEvent(QCloseEvent *event);

public slots:
  void showNormal();

  void resourceEdited(QTableWidgetItem* item);

  void cancel();

  // ------------- status bar -------------
  void displayStatus(const QString& message);
  void resetStatus();
  // ------------- status bar -------------

  void signInOrOut();
  void signIn(bool ok);
  void signOut();
  void createProfile(std::string name, std::string email,
                     std::string apiName, std::string apiKey,
                     std::string rootDir);
  void chooseLocalDatabase();
  void setLocalDatabase(std::string file);
  void deleteLocalResource(bool deleteFiles);
  void updateClientTreeView();
  void updateServerTreeView();
  void decorateServerTree();
  void decorateCallback();

  void startedExpandingTree();
  void finishedExpandingTree();

  void enableActions(bool val);

  void enableResourceEditing(bool val);

  // ------------- settings -------------
  void setTimerInterval();
  void adjustTimerSettings();
  // ------------- settings -------------

  // -------------- progress bar ----------
  void setProgressIndeterminate();
  void setProgressEmpty();
  // -------------- progress bar ----------

  // ------------- tray icon -------------
  void iconActivated(QSystemTrayIcon::ActivationReason reason);
  void alertNewResources();
  // ------------- tray icon -------------

  // ------------- UI updates -------------
  void updateInfoPanel( const MidasCommunityTreeItem* communityTreeItem );
  void updateInfoPanel( const MidasCollectionTreeItem* collectionTreeItem );
  void updateInfoPanel( const MidasItemTreeItem* itemTreeItem );
  void updateInfoPanel( const MidasBitstreamTreeItem* bitstreamTreeItem );
  void clearInfoPanel();
  void editInfo();

  void updateActionState(const MidasTreeItem* item);
  void updateActionStateClient(const MidasTreeItem* item);

  void displayServerResourceContextMenu(QContextMenuEvent* e);
  void displayClientResourceContextMenu(QContextMenuEvent* e);
  // ------------- UI updates -------------

  // ------------- resource manipulation -------------

  void addCommunity();
  void addSubcommunity();
  void addCollection();
  void addItem();
  void addBitstream();
  void addBitstreams(const MidasItemTreeItem* parentItem,
                     const QStringList & files);
  void pullRecursive(int type, int id);
  void viewInBrowser();
  void viewDirectory();
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

  void setServerURL(std::string);

  // ------------- log ----------------
  void showLogTab();
  void alertErrorInLog();
  void clearLogTabIcon(int index);
  // ------------- log ----------------

private:
  
  void infoPanel(MidasCommunityTreeItem* node, bool editable);
  void infoPanel(MidasCollectionTreeItem* node, bool editable);
  void infoPanel(MidasItemTreeItem* node, bool editable);
  void infoPanel(MidasBitstreamTreeItem* node, bool editable);

  // ------------- UI Dialogs -------------
  CreateMidasCommunityUI *    dlg_createMidasCommunityUI;
  CreateMidasCommunityUI *    dlg_createMidasSubCommunityUI;
  CreateMidasCommunityUI *    dlg_addMidasCommunityUI;
  CreateMidasCommunityUI *    dlg_addMidasSubCommunityUI;
  CreateMidasCollectionUI *   dlg_createMidasCollectionUI;
  CreateMidasCollectionUI *   dlg_addMidasCollectionUI;
  CreateMidasItemUI *         dlg_createMidasItemUI;
  CreateMidasItemUI *         dlg_addMidasItemUI;
  CreateProfileUI *           dlg_createProfileUI;
  SignInUI *                  dlg_signInUI;
  UploadAgreementUI *         dlg_uploadAgreementUI;
  AboutUI *                   dlg_aboutUI;
  PreferencesUI *             dlg_preferencesUI;
  PullUI *                    dlg_pullUI;
  DeleteResourceUI*           dlg_deleteResourceUI;
  // ------------- UI Dialogs -------------

  // ------------- status bar -------------
  QLabel *                    stateLabel;
  QLabel *                    connectLabel;
  QProgressBar *              progressBar;
  QPushButton *               cancelButton;
  // ------------- status bar -------------

  QPushButton *               saveButton;

  // ------------- tray ----------------
  QAction *                   showAction;
  QSystemTrayIcon *           trayIcon;
  QMenu *                     trayIconMenu;
  // ------------- tray ----------------

  // ------------- auto-refresh -----------
  QTimer *                    refreshTimer;
  // ------------- auto-refresh -----------


  bool                        m_signIn;
  bool                        m_editMode;
  midasDatabaseProxy*         m_database;
  midasAuthenticator*         m_auth;
  midasSynchronizer*          m_synch;
  std::string                 m_url;
  midasProgressReporter*      m_progress;
  std::vector<std::string>    m_dirtyUuids;
  std::vector<mdo::Object*>   m_SearchResults;

  // ----------- threads -----------------
  RefreshServerTreeThread*    m_RefreshThread;
  SynchronizerThread*         m_SynchronizerThread;
  SearchThread*               m_SearchThread;
  // ----------- threads -----------------
};

Q_DECLARE_OPERATORS_FOR_FLAGS( MIDASDesktopUI::ActivateActions )

#endif //__MIDASDesktopUI_H

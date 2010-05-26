#ifndef __MIDASDesktopUI_H
#define __MIDASDesktopUI_H

#include <QFlags>
#include <QProgressBar>
#include <QSystemTrayIcon>
#include <QCloseEvent>

#include "ui_MIDASDesktopUI.h"

class UploadAgreementUI;
class UpDownLoadUI; 
class SettingsUI;
class CreateMidasCommunityUI; 
class CreateMidasCollectionUI; 
class CreateMidasItemUI; 
class SignInUI;
class PullUI;
class CreateProfileUI;
class AddResourceUI;
class AboutUI;
class AutoRefreshUI;
class BuyUI;
class midasSynchronizer;
class midasAuthenticator;
class midasDatabaseProxy;
class midasLog;
class midasProgressReporter;

class QContextMenuEvent; 
class MidasTreeItem; 

extern "C" {
  static int progress_transfer_callback(void* data, double dltotal, double dlnow, double ultotal, double ulnow); 
}

class MIDASDesktopUI :  public QMainWindow, private Ui::MIDASDesktopWindow
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
    ACTION_CLIENT_BITSTREAM      = 0x400
    }; 
  Q_DECLARE_FLAGS(ActivateActions, ActivateAction)
     
  MIDASDesktopUI();
  ~MIDASDesktopUI();

  void activateActions(bool value, ActivateActions activateAction); 

  // ------------- status bar -------------
  void displayStatus(const QString& message);
  void resetStatus();
  // ------------- status bar -------------

  MidasTreeView * getTreeView();
  MidasTreeViewClient * getTreeViewClient() { return treeViewClient; }
  midasDatabaseProxy* getDatabaseProxy() { return m_database; }
  midasAuthenticator* getAuthenticator() { return m_auth; }
  midasSynchronizer* getSynchronizer() { return m_synch; }
  midasLog* getLog() { return m_logger; }

protected:
  void closeEvent(QCloseEvent *event);

public slots:
  void signInOrOut();
  void signIn();
  void signOut();
  void createProfile(std::string name, std::string email,
                     std::string apiName, std::string apiKey );
  void chooseLocalDatabase();
  void setLocalDatabase(std::string file);
  void clientTreeViewUpdated();
  void serverTreeViewUpdated();
  void decorateServerTree();

  // ------------- settings -------------
  void editServerSettings();
  void editSearchSettings();
  void checkDatabaseSettings();
  // ------------- settings -------------

  // -------------- progress bar ----------
  void setProgressIndeterminate();
  void setProgressEmpty();
  // -------------- progress bar ----------

  // ------------- tray icon -------------
  void iconActivated(QSystemTrayIcon::ActivationReason reason);
  // ------------- tray icon -------------

  // ------------- UI updates -------------
  void updateInfoPanel( const MidasCommunityTreeItem* communityTreeItem ); 
  void updateInfoPanel( const MidasCollectionTreeItem* collectionTreeItem ); 
  void updateInfoPanel( const MidasItemTreeItem* itemTreeItem ); 
  void clearInfoPanel();

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
  void openBitstream(int id);
  void viewInBrowser();
  void storeLastPollTime();
  // ------------- resource manipulation -------------

  // ------------- synchronizer ----------------------
  void pushResources();
  // ------------- synchronizer ----------------------

  // ------------- search -------------
  void search(); 
  void searchItemClicked(QListWidgetItemMidasItem * item);
  void searchItemContextMenu(QContextMenuEvent * e);
  // ------------- search -------------

  void setServerURL(std::string);

  void showLogTab();

private:

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
  SettingsUI *                dlg_settingsUI;
  UpDownLoadUI *              dlg_upDownLoadUI;
  UploadAgreementUI *         dlg_uploadAgreementUI;
  AboutUI *                   dlg_aboutUI;
  AddResourceUI *             dlg_addResourceUI;
  AutoRefreshUI *             dlg_autoRefreshUI;
  PullUI *                    dlg_pullUI;
  BuyUI *                     dlg_buyUI;
  // ------------- UI Dialogs -------------

  // ------------- status bar -------------
  QLabel *                    stateLabel;
  QLabel *                    connectLabel;
  QLabel *                    hostLabel;
  QProgressBar *              progressBar;
  // ------------- status bar -------------

  // ------------- tray ----------------
  QAction *                   minimizeAction;
  QAction *                   maximizeAction;
  QAction *                   restoreAction;
  QAction *                   quitAction;
  QSystemTrayIcon *           trayIcon;
  QMenu *                     trayIconMenu;
  // ------------- tray ----------------

  bool                        m_signIn;
  midasDatabaseProxy*         m_database;
  midasAuthenticator*         m_auth;
  midasSynchronizer*          m_synch;
  std::string                 m_url;
  midasLog*                   m_logger;
  midasProgressReporter*      m_progress;
  std::vector<std::string>    m_dirtyUuids;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( MIDASDesktopUI::ActivateActions )

#endif //__MIDASDesktopUI_H

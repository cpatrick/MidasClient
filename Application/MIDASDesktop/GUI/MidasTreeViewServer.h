#ifndef __MidasTreeViewServer_H
#define __MidasTreeViewServer_H

#include "MidasTreeView.h"
#include <QMutex>
#include <QPoint>
#include <string>

class QItemSelection;
class MidasTreeItem;
class MidasCommunityTreeItem;
class MidasCollectionTreeItem;
class MidasItemTreeItem;
class MidasBitstreamTreeItem;
class QContextMenuEvent;
class MidasTreeModelServer;
class ExpandTreeThread;

namespace mws{
  class WebAPI;
}

class MidasTreeViewServer : public MidasTreeView
{
  Q_OBJECT

public:
  
  /** Constructor */
  MidasTreeViewServer(QWidget * parent = 0); 
  ~MidasTreeViewServer();

  /** Set the web API */
  void SetWebAPI(mws::WebAPI* api);

  /** Initialize the tree */
  bool Initialize();

  /** Clear the tree */
  void Clear(); 

  bool isModelIndexSelected() const; 
  //const MidasTreeItem * getMidasTreeItemParent(QModelIndex& selected) const;
  //const std::string getSelectedMidasTreeItemParentId() const; 
  const QModelIndex getSelectedModelIndex() const;
  const MidasTreeItem * getSelectedMidasTreeItem() const;

  void mousePressEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);

public slots:
  
  void updateSelection(const QItemSelection &selected,
    const QItemSelection &deselected);
  void selectByUuid(std::string uuid);
  void decorateByUuid(std::string uuid);
  void alertFetchedMore();
  void Update();

  void expansionDone();
  void selectByIndex(const QModelIndex& index);

signals:

  void midasTreeItemSelected(const MidasTreeItem* item);
  void midasCommunityTreeItemSelected(const MidasCommunityTreeItem * item);
  void midasCollectionTreeItemSelected(const MidasCollectionTreeItem * item);
  void midasItemTreeItemSelected(const MidasItemTreeItem * item);
  void midasBitstreamTreeItemSelected(const MidasBitstreamTreeItem * item);
  void midasTreeViewServerContextMenu( QContextMenuEvent * e ); 
  void midasNoTreeItemSelected();
  void fetchedMore();

  void startedExpandingTree();
  void finishedExpandingTree();

protected:
 
  void contextMenuEvent ( QContextMenuEvent * e );
  mws::WebAPI*          m_WebAPI;
  MidasTreeModelServer* m_Model;
  ExpandTreeThread*     m_ExpandTreeThread;
  QMutex                m_Mutex;
  QPoint                m_DragStart;
 };

#endif //__MidasTreeViewServer_H
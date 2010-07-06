#ifndef __MidasTreeViewServer_H
#define __MidasTreeViewServer_H

#include <QTreeView>
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
class MidasTreeModel;
class ExpandTreeThread;

namespace mws{
  class WebAPI;
}

namespace mdo{
  class Object;
}

class MidasTreeViewServer : public QTreeView
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
  void decorateByUuid(std::string uuid);
  void alertFetchedMore();
  void selectByObject(mdo::Object* object);
  void Update();

  void expansionDone();
  void selectByIndex(const QModelIndex& index);

signals:

  void midasTreeItemSelected(const MidasTreeItem* item);
  void midasCommunityTreeItemSelected(const MidasCommunityTreeItem * item);
  void midasCollectionTreeItemSelected(const MidasCollectionTreeItem * item);
  void midasItemTreeItemSelected(const MidasItemTreeItem * item);
  void midasBitstreamTreeItemSelected(const MidasBitstreamTreeItem * item);
  void MidasTreeViewServerContextMenu( QContextMenuEvent * e ); 
  void midasNoTreeItemSelected();
  void fetchedMore();

  void startedExpandingTree();
  void finishedExpandingTree();

protected:
 
  void contextMenuEvent ( QContextMenuEvent * e );
  mws::WebAPI*        m_WebAPI;
  MidasTreeModel*     m_Model;
  ExpandTreeThread*   m_ExpandTreeThread;
  QMutex              m_Mutex;
  QPoint              m_DragStart;
 };

#endif //__MidasTreeViewServer_H
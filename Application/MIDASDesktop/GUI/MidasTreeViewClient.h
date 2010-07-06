#ifndef __MidasTreeViewClient_H
#define __MidasTreeViewClient_H

#include "MidasTreeView.h"
#include <string>

class QItemSelection;
class MidasTreeItem;
class MidasCommunityTreeItem;
class MidasCollectionTreeItem;
class MidasItemTreeItem;
class MidasBitstreamTreeItem;
class QContextMenuEvent;
class MidasTreeModelClient;
class midasDatabaseProxy;
class midasLog;

class MidasTreeViewClient : public MidasTreeView
{
  Q_OBJECT

public:
  
  /** Constructor */
  MidasTreeViewClient(QWidget * parent = 0);
  ~MidasTreeViewClient();

  /** Initialize the tree */
  bool Initialize();

  void SetDatabaseProxy(midasDatabaseProxy* proxy);
  void SetLog(midasLog* log);

  /** Clear the tree */
  void Clear();

  bool isModelIndexSelected() const; 
  //const MidasTreeItem * getMidasTreeItemParent(QModelIndex& selected) const;
  //const std::string getSelectedMidasTreeItemParentId() const; 
  const QModelIndex getSelectedModelIndex() const; 
  const MidasTreeItem * getSelectedMidasTreeItem() const; 
   
  MidasItemTreeItem * selectedMidasItemTreeItem(); 

public slots:
  void Update();
  void updateSelection(const QItemSelection &selected,
    const QItemSelection &deselected);

signals:

  void midasTreeItemSelected(const MidasTreeItem* item);
  void midasCommunityTreeItemSelected(const MidasCommunityTreeItem * item);
  void midasCollectionTreeItemSelected(const MidasCollectionTreeItem * item);
  void midasItemTreeItemSelected(const MidasItemTreeItem * item);
  void midasBitstreamTreeItemSelected(const MidasBitstreamTreeItem * item);
  void midasTreeViewContextMenu( QContextMenuEvent * e ); 
  void midasNoTreeItemSelected();
  void bitstreamsDropped(const MidasItemTreeItem* parentItem, const QStringList & files);
  void bitstreamOpenRequest();
  void resourceDropped(int type, int id);

protected:
  virtual void dragEnterEvent ( QDragEnterEvent * event );
  virtual void dragLeaveEvent ( QDragLeaveEvent * event ); 
  virtual void dragMoveEvent ( QDragMoveEvent * event ); 
  virtual void dropEvent( QDropEvent * event );
  virtual void mouseDoubleClickEvent( QMouseEvent * event);
  void contextMenuEvent ( QContextMenuEvent * e );

  MidasTreeModelClient* m_Model;
 };

#endif //__MidasTreeViewClient_H
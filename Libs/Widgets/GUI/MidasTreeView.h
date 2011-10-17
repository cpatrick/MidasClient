#ifndef __MidasTreeView_H
#define __MidasTreeView_H

#include "MidasTreeViewBase.h"

#include <QItemSelection>
#include <QContextMenuEvent>
#include <QPoint>

class MidasTreeItem;
class MidasCommunityTreeItem;
class MidasCollectionTreeItem;
class MidasItemTreeItem;
class MidasBitstreamTreeItem;
class MidasTreeModel;

class midasSynchronizer;

namespace mdo
{
class Object;
}

class MidasTreeView : public MidasTreeViewBase
{
  Q_OBJECT
public:
  MidasTreeView(QWidget* parent);
  ~MidasTreeView();

  void SetSynchronizer(midasSynchronizer* synch);

  virtual void Clear();

  virtual void Initialize();

  bool isModelIndexSelected() const;

  const QModelIndex getSelectedModelIndex() const;

  const MidasTreeItem * getSelectedMidasTreeItem() const;

signals:
  void midasTreeItemSelected(const MidasTreeItem* item);

  void midasCommunityTreeItemSelected(const MidasCommunityTreeItem* item);

  void midasCollectionTreeItemSelected(const MidasCollectionTreeItem* item);

  void midasItemTreeItemSelected(const MidasItemTreeItem* item);

  void midasBitstreamTreeItemSelected(const MidasBitstreamTreeItem* item);

  void midasNoTreeItemSelected();

  void midasTreeViewContextMenu(QContextMenuEvent* e);

  void resourceDropped(int type, int id);

  void fetchedSelectedResource();

public slots:
  virtual void Update();

  virtual void updateSelection(const QItemSelection& selected, const QItemSelection& deselected);

  virtual void decorateByUuid(std::string uuid);

  virtual void addResource(mdo::Object *);

  virtual void updateResource(mdo::Object *);

  virtual void deleteResource(mdo::Object *);

protected:
  virtual void fetchItemData(MidasTreeItem* item) = 0;

  virtual void contextMenuEvent(QContextMenuEvent* event);

  virtual void dragEnterEvent(QDragEnterEvent* event);

  virtual void dragLeaveEvent(QDragLeaveEvent* event);

  virtual void dragMoveEvent(QDragMoveEvent* event);

  virtual void dropEvent(QDropEvent* event);

  virtual void mouseDoubleClickEvent(QMouseEvent* event);

  virtual void mousePressEvent(QMouseEvent* event);

  virtual void mouseMoveEvent(QMouseEvent* event);

  MidasTreeModel*    m_Model;
  midasSynchronizer* m_Synch;
  QPoint             m_DragStart;
  QString            m_MimeType;
  QString            m_AcceptMimeType;
};

#endif

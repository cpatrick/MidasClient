#ifndef __MidasTreeView_H
#define __MidasTreeView_H

#include <QTreeView>
#include <QItemSelection>
#include <QContextMenuEvent>

class MidasTreeItem;
class MidasCommunityTreeItem;
class MidasCollectionTreeItem;
class MidasItemTreeItem;
class MidasBitstreamTreeItem;
class MidasTreeModel;

class midasSynchronizer;

namespace mdo{
  class Object;
}

class MidasTreeView : public QTreeView
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
  const MidasTreeItem* getSelectedMidasTreeItem() const;

signals:
  void midasTreeItemSelected(const MidasTreeItem* item);
  void midasCommunityTreeItemSelected(const MidasCommunityTreeItem* item);
  void midasCollectionTreeItemSelected(const MidasCollectionTreeItem* item);
  void midasItemTreeItemSelected(const MidasItemTreeItem* item);
  void midasBitstreamTreeItemSelected(const MidasBitstreamTreeItem* item);
  void midasNoTreeItemSelected();

  void midasTreeViewContextMenu( QContextMenuEvent* e );

public slots:
  virtual void Update();
  virtual void updateSelection(const QItemSelection& selected,
    const QItemSelection& deselected);
  virtual void decorateByUuid(std::string uuid);
  virtual void addResource(mdo::Object*) {}
  virtual void updateResource(mdo::Object*) {}
  virtual void deleteResource(mdo::Object*) {}

protected:
  virtual void contextMenuEvent (QContextMenuEvent* e);
  virtual void fetchItemData(MidasTreeItem* item) = 0;

  MidasTreeModel*    m_Model;
  midasSynchronizer* m_Synch;
};

#endif

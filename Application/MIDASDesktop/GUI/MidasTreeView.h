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

class MidasTreeView : public QTreeView
{
  Q_OBJECT

public:
  MidasTreeView(QWidget* parent) : QTreeView(parent) {}
  ~MidasTreeView() {}

  virtual void Clear();
  virtual void Initialize();

  bool isModelIndexSelected() const;
  const QModelIndex getSelectedModelIndex() const;
  const MidasTreeItem* getSelectedMidasTreeItem() const;

signals:
  void midasTreeItemSelected(const MidasTreeItem* item);
  void midasCommunityTreeItemSelected(const MidasCommunityTreeItem * item);
  void midasCollectionTreeItemSelected(const MidasCollectionTreeItem * item);
  void midasItemTreeItemSelected(const MidasItemTreeItem * item);
  void midasBitstreamTreeItemSelected(const MidasBitstreamTreeItem * item);
  void midasNoTreeItemSelected();

  void midasTreeViewContextMenu( QContextMenuEvent * e );

public slots:
  virtual void Update();
  virtual void updateSelection(const QItemSelection &selected,
    const QItemSelection &deselected);

protected:
  virtual void contextMenuEvent (QContextMenuEvent * e);
  virtual void fetchItemData(MidasTreeItem* item) = 0;

  MidasTreeModel* m_Model;
};

#endif

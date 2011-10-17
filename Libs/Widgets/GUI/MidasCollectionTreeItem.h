#ifndef __MidasCollectionTreeItem_H
#define __MidasCollectionTreeItem_H

#include "MidasTreeItem.h"
#include "mdoCollection.h"

class MidasCollectionTreeItem : public MidasTreeItem
{
  Q_OBJECT
public:
  MidasCollectionTreeItem(const QList<QVariant> & itemData, MidasTreeModel* model, MidasTreeItem *parent = 0);
  ~MidasCollectionTreeItem();
  virtual void Populate(QModelIndex index);

  int GetType() const;

  int GetId() const;

  std::string GetUuid() const;

  void UpdateDisplayName();

  void RemoveFromTree();

  void SetCollection(mdo::Collection* collection);

  mdo::Collection * GetCollection() const;

  mdo::Object * GetObject() const;

  /** Whether the underlying resource info has been fetched */
  bool ResourceIsFetched() const;

private:

  mdo::Collection* m_Collection;
};

#endif // __MidasCollectionTreeItem_H

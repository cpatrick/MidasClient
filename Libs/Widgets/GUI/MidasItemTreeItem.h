#ifndef __MidasItemTreeItem_H
#define __MidasItemTreeItem_H

#include "MidasTreeItem.h"
#include "mdoItem.h"

class MidasItemTreeItem : public MidasTreeItem
{
Q_OBJECT

public:
  MidasItemTreeItem(const QList<QVariant> &itemData, MidasTreeModel* model, MidasTreeItem *parent = 0); 
  virtual ~MidasItemTreeItem();
  virtual void Populate(QModelIndex parent);

  int GetType() const;
  int GetId() const;
  std::string GetUuid() const;
  void UpdateDisplayName();
  void RemoveFromTree();

  void SetItem(mdo::Item* item);
  mdo::Item* GetItem() const;
  mdo::Object* GetObject() const;

  /** Whether the underlying resource info has been fetched */
  bool ResourceIsFetched() const;

private:

  mdo::Item* m_Item;
  
}; 

#endif //__MidasItemTreeItem_H

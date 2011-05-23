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
  virtual void populate(QModelIndex parent);

  int getType() const;
  int getId() const;
  std::string getUuid() const;
  void updateDisplayName();
  void removeFromTree();

  void setItem(mdo::Item* item) {m_Item = item;}
  mdo::Item* getItem() const {return m_Item;}
  mdo::Object* getObject() const { return m_Item; }

private:

  mdo::Item* m_Item;
  
}; 

#endif //__MidasItemTreeItem_H

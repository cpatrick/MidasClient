#ifndef __MidasItemTreeItem_H
#define __MidasItemTreeItem_H

#include "MidasTreeItem.h"

namespace mdo
{
  class Item;
}

class MidasItemTreeItem : public MidasTreeItem
{
Q_OBJECT

public:
  MidasItemTreeItem(const QList<QVariant> &itemData, MidasTreeItem *parent = 0); 
  virtual ~MidasItemTreeItem();
  virtual void populate();

  int getType() const;
  int getId() const;
  std::string getUuid() const;

  void setItem(mdo::Item* item) {m_Item = item;}
  mdo::Item* getItem() const {return m_Item;}

private:

  mdo::Item* m_Item;
  
}; 

#endif //__MidasItemTreeItem_H

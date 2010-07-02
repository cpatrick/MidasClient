#ifndef __MidasCollectionTreeItem_H
#define __MidasCollectionTreeItem_H

#include "MidasTreeItem.h"

namespace mdo
{
  class Collection;
}

class MidasCollectionTreeItem : public MidasTreeItem
{
  Q_OBJECT

public:
  MidasCollectionTreeItem(const QList<QVariant> &itemData, MidasTreeItem *parent = 0);
  ~MidasCollectionTreeItem();
  virtual void populate();

  int getType() const;
  int getId() const;
  std::string getUuid() const;

  void setCollection(mdo::Collection* collection) {m_Collection = collection;}
  mdo::Collection* getCollection() const {return m_Collection;}

private:

  mdo::Collection* m_Collection;
}; 

#endif //__MidasCollectionTreeItem_H

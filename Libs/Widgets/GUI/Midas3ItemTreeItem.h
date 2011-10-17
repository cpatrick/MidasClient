#ifndef __Midas3ItemTreeItem_H
#define __Midas3ItemTreeItem_H

#include "Midas3TreeItem.h"

namespace m3do {
  class Item;
}
class Midas3TreeModel;

class Midas3ItemTreeItem : public Midas3TreeItem
{
  Q_OBJECT

public:

  Midas3ItemTreeItem(const QList<QVariant>& itemData,
                       Midas3TreeModel* model, Midas3TreeItem* parent = 0);
  ~Midas3ItemTreeItem();
  virtual void populate(QModelIndex parent);

  int getType() const;
  int getId() const;
  std::string getUuid() const;
  std::string getPath() const;
  void updateDisplayName();
  void removeFromTree();
  
  void setItem(m3do::Item* item);
  m3do::Item* getItem() const;
  mdo::Object* getObject() const;

  /** Whether the underlying resource info has been fetched */
  bool resourceIsFetched() const;

  QPixmap getDecoration();

private:
  m3do::Item* m_Item;

}; 

#endif //__MidasItemTreeItem_H

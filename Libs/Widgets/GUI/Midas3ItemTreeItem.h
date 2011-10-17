#ifndef __Midas3ItemTreeItem_H
#define __Midas3ItemTreeItem_H

#include "Midas3TreeItem.h"

namespace m3do
{
class Item;
}
class Midas3TreeModel;

class Midas3ItemTreeItem : public Midas3TreeItem
{
  Q_OBJECT
public:

  Midas3ItemTreeItem(const QList<QVariant>& itemData, Midas3TreeModel* model, Midas3TreeItem* parent = 0);
  ~Midas3ItemTreeItem();
  virtual void Populate(QModelIndex parent);

  int GetType() const;

  int GetId() const;

  std::string GetUuid() const;

  std::string GetPath() const;

  void UpdateDisplayName();

  void RemoveFromTree();

  void SetItem(m3do::Item* item);

  m3do::Item * GetItem() const;

  mdo::Object * GetObject() const;

  /** Whether the underlying resource info has been fetched */
  bool ResourceIsFetched() const;

  QPixmap GetDecoration();

private:
  m3do::Item* m_Item;

};

#endif // __Midas3ItemTreeItem_H

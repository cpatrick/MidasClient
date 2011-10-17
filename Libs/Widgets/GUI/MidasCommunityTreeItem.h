#ifndef __MidasCommunityTreeItem_H
#define __MidasCommunityTreeItem_H

#include "MidasTreeItem.h"
#include "mdoCommunity.h"

class MidasCommunityTreeItem : public MidasTreeItem
{
  Q_OBJECT
public:

  MidasCommunityTreeItem(const QList<QVariant>& itemData, MidasTreeModel* model, MidasTreeItem* parent = 0);
  ~MidasCommunityTreeItem();
  virtual void Populate(QModelIndex parent);

  int GetType() const;

  int GetId() const;

  std::string GetUuid() const;

  void UpdateDisplayName();

  void RemoveFromTree();

  void SetCommunity(mdo::Community* community);

  mdo::Community * GetCommunity() const;

  mdo::Object * GetObject() const;

  /** Whether the underlying resource info has been fetched */
  bool ResourceIsFetched() const;

private:

  mdo::Community* m_Community;

};

#endif // __MidasCommunityTreeItem_H

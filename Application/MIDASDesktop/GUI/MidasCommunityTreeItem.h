#ifndef __MidasCommunityTreeItem_H
#define __MidasCommunityTreeItem_H

#include "MidasTreeItem.h"

namespace mdo {
  class Community;
}

class MidasCommunityTreeItem : public MidasTreeItem
{
  Q_OBJECT

public:

  MidasCommunityTreeItem(const QList<QVariant> &itemData, MidasTreeModel* model, MidasTreeItem *parent = 0);
  ~MidasCommunityTreeItem();
  virtual void populate(QModelIndex parent);

  int getType() const;
  int getId() const;
  std::string getUuid() const;
  void updateDisplayName();
  
  void setCommunity(mdo::Community* community);
  mdo::Community* getCommunity() const {return m_Community;}

private:

  mdo::Community* m_Community;
  
}; 

#endif //__MidasCommunityTreeItem_H

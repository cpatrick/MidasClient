#ifndef __Midas3FolderTreeItem_H
#define __Midas3FolderTreeItem_H

#include "Midas3TreeItem.h"

namespace m3do {
  class Folder;
}
class Midas3TreeModel;

class Midas3FolderTreeItem : public Midas3TreeItem
{
  Q_OBJECT

public:

  Midas3FolderTreeItem(const QList<QVariant>& itemData,
                       Midas3TreeModel* model, Midas3TreeItem* parent = 0);
  ~Midas3FolderTreeItem();
  virtual void populate(QModelIndex parent);

  int getType() const;
  int getId() const;
  std::string getUuid() const;
  std::string getPath() const;
  void updateDisplayName();
  void removeFromTree();
  
  void setFolder(m3do::Folder* Folder);
  m3do::Folder* getFolder() const;
  mdo::Object* getObject() const;

  /** Whether the underlying resource info has been fetched */
  bool resourceIsFetched() const;

  QPixmap getDecoration();

private:
  m3do::Folder* m_Folder;

}; 

#endif //__MidasFolderTreeItem_H

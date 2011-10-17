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
  virtual void Populate(QModelIndex parent);

  int GetType() const;
  int GetId() const;
  std::string GetUuid() const;
  std::string GetPath() const;
  void UpdateDisplayName();
  void RemoveFromTree();
  
  void SetFolder(m3do::Folder* Folder);
  m3do::Folder* GetFolder() const;
  mdo::Object* GetObject() const;

  /** Whether the underlying resource info has been fetched */
  bool ResourceIsFetched() const;

  QPixmap GetDecoration();

private:
  m3do::Folder* m_Folder;

}; 

#endif //__MidasFolderTreeItem_H

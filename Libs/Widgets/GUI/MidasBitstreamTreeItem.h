#ifndef __MidasBitstreamTreeItem_H
#define __MidasBitstreamTreeItem_H

#include "MidasTreeItem.h"
#include "mdoBitstream.h"

class MidasBitstreamTreeItem : public MidasTreeItem
{
Q_OBJECT

public:
  MidasBitstreamTreeItem(const QList<QVariant> &itemData, MidasTreeModel* model, MidasTreeItem *parent = 0); 
  virtual ~MidasBitstreamTreeItem();
  
  virtual QPixmap GetDecoration();

  int GetType() const;
  int GetId() const;
  std::string GetUuid() const;
  void UpdateDisplayName();
  void RemoveFromTree();

  void SetBitstream(mdo::Bitstream* bitstream);
  mdo::Bitstream* GetBitstream() const;
  mdo::Object* GetObject() const;

  virtual void Populate(QModelIndex parent);

  /** Whether the underlying resource info has been fetched */
  bool ResourceIsFetched() const;
  
private:

  mdo::Bitstream* m_Bitstream;
  
}; 

#endif //__MidasBitstreamTreeItem_H

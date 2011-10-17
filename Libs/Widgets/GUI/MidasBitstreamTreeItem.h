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
  
  virtual QPixmap getDecoration();

  int getType() const;
  int getId() const;
  std::string getUuid() const;
  void updateDisplayName();
  void removeFromTree();

  void setBitstream(mdo::Bitstream* bitstream) {m_Bitstream = bitstream;}
  mdo::Bitstream* getBitstream() const {return m_Bitstream;}
  mdo::Object* getObject() const { return m_Bitstream; }

  virtual void populate(QModelIndex parent);

  /** Whether the underlying resource info has been fetched */
  bool resourceIsFetched() const { return m_Bitstream->IsFetched(); }
  
private:

  mdo::Bitstream* m_Bitstream;
  
}; 

#endif //__MidasBitstreamTreeItem_H

#ifndef __MidasBitstreamTreeItem_H
#define __MidasBitstreamTreeItem_H

#include "MidasTreeItem.h"

namespace mdo
{
  class Bitstream;
}

class MidasBitstreamTreeItem : public MidasTreeItem
{
Q_OBJECT

public:
  MidasBitstreamTreeItem(const QList<QVariant> &itemData, MidasTreeItem *parent = 0); 
  virtual ~MidasBitstreamTreeItem();
  
  virtual QPixmap getDecoration();

  int getType() const;
  int getId() const;
  std::string getUuid() const;

  void setBitstream(mdo::Bitstream* bitstream) {m_Bitstream = bitstream;}
  mdo::Bitstream* getBitstream() const {return m_Bitstream;}
  
private:

  mdo::Bitstream* m_Bitstream;
  
}; 

#endif //__MidasBitstreamTreeItem_H

#ifndef __Midas3BitstreamTreeItem_H
#define __Midas3BitstreamTreeItem_H

#include "Midas3TreeItem.h"

namespace m3do {
  class Bitstream;
}
class Midas3TreeModel;

class Midas3BitstreamTreeItem : public Midas3TreeItem
{
  Q_OBJECT

public:

  Midas3BitstreamTreeItem(const QList<QVariant>& bitstreamData,
                       Midas3TreeModel* model, Midas3TreeItem* parent = 0);
  ~Midas3BitstreamTreeItem();
  virtual void populate(QModelIndex parent);

  int getType() const;
  int getId() const;
  std::string getUuid() const;
  std::string getPath() const;
  void updateDisplayName();
  void removeFromTree();
  
  void setBitstream(m3do::Bitstream* bitstream);
  m3do::Bitstream* getBitstream() const;
  mdo::Object* getObject() const;

  /** Whether the underlying resource info has been fetched */
  bool resourceIsFetched() const;

  QPixmap getDecoration();

private:
  m3do::Bitstream* m_Bitstream;

}; 

#endif //__MidasBitstreamTreeItem_H

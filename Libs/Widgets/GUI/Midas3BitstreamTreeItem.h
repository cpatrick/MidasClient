#ifndef __Midas3BitstreamTreeItem_H
#define __Midas3BitstreamTreeItem_H

#include "Midas3TreeItem.h"

namespace m3do
{
class Bitstream;
}
class Midas3TreeModel;

class Midas3BitstreamTreeItem : public Midas3TreeItem
{
  Q_OBJECT
public:

  Midas3BitstreamTreeItem(const QList<QVariant>& bitstreamData, Midas3TreeModel* model, Midas3TreeItem* parent = 0);
  ~Midas3BitstreamTreeItem();
  virtual void Populate(QModelIndex parent);

  int GetType() const;

  int GetId() const;

  std::string GetUuid() const;

  std::string GetPath() const;

  void UpdateDisplayName();

  void RemoveFromTree();

  void SetBitstream(m3do::Bitstream* bitstream);

  m3do::Bitstream * GetBitstream() const;

  mdo::Object * GetObject() const;

  /** Whether the underlying resource info has been fetched */
  bool ResourceIsFetched() const;

  QPixmap GetDecoration();

private:
  m3do::Bitstream* m_Bitstream;

};

#endif // __MidasBitstreamTreeItem_H

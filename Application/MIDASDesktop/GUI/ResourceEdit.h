#ifndef __ResourceEdit_H
#define __ResourceEdit_H

#include "midasLogAware.h"
#include "MidasResourceDescTable.h"
#include <QObject>

class midasDatabaseProxy;
class QTableWidgetItem;

namespace mdo {
  class Community;
  class Collection;
  class Item;
  class Bitstream;
};

class ResourceEdit : public QObject, public midasLogAware
{
  Q_OBJECT
public:

  ResourceEdit(midasDatabaseProxy* database);
  ~ResourceEdit();

  void Save(QTableWidgetItem* row);

signals:
  void DataModified(std::string uuid);

protected:

  void SaveCommunity(mdo::Community*, MIDASFields, std::string data);
  void SaveCollection(mdo::Collection*, MIDASFields, std::string data);
  void SaveItem(mdo::Item*, MIDASFields, std::string data);
  void SaveBitstream(mdo::Bitstream*, MIDASFields, std::string data);

  midasDatabaseProxy* m_database;
};

#endif

#ifndef __Midas3TreeModelClient_H
#define __Midas3TreeModelClient_H

#include "midasStandardIncludes.h"
#include "Midas3TreeModel.h"
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <string>
#include <vector>

class Midas3FolderTreeItem;
class Midas3ItemTreeItem;

class Midas3TreeModelClient : public Midas3TreeModel
{
  Q_OBJECT
public:

  Midas3TreeModelClient(QObject *parent = 0);
  ~Midas3TreeModelClient();

  void Populate();

  void addResource(mdo::Object *);

  void updateResource(mdo::Object *);

  void deleteResource(mdo::Object *);

  void fetchMore(const QModelIndex& parent);

  void fetchFolder(Midas3FolderTreeItem* parent);

  void fetchItem(Midas3ItemTreeItem* parent);

};

#endif // __Midas3TreeModelClient_H

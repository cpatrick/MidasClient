#ifndef __TreeViewUpdateHandler_H
#define __TreeViewUpdateHandler_H

#include "mdsResourceUpdateHandler.h"
#include <QObject>

class MidasTreeView;

/**
 * Passes update events to the tree view
 */
class TreeViewUpdateHandler : public QObject, public mds::ResourceUpdateHandler
{
  Q_OBJECT
public:
  TreeViewUpdateHandler(MidasTreeView* view);
  ~TreeViewUpdateHandler();

  void AddedResource(mdo::Object* resource);
  void DeletedResource(mdo::Object* resource);
  void UpdatedResource(mdo::Object* resource);
signals:
  void ResourceAdded(mdo::Object* resource);
  void ResourceDeleted(mdo::Object* resource);
  void ResourceUpdated(mdo::Object* resource);
protected:
  MidasTreeView* m_View;
};

#endif

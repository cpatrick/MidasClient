#ifndef __Midas3TreeItem_H
#define __Midas3TreeItem_H

#include "Midas3TreeModel.h"
#include <QList>
#include <QVariant>
#include <QFlags>
#include <QModelIndex>

class Midas3FolderTreeItem;

namespace mdo
{
  class Object;
}

class Midas3TreeItem: public QObject
{
Q_OBJECT

public:

  enum DecorationRole
    {
    Collapsed = 0x0,
    Expanded = 0x1,
    Dirty = 0x2,
    };
  Q_DECLARE_FLAGS(DecorationRoles, DecorationRole)

  Midas3TreeItem(const QList<QVariant> &itemData, Midas3TreeModel* model, Midas3TreeItem *parent = 0);
  virtual ~Midas3TreeItem();

  virtual void populate(QModelIndex parent) = 0;

  bool operator==(const Midas3TreeItem* other) const;

  void appendChild(Midas3TreeItem *child);
  void removeChild(Midas3TreeItem *item); 
  void removeAllChild(); 

  Midas3TreeItem* child(int row);
  QList<Midas3TreeItem*> getChildren() { return childItems; }

  bool isFetchedChildren() const;
  void setFetchedChildren(bool value);
  bool isDynamicFetch() const;
  void setDynamicFetch(bool value);

  int columnCount() const;
  void setData(const QVariant& value, int column = 0);
  QVariant data(int column) const;
  int row() const;
  Midas3TreeItem* parent();
  const Midas3TreeItem* parent() const;
  bool isValid() const;

  bool isClientResource() const { return m_ClientResource; }
  void setClientResource(bool val) { m_ClientResource = val; }

  /** Whether the underlying resource info has been fetched */
  virtual bool resourceIsFetched() const = 0;

  virtual int getId() const = 0;
  virtual int getType() const = 0;
  virtual std::string getUuid() const = 0;
  virtual std::string getPath() const = 0;
  virtual int childCount() const;

  /** Reset the name based on the underlying resource name */
  virtual void updateDisplayName() = 0;
  virtual void removeFromTree() = 0;

  virtual mdo::Object* getObject() const = 0;

  virtual QPixmap getDecoration();
  void setDecorationRole(DecorationRoles role);

  void setTopLevelFolders(QList<Midas3FolderTreeItem*>* tlc) { m_TopLevelFolders = tlc; }

protected:
  DecorationRoles decorationRole;
  Midas3TreeModel* m_Model;
  QList<Midas3TreeItem*> childItems;
  QList<QVariant> itemData;
  Midas3TreeItem* parentItem;
  QList<Midas3FolderTreeItem*>* m_TopLevelFolders;
  uint timestamp;
  uint lifespan;
  bool fetchedChildren;
  bool dynamicFetch;
  bool m_ClientResource;
};
Q_DECLARE_OPERATORS_FOR_FLAGS( Midas3TreeItem::DecorationRoles )

#endif //__MidasTreeItem_H

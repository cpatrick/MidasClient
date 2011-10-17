#ifndef __MidasTreeItem_H
#define __MidasTreeItem_H

#include "MidasTreeModel.h"
#include <QList>
#include <QVariant>
#include <QFlags>
#include <QModelIndex>

class MidasCommunityTreeItem;

namespace mdo
{
  class Object;
}

class MidasTreeItem: public QObject
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

  MidasTreeItem(const QList<QVariant> &itemData, MidasTreeModel* model, MidasTreeItem *parent = 0);
  virtual ~MidasTreeItem();

  virtual void populate(QModelIndex parent) = 0;

  bool operator==(const MidasTreeItem* other) const;

  void appendChild(MidasTreeItem *child);
  void removeChild(MidasTreeItem *item); 
  void removeAllChild(); 

  MidasTreeItem *child(int row);
  QList<MidasTreeItem*> getChildren() { return childItems; }

  bool isFetchedChildren() const;
  void setFetchedChildren(bool value);
  bool isDynamicFetch() const;
  void setDynamicFetch(bool value);

  int columnCount() const;
  void setData(const QVariant& value, int column = 0);
  QVariant data(int column) const;
  int row() const;
  MidasTreeItem *parent();
  const MidasTreeItem *parent() const;
  bool isValid() const;

  bool isClientResource() const { return m_ClientResource; }
  void setClientResource(bool val) { m_ClientResource = val; }

  /** Whether the underlying resource info has been fetched */
  virtual bool resourceIsFetched() const = 0;

  virtual int getId() const = 0;
  virtual int getType() const = 0;
  virtual std::string getUuid() const = 0;
  virtual int childCount() const;

  /** Reset the name based on the underlying resource name */
  virtual void updateDisplayName() = 0;
  virtual void removeFromTree() = 0;

  virtual mdo::Object* getObject() const = 0;

  virtual QPixmap getDecoration();
  void setDecorationRole(DecorationRoles role);

  void setTopLevelCommunities(QList<MidasCommunityTreeItem*>* tlc) { m_TopLevelCommunities = tlc; }

protected:
  DecorationRoles decorationRole;
  MidasTreeModel* m_Model;
  QList<MidasTreeItem*> childItems;
  QList<QVariant> itemData;
  MidasTreeItem *parentItem;
  QList<MidasCommunityTreeItem*>* m_TopLevelCommunities;
  uint timestamp;
  uint lifespan;
  bool fetchedChildren;
  bool dynamicFetch;
  bool m_ClientResource;
};
Q_DECLARE_OPERATORS_FOR_FLAGS( MidasTreeItem::DecorationRoles )

#endif //__MidasTreeItem_H

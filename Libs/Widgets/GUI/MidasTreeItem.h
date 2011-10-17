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

class MidasTreeItem : public QObject
{
  Q_OBJECT
public:

  enum DecorationRole
    {
    Collapsed = 0x0,
    Expanded = 0x1,
    Dirty = 0x2,
    };
  Q_DECLARE_FLAGS(DecorationRoles, DecorationRole) MidasTreeItem(const QList<QVariant> & itemData,
                                                                 MidasTreeModel * model,
                                                                 MidasTreeItem * parent = 0);
  virtual ~MidasTreeItem();

  virtual void Populate(QModelIndex parent) = 0;

  bool operator==(const MidasTreeItem* other) const;

  void AppendChild(MidasTreeItem *child);

  void RemoveChild(MidasTreeItem *item);

  void RemoveAllChildren();

  MidasTreeItem * GetChild(int row);

  QList<MidasTreeItem *> GetChildren();

  bool IsFetchedChildren() const;

  void SetFetchedChildren(bool value);

  bool IsDynamicFetch() const;

  void SetDynamicFetch(bool value);

  int ColumnCount() const;

  void SetData(const QVariant& value, int column = 0);

  QVariant GetData(int column) const;

  int GetRow() const;

  MidasTreeItem * GetParent();

  const MidasTreeItem * GetParent() const;

  bool IsValid() const;

  bool IsClientResource() const;

  void SetClientResource(bool val);

  /** Whether the underlying resource info has been fetched */
  virtual bool ResourceIsFetched() const = 0;

  virtual int GetId() const = 0;

  virtual int GetType() const = 0;

  virtual std::string GetUuid() const = 0;

  virtual int ChildCount() const;

  /** Reset the name based on the underlying resource name */
  virtual void UpdateDisplayName() = 0;

  virtual void RemoveFromTree() = 0;

  virtual mdo::Object * GetObject() const = 0;

  virtual QPixmap GetDecoration();

  void SetDecorationRole(DecorationRoles role);

  void SetTopLevelCommunities(QList<MidasCommunityTreeItem *>* tlc);

protected:
  DecorationRoles                  m_DecorationRole;
  MidasTreeModel*                  m_Model;
  QList<MidasTreeItem *>           m_ChildItems;
  QList<QVariant>                  m_ItemData;
  MidasTreeItem*                   m_ParentItem;
  QList<MidasCommunityTreeItem *>* m_TopLevelCommunities;
  uint                             m_Timestamp;
  uint                             m_Lifespan;
  bool                             m_FetchedChildren;
  bool                             m_DynamicFetch;
  bool                             m_ClientResource;
};
Q_DECLARE_OPERATORS_FOR_FLAGS( MidasTreeItem::DecorationRoles )

#endif // __MidasTreeItem_H

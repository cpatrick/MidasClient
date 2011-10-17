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

class Midas3TreeItem : public QObject
{
  Q_OBJECT
public:

  enum DecorationRole
    {
    Collapsed = 0x0,
    Expanded = 0x1,
    Dirty = 0x2,
    };
  Q_DECLARE_FLAGS(DecorationRoles, DecorationRole) Midas3TreeItem(const QList<QVariant> & itemData,
                                                                  Midas3TreeModel * model,
                                                                  Midas3TreeItem *parent = 0);
  virtual ~Midas3TreeItem();

  virtual void Populate(QModelIndex parent) = 0;

  bool operator==(const Midas3TreeItem* other) const;

  void AppendChild(Midas3TreeItem *child);

  void RemoveChild(Midas3TreeItem *item);

  void RemoveAllChildren();

  Midas3TreeItem * GetChild(int row);

  QList<Midas3TreeItem *> GetChildren();

  bool IsFetchedChildren() const;

  void SetFetchedChildren(bool value);

  bool IsDynamicFetch() const;

  void SetDynamicFetch(bool value);

  int ColumnCount() const;

  void SetData(const QVariant& value, int column = 0);

  QVariant GetData(int column) const;

  int GetRow() const;

  Midas3TreeItem * GetParent();

  const Midas3TreeItem * GetParent() const;

  bool IsValid() const;

  bool IsClientResource() const;

  void SetClientResource(bool val);

  /** Whether the underlying resource info has been fetched */
  virtual bool ResourceIsFetched() const = 0;

  virtual int GetId() const = 0;

  virtual int GetType() const = 0;

  virtual std::string GetUuid() const = 0;

  virtual std::string GetPath() const = 0;

  virtual int ChildCount() const;

  /** Reset the name based on the underlying resource name */
  virtual void UpdateDisplayName() = 0;

  virtual void RemoveFromTree() = 0;

  virtual mdo::Object * GetObject() const = 0;

  virtual QPixmap GetDecoration();

  void SetDecorationRole(DecorationRoles role);

  void SetTopLevelFolders(QList<Midas3FolderTreeItem *>* tlf);

protected:
  DecorationRoles                m_DecorationRole;
  Midas3TreeModel*               m_Model;
  QList<Midas3TreeItem *>        m_ChildItems;
  QList<QVariant>                m_ItemData;
  Midas3TreeItem*                m_ParentItem;
  QList<Midas3FolderTreeItem *>* m_TopLevelFolders;
  uint                           m_Timestamp;
  uint                           m_Lifespan;
  bool                           m_FetchedChildren;
  bool                           m_DynamicFetch;
  bool                           m_ClientResource;
};
Q_DECLARE_OPERATORS_FOR_FLAGS( Midas3TreeItem::DecorationRoles )

#endif // __MidasTreeItem_H

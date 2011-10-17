/******************************************************************************
 * Copyright 2011 Kitware Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
#ifndef __MidasCollectionTreeItem_H
#define __MidasCollectionTreeItem_H

#include "MidasTreeItem.h"
#include "mdoCollection.h"

class MidasCollectionTreeItem : public MidasTreeItem
{
  Q_OBJECT
public:
  MidasCollectionTreeItem(const QList<QVariant> & itemData, MidasTreeModel* model, MidasTreeItem *parent = 0);
  ~MidasCollectionTreeItem();
  virtual void Populate(QModelIndex index);

  int GetType() const;

  int GetId() const;

  std::string GetUuid() const;

  void UpdateDisplayName();

  void RemoveFromTree();

  void SetCollection(mdo::Collection* collection);

  mdo::Collection * GetCollection() const;

  mdo::Object * GetObject() const;

  /** Whether the underlying resource info has been fetched */
  bool ResourceIsFetched() const;

private:

  mdo::Collection* m_Collection;
};

#endif // __MidasCollectionTreeItem_H

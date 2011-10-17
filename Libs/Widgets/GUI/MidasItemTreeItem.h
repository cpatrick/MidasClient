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
#ifndef __MidasItemTreeItem_H
#define __MidasItemTreeItem_H

#include "MidasTreeItem.h"
#include "mdoItem.h"

class MidasItemTreeItem : public MidasTreeItem
{
  Q_OBJECT
public:
  MidasItemTreeItem(const QList<QVariant> & itemData, MidasTreeModel* model, MidasTreeItem *parent = 0);
  virtual ~MidasItemTreeItem();
  virtual void Populate(QModelIndex parent);

  int GetType() const;

  int GetId() const;

  std::string GetUuid() const;

  void UpdateDisplayName();

  void RemoveFromTree();

  void SetItem(mdo::Item* item);

  mdo::Item * GetItem() const;

  mdo::Object * GetObject() const;

  /** Whether the underlying resource info has been fetched */
  bool ResourceIsFetched() const;

private:

  mdo::Item* m_Item;

};

#endif // __MidasItemTreeItem_H

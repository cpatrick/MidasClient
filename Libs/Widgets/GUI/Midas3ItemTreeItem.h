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
#ifndef __Midas3ItemTreeItem_H
#define __Midas3ItemTreeItem_H

#include "Midas3TreeItem.h"

namespace m3do
{
class Item;
}
class Midas3TreeModel;

class Midas3ItemTreeItem : public Midas3TreeItem
{
  Q_OBJECT
public:

  Midas3ItemTreeItem(const QList<QVariant>& itemData, Midas3TreeModel* model, Midas3TreeItem* parent = 0);
  ~Midas3ItemTreeItem();
  virtual void Populate(QModelIndex parent);

  int GetType() const;

  int GetId() const;

  std::string GetUuid() const;

  std::string GetPath() const;

  void UpdateDisplayName();

  void RemoveFromTree();

  void SetItem(m3do::Item* item);

  m3do::Item * GetItem() const;

  mdo::Object * GetObject() const;

  /** Whether the underlying resource info has been fetched */
  bool ResourceIsFetched() const;

  QPixmap GetDecoration();

private:
  m3do::Item* m_Item;

};

#endif // __Midas3ItemTreeItem_H

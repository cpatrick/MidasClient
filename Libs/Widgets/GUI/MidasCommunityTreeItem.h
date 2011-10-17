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
#ifndef __MidasCommunityTreeItem_H
#define __MidasCommunityTreeItem_H

#include "MidasTreeItem.h"
#include "mdoCommunity.h"

class MidasCommunityTreeItem : public MidasTreeItem
{
  Q_OBJECT
public:

  MidasCommunityTreeItem(const QList<QVariant>& itemData, MidasTreeModel* model, MidasTreeItem* parent = 0);
  ~MidasCommunityTreeItem();
  virtual void Populate(QModelIndex parent);

  int GetType() const;

  int GetId() const;

  std::string GetUuid() const;

  void UpdateDisplayName();

  void RemoveFromTree();

  void SetCommunity(mdo::Community* community);

  mdo::Community * GetCommunity() const;

  mdo::Object * GetObject() const;

  /** Whether the underlying resource info has been fetched */
  bool ResourceIsFetched() const;

private:

  mdo::Community* m_Community;

};

#endif // __MidasCommunityTreeItem_H

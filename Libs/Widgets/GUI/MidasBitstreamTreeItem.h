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
#ifndef __MidasBitstreamTreeItem_H
#define __MidasBitstreamTreeItem_H

#include "MidasTreeItem.h"
#include "mdoBitstream.h"

class MidasBitstreamTreeItem : public MidasTreeItem
{
  Q_OBJECT
public:
  MidasBitstreamTreeItem(const QList<QVariant> & itemData, MidasTreeModel* model, MidasTreeItem *parent = 0);
  virtual ~MidasBitstreamTreeItem();

  virtual QPixmap GetDecoration();

  int GetType() const;

  int GetId() const;

  std::string GetUuid() const;

  void UpdateDisplayName();

  void RemoveFromTree();

  void SetBitstream(mdo::Bitstream* bitstream);

  mdo::Bitstream * GetBitstream() const;

  mdo::Object * GetObject() const;

  virtual void Populate(QModelIndex parent);

  /** Whether the underlying resource info has been fetched */
  bool ResourceIsFetched() const;

private:

  mdo::Bitstream* m_Bitstream;

};

#endif // __MidasBitstreamTreeItem_H

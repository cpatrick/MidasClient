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
#ifndef __ResourceEdit_H
#define __ResourceEdit_H

#include "midasLogAware.h"
#include "MidasResourceDescTable.h"
#include <QObject>

class QTableWidgetItem;

namespace mdo
{
class Community;
class Collection;
class Item;
class Bitstream;
};

class ResourceEdit : public QObject, public midasLogAware
{
  Q_OBJECT
public:

  ResourceEdit();
  ~ResourceEdit();

  void Save(QTableWidgetItem* row);

signals:
  void DataModified(std::string uuid);

protected:

  void SaveCommunity(mdo::Community *, MIDASFields, std::string data);
  void SaveCollection(mdo::Collection *, MIDASFields, std::string data);
  void SaveItem(mdo::Item *, MIDASFields, std::string data);
  void SaveBitstream(mdo::Bitstream *, MIDASFields, std::string data);
};

#endif

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
#ifndef __Midas3TreeModelClient_H
#define __Midas3TreeModelClient_H

#include "midasStandardIncludes.h"
#include "Midas3TreeModel.h"
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <string>
#include <vector>

class Midas3FolderTreeItem;
class Midas3ItemTreeItem;

class Midas3TreeModelClient : public Midas3TreeModel
{
  Q_OBJECT
public:

  Midas3TreeModelClient(QObject *parent = 0);
  ~Midas3TreeModelClient();

  void Populate();

  void AddResource(mdo::Object *);

  void UpdateResource(mdo::Object *);

  void DeleteResource(mdo::Object *);

  void fetchMore(const QModelIndex& parent);

  void FetchFolder(Midas3FolderTreeItem* parent);

  void FetchItem(Midas3ItemTreeItem* parent);

};

#endif // __Midas3TreeModelClient_H

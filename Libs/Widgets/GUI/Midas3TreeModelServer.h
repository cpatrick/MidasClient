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
#ifndef __Midas3TreeModelServer_H
#define __Midas3TreeModelServer_H

#include "Midas3TreeModel.h"
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <string>
#include <vector>

namespace mws
{
class WebAPI;
}

namespace m3ws
{
class Folder;
}

namespace m3do
{
class Folder;
}

class Midas3TreeItem;
class Midas3FolderTreeItem;
class Midas3ItemTreeItem;
class midasSynchronizer;

class Midas3TreeModelServer : public Midas3TreeModel
{
  Q_OBJECT
public:

  Midas3TreeModelServer(QObject* parent = 0);
  ~Midas3TreeModelServer();

  void Populate();

  void fetchMore(const QModelIndex& parent);

  void fetchFolder(Midas3FolderTreeItem* folder);

  void fetchItem(Midas3ItemTreeItem* parent);

signals:
  void serverPolled();

  void fetchedMore();

public slots:

  void itemExpanded(const QModelIndex& index);

  void emitLayoutChanged()
  {
    emit layoutChanged();
  }
};

#endif // __MidasTreeModelServer_H

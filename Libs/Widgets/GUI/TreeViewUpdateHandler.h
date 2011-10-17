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
#ifndef __TreeViewUpdateHandler_H
#define __TreeViewUpdateHandler_H

#include "mdsResourceUpdateHandler.h"
#include <QObject>
#include <QTreeView>

/**
 * Passes update events to the tree view
 */
class TreeViewUpdateHandler : public QObject, public mds::ResourceUpdateHandler
{
  Q_OBJECT
public:
  TreeViewUpdateHandler(QTreeView* view);
  ~TreeViewUpdateHandler();

  void AddedResource(mdo::Object* resource);

  void DeletedResource(mdo::Object* resource);

  void UpdatedResource(mdo::Object* resource);

signals:
  void ResourceAdded(mdo::Object* resource);

  void ResourceDeleted(mdo::Object* resource);

  void ResourceUpdated(mdo::Object* resource);

protected:
  QTreeView* m_View;
};

#endif

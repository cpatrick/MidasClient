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
#ifndef __MidasTreeViewServer_H
#define __MidasTreeViewServer_H

#include "MidasTreeView.h"
#include <QMutex>

class MidasTreeModelServer;
class ExpandTreeThread;

namespace mws
{
class WebAPI;
}

class MidasTreeViewServer : public MidasTreeView
{
  Q_OBJECT
public:

  /** Constructor */
  MidasTreeViewServer(QWidget * parent = 0);
  ~MidasTreeViewServer();
public slots:
  void selectByUuid(std::string uuid, bool select = false);

  void alertFetchedMore();

  void expansionDone();

  void selectByIndex(const QModelIndex& index);

signals:
  void fetchedMore();

  void enableActions(bool);

  void startedExpandingTree();

  void finishedExpandingTree();

protected:
  void fetchItemData(MidasTreeItem* item);

  void dragMoveEvent(QDragMoveEvent* event);

  void dropEvent(QDropEvent* event);

  ExpandTreeThread* m_ExpandTreeThread;
  QMutex            m_Mutex;
};

#endif // __MidasTreeViewServer_H

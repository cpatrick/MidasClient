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
#ifndef __MidasTreeViewClient_H
#define __MidasTreeViewClient_H

#include "MidasTreeView.h"

class MidasTreeModelClient;

class MidasTreeViewClient : public MidasTreeView
{
  Q_OBJECT
public:

  MidasTreeViewClient(QWidget* parent = 0);
  ~MidasTreeViewClient();
public slots:
  void expandAll();

  void collapseAll();

  void AddResource(mdo::Object *);

  void UpdateResource(mdo::Object *);

  void DeleteResource(mdo::Object *);

signals:
  void BitstreamsDropped(const MidasItemTreeItem* parentItem, const QStringList& files);

  void BitstreamOpenRequest();

protected:
  void FetchItemData(MidasTreeItem* item);

  void dragMoveEvent(QDragMoveEvent* event);

  void dropEvent(QDropEvent* event);

  void mouseDoubleClickEvent(QMouseEvent* event);

};

#endif // __MidasTreeViewClient_H

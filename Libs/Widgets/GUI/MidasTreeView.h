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
#ifndef __MidasTreeView_H
#define __MidasTreeView_H

#include "MidasTreeViewBase.h"

#include <QItemSelection>
#include <QContextMenuEvent>
#include <QPoint>

class MidasTreeItem;
class MidasCommunityTreeItem;
class MidasCollectionTreeItem;
class MidasItemTreeItem;
class MidasBitstreamTreeItem;
class MidasTreeModel;

class midasSynchronizer;

namespace mdo
{
class Object;
}

class MidasTreeView : public MidasTreeViewBase
{
  Q_OBJECT
public:
  MidasTreeView(QWidget* parent);
  ~MidasTreeView();

  void SetSynchronizer(midasSynchronizer* synch);

  virtual void Clear();

  virtual void Initialize();

  bool IsModelIndexSelected() const;

  const QModelIndex GetSelectedModelIndex() const;

  const MidasTreeItem* GetSelectedMidasTreeItem() const;

signals:
  void MidasTreeItemSelected(const MidasTreeItem* item);

  void MidasCommunityTreeItemSelected(const MidasCommunityTreeItem* item);

  void MidasCollectionTreeItemSelected(const MidasCollectionTreeItem* item);

  void MidasItemTreeItemSelected(const MidasItemTreeItem* item);

  void MidasBitstreamTreeItemSelected(const MidasBitstreamTreeItem* item);

  void MidasNoTreeItemSelected();

  void MidasTreeViewContextMenu(QContextMenuEvent* e);

  void ResourceDropped(int type, int id);

  void FetchedSelectedResource();

public slots:
  virtual void Update();

  virtual void UpdateSelection(const QItemSelection& selected, const QItemSelection& deselected);

  virtual void DecorateByUuid(std::string uuid);

  virtual void AddResource(mdo::Object *);

  virtual void UpdateResource(mdo::Object *);

  virtual void DeleteResource(mdo::Object *);

protected:
  virtual void FetchItemData(MidasTreeItem* item) = 0;

  virtual void contextMenuEvent(QContextMenuEvent* event);

  virtual void dragEnterEvent(QDragEnterEvent* event);

  virtual void dragLeaveEvent(QDragLeaveEvent* event);

  virtual void dragMoveEvent(QDragMoveEvent* event);

  virtual void dropEvent(QDropEvent* event);

  virtual void mouseDoubleClickEvent(QMouseEvent* event);

  virtual void mousePressEvent(QMouseEvent* event);

  virtual void mouseMoveEvent(QMouseEvent* event);

  MidasTreeModel*    m_Model;
  midasSynchronizer* m_Synch;
  QPoint             m_DragStart;
  QString            m_MimeType;
  QString            m_AcceptMimeType;
};

#endif

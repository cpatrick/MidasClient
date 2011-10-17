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
#ifndef __Midas3TreeView_H
#define __Midas3TreeView_H

#include "MidasTreeViewBase.h"

#include <QItemSelection>
#include <QContextMenuEvent>
#include <QPoint>

class Midas3TreeItem;
class Midas3FolderTreeItem;
class Midas3ItemTreeItem;
class Midas3BitstreamTreeItem;
class Midas3TreeModel;

class midasSynchronizer;

namespace mdo
{
class Object;
}

class Midas3TreeView : public MidasTreeViewBase
{
  Q_OBJECT
public:
  Midas3TreeView(QWidget* parent);
  ~Midas3TreeView();

  void SetSynchronizer(midasSynchronizer* synch);

  virtual void Clear();

  virtual void Initialize();

  bool isModelIndexSelected() const;

  const QModelIndex getSelectedModelIndex() const;

  const Midas3TreeItem * getSelectedMidasTreeItem() const;

signals:
  void midasTreeItemSelected(const Midas3TreeItem* item);

  void midas3FolderTreeItemSelected(const Midas3FolderTreeItem* item);

  void midas3ItemTreeItemSelected(const Midas3ItemTreeItem* item);

  void midas3BitstreamTreeItemSelected(const Midas3BitstreamTreeItem* item);

  void midasNoTreeItemSelected();

  void midasTreeViewContextMenu(QContextMenuEvent* e);

  void resourceDropped(int type, int id);

  void fetchedSelectedResource();

public slots:
  virtual void Update();

  virtual void updateSelection(const QItemSelection& selected, const QItemSelection& deselected);

  virtual void decorateByUuid(std::string uuid);

  virtual void addResource(mdo::Object *);

  virtual void updateResource(mdo::Object *);

  virtual void deleteResource(mdo::Object *);

protected:
  virtual void fetchItemData(Midas3TreeItem* item) = 0;

  virtual void contextMenuEvent(QContextMenuEvent* event);

  virtual void dragEnterEvent(QDragEnterEvent* event);

  virtual void dragLeaveEvent(QDragLeaveEvent* event);

  virtual void dragMoveEvent(QDragMoveEvent* event);

  virtual void dropEvent(QDropEvent* event);

  virtual void mouseDoubleClickEvent(QMouseEvent* event);

  virtual void mousePressEvent(QMouseEvent* event);

  virtual void mouseMoveEvent(QMouseEvent* event);

  Midas3TreeModel*   m_Model;
  midasSynchronizer* m_Synch;
  QPoint             m_DragStart;
  QString            m_MimeType;
  QString            m_AcceptMimeType;
};

#endif

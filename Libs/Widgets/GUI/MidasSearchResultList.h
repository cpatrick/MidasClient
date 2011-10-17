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
#ifndef __MidasSearchResultList_H
#define __MidasSearchResultList_H

#include <QListWidget>

namespace mdo
{
class Object;
}

// ------------- QListWidgetItemMidasItem -------------
class QListWidgetItemMidasItem : public QListWidgetItem
{
public:

  QListWidgetItemMidasItem(QListWidget * parent, mdo::Object* data);
  ~QListWidgetItemMidasItem()
  {
  }

  mdo::Object * getObject()
  {
    return m_Object;
  }
private:
  mdo::Object* m_Object;
};
// ------------- QListWidgetItemMidasItem -------------

// ------------- MidasSearchResultList -------------
class MidasSearchResultList : public QListWidget
{
  Q_OBJECT
public:
  MidasSearchResultList(QWidget * parent);
  ~MidasSearchResultList()
  {
  }
signals:
  void midasListWidgetContextMenu( QContextMenuEvent * e );

  void midasListWidgetItemClicked( QListWidgetItemMidasItem * item );

protected:
  void contextMenuEvent( QContextMenuEvent * e );

public slots:
  void listItemClicked( QListWidgetItem * item );

  void clear();

};
// ------------- MidasSearchResultList -------------

#endif // __MidasSearchResultList_H

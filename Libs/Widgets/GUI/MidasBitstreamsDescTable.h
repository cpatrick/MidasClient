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
#ifndef __MidasBitstreamsDescTable_H
#define __MidasBitstreamsDescTable_H

#include <QTableWidget>
#include <QFlags>

#include "MidasResourceDescTable.h"

class QContextMenuEvent;
class STRUCT_ITEM;
class STRUCT_BITSTREAM;

class QTableWidgetBitstreamItem : public QTableWidgetDescriptionItem
{
public:
  QTableWidgetBitstreamItem(STRUCT_BITSTREAM * modeldata, STRUCT_ITEM * itemModelData, const char* text,
                            QTableWidgetBitstreamItem::Options options = Tooltip);
  STRUCT_ITEM * getItemModelData()
  {
    return this->itemModelData;
  }

  STRUCT_BITSTREAM * getModelData()
  {
    return this->modeldata;
  }

private:
  STRUCT_ITEM *      itemModelData;
  STRUCT_BITSTREAM * modeldata;
};

class MidasBitstreamsDescTable : public QTableWidget
{
  Q_OBJECT
public:
  MidasBitstreamsDescTable(QWidget * parent) : QTableWidget( parent )
  {
  }
  ~MidasBitstreamsDescTable()
  {
  }
signals:
  void midasTableWidgetContextMenu( QContextMenuEvent * e );

  void localFilesDropped( const QStringList & file );

protected:
  virtual void contextMenuEvent( QContextMenuEvent * e );

  virtual void dragMoveEvent( QDragMoveEvent * event );

  virtual void dragEnterEvent( QDragEnterEvent * event );

  virtual void dropEvent( QDropEvent * event );

  // virtual void dragLeaveEvent ( QDragLeaveEvent * event );
  // virtual void mousePressEvent ( QMouseEvent * event );
};

#endif // __MidasBitstreamsDescTable_H

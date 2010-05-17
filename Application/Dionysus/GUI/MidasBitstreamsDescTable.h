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
  QTableWidgetBitstreamItem(STRUCT_BITSTREAM * modeldata, 
    STRUCT_ITEM * itemModelData, 
    const char* text, 
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
  STRUCT_ITEM * itemModelData; 
  STRUCT_BITSTREAM * modeldata; 
}; 

class MidasBitstreamsDescTable : public QTableWidget
{
  Q_OBJECT

public:
  MidasBitstreamsDescTable(QWidget * parent):QTableWidget( parent ){}
  ~MidasBitstreamsDescTable(){}

signals:
  void midasTableWidgetContextMenu( QContextMenuEvent * e ); 
  void localFilesDropped( const QStringList & file );

protected:
  virtual void contextMenuEvent ( QContextMenuEvent * e );
  virtual void dragMoveEvent ( QDragMoveEvent * event );
  virtual void dragEnterEvent ( QDragEnterEvent * event );
  virtual void dropEvent( QDropEvent * event );
  //virtual void dragLeaveEvent ( QDragLeaveEvent * event ); 
  //virtual void mousePressEvent ( QMouseEvent * event ); 
}; 

#endif //__MidasBitstreamsDescTable_H
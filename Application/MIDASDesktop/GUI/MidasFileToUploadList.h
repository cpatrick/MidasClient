#ifndef __MidasFileToUploadList_H
#define __MidasFileToUploadList_H

#include <QListWidget>

class STRUCT_ITEM; 
//struct STRUCT_ITEM_INFO; 

// ------------- MidasFileToUploadList -------------
class MidasFileToUploadList : public QListWidget
{
  Q_OBJECT

public:
  MidasFileToUploadList(QWidget * parent):QListWidget(parent){}
  ~MidasFileToUploadList(){}

signals: 
  void localFilesDropped( const QStringList & files );

protected:
  virtual void dragEnterEvent ( QDragEnterEvent * event );
  virtual void dragLeaveEvent ( QDragLeaveEvent * event ); 
  virtual void dragMoveEvent ( QDragMoveEvent * event ); 
  virtual void dropEvent( QDropEvent * event ); 
  //virtual bool dropMimeData ( int index, const QMimeData * data, Qt::DropAction action ); 

private:
}; 
// ------------- MidasFileToUploadList -------------

#endif //__MidasFileToUploadList_H
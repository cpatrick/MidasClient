#include "MidasFileToUploadList.h"

#include <QDragEnterEvent>
#include <QUrl>
#include <QFileInfo>

#include "MidasClientGlobal.h"
#include "Logger.h"

void MidasFileToUploadList::dragEnterEvent ( QDragEnterEvent * event )
  {
  LOGGER_FUNC_START();
  if (event && event->mimeData())
    {
    const QMimeData * md = event->mimeData();
    QStringList mimetypes = md->formats(); 
    foreach(QString mimetype, mimetypes)
      {
      Logger::debug(STR("\t") + QSTR2STR(mimetype)); 
      }
    if ( md->hasUrls() )
      {
      Logger::debug("acceptProposedAction");
      event->acceptProposedAction();
      }
    }
  LOGGER_FUNC_END();
  }

void MidasFileToUploadList::dragLeaveEvent ( QDragLeaveEvent * event )
  {
  LOGGER_FUNC();
  }

void MidasFileToUploadList::dragMoveEvent ( QDragMoveEvent * event )
  {
  //LOGGER_FUNC();
  }

void MidasFileToUploadList::dropEvent( QDropEvent * event )
  {
  LOGGER_FUNC_START();
  if (event && event->mimeData())
    {
    const QMimeData * md = event->mimeData();
    if ( md->hasUrls() )
      {
      QStringList files; 
      foreach (QUrl url, md->urls())
        {
        QFileInfo info(url.toLocalFile()); 
        if (info.exists() && info.isFile())
          {
          Logger::debug("name:" + QSTR2STR(url.toLocalFile()));
          files <<  url.toLocalFile(); 
          //emit localFileDropped( url.toLocalFile() ); 
          }
        }
      emit localFilesDropped( files ); 
      }
    }
  LOGGER_FUNC_END();
  }

//bool MidasFileToUploadList::dropMimeData ( int index, const QMimeData * data, Qt::DropAction action )
//  {
//  LOGGER_FUNC();
//  return false; 
//  }

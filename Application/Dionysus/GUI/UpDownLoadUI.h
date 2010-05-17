#ifndef UpDownLoadUI_h
#define UpDownLoadUI_h

#include <DSpaceExporter.h> // This line has to be first!
#include <DSpaceImporter.h>

#include "ui_UpDownLoadUI.h"

class QTreeWidget; 

class DYSInit; 
class QtDionysusCommand; 
class QTableWidget; 
 
class UpDownLoadUI :  public QDialog, private Ui::UpDownLoadDialog
{
  Q_OBJECT
 
public:

  UpDownLoadUI(QWidget *parent = 0);
  ~UpDownLoadUI(); 

  void Init(std::string email, std::string password, int updown);//updown = 1 if upload
  void UpLoad(QStringList filelist, QString targetitem, QTableWidget *bitstreamsTable);
  void DownLoad(QStringList filelist, QStringList checksumlist,
                QString targetitem, QString directory);
                  
   
public slots:

  void CancelUpload();


protected:

  std::string         m_Email;
  std::string         m_Password;
  DYSInit *           m_InitFile;
  DSpaceImporter      m_Importer;
  DSpaceExporter      m_DSpaceExporter;
  QtDionysusCommand * m_Command;
  bool                m_Cancel;

};

#endif




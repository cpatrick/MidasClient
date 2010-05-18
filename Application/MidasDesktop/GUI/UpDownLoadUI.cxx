#include <iostream>
#include <fstream>
#include <stdio.h>

#include <QtGui> 
#include <Qt>

#include <kwsys/SystemTools.hxx>

#include "DYSInit.h"
#include "Logger.h"
#include "DetailedException.h"
#include "kwCrypt.h"

#include "QtDionysusCommand.h"
#include "UpDownLoadUI.h"

UpDownLoadUI::UpDownLoadUI(QWidget *parent):
  QDialog(parent)
  {
  setupUi(this); // this sets up GUI

  m_Command = new QtDionysusCommand; 

  this->m_InitFile = DYSInit::instance(); 

  // signals/slots mechanism in action
  //connect(cancelButton, SIGNAL( clicked() ), this, SLOT( CancelUpload() ) );
  }

void UpDownLoadUI::Init(std::string email, std::string password, int updown)
  {
  Logger::debug(__func__);
  m_Cancel = false;
  m_Email = email;
  m_Password = password;
  m_Command->SetQLabel(updateLabel);

  /*m_Importer.SetDatabaseName(m_InitFile->GetDatabaseName().c_str());
  m_Importer.SeUserName(m_InitFile->GetDatabaseUser().c_str());

  m_Importer.SetPassword(m_InitFile->GetDatabasePassword().c_str());
  m_Importer.SetPort(m_InitFile->GetDatabasePort().c_str());
  m_Importer.SetHostname(m_InitFile->GetDatabaseHost().c_str());
  m_Importer.Connect(m_Email.c_str(),m_Password.c_str());*/
  m_DSpaceExporter.SetCommand(m_Command);

  if(updown)
    {
    this->setWindowTitle("Uploading");
    updateLabel->setText("");
    globalStateLabel->setText("Global progression...");
    progressBar->setValue(0);
    globalBar->setValue(0);
    this->update();
    this->repaint();
    qApp->processEvents();
    }
  else
    {
    this->setWindowTitle("Downloading");
    updateLabel->setText("");
    globalStateLabel->setText("Global progression...");
    progressBar->setValue(0);
    globalBar->setValue(0);
    this->update();
    this->repaint();
    qApp->processEvents();
    }
  }

    
void UpDownLoadUI::UpLoad(QStringList filelist, QString targetitem, QTableWidget *bitstreamsTable)
  {
  Logger::debug(__func__);
  int nbrfiles = filelist.size();
  std::string fileTOdelete = "";
  
  m_DSpaceExporter.SetEmail(m_Email.c_str());
  m_DSpaceExporter.SetPassword(m_Password.c_str());
  
  for(int i = nbrfiles-1; i >= 0; i--)
    { 
    this->setWindowTitle("Uploading");
    qApp->processEvents();//origin

    if(m_Cancel)
      {
      break;
      }

    m_Command->SetQProgressBar(progressBar,
                              globalBar,nbrfiles,nbrfiles-i-1);
    uploadNameLabel->setText(filelist[i]);
    this->update();
    this->repaint();
    qApp->processEvents();

    m_DSpaceExporter.SetFilename(filelist[i].toStdString().c_str());
    m_DSpaceExporter.SetImporter(&m_Importer); // to check the file validity

    //#ifdef USE_PublicationDB
    //m_DSpaceExporter.SetUploadUrl(m_InitFile->GetMidasUrl().c_str());
    //m_DSpaceExporter.UploadPBBitstream(m_Targetitem);
    //#else
    m_DSpaceExporter.SetUploadUrl(m_InitFile->GetMidasURL().c_str());
    //m_DSpaceExporter.UploadBitstream(targetitem.toStdString().c_str());
    m_DSpaceExporter.UploadBitstream2(targetitem.toStdString().c_str());
    //#endif
    Logger::debug("Upload done"); 
    //update the revision
    qApp->processEvents();
    this->update();
    this->repaint();
    qApp->processEvents();
    /*if(bitTreeWidget)
      {
      for(int treeitem = 0; treeitem < bitTreeWidget->topLevelItemCount(); treeitem++)
        {
        std::string filename = filelist[i].toStdString();
        fileTOdelete = filename;
        while(filename.find("/") !=-1)
          {
          int place = filename.find("/");
          filename.erase(0, place+1);
          }
        if(bitTreeWidget->topLevelItem(treeitem)->
          data(0,0).toString().toStdString() == filename)
          {
          std::list<std::string> bitstreamsToUpdate;
          bitstreamsToUpdate.push_front(bitTreeWidget->topLevelItem(treeitem)->
          data(5,0).toString().toStdString());

          for( int j = 0; j < bitTreeWidget->topLevelItem(treeitem)->childCount(); j++)
            {
            bitstreamsToUpdate.push_front(bitTreeWidget->topLevelItem(treeitem)->child(j)->
           data(5,0).toString().toStdString());
            }
          m_Importer.UpdateRevision(bitstreamsToUpdate);
          
          break;
          }
        }
      }*/
    }
    Logger::debug("Finishing..."); 

  if(!m_Cancel)
    {
    this->hide();
    QMessageBox::information(this, tr("Upload"),
                            tr("Bitstreams uploaded successfully!"));
    }
  else
    {
    kwsys::SystemTools::RemoveFile(fileTOdelete.c_str());
    }

  this->close();
  }

void UpDownLoadUI::DownLoad(QStringList filelist, QStringList sequenceidlist,
                            QString targetitem, QString directory)
  {
  int nbrfiles = filelist.size();
  std::string fileTOdelete = "";

  for(int i=0;i < nbrfiles;i++)
    {
    this->setWindowTitle("Downloading");
    qApp->processEvents();
    if(m_Cancel)
      {
      m_Cancel = true;
      break;
      }

    m_Command->SetQProgressBar(progressBar,
                              globalBar,nbrfiles,i); 
    uploadNameLabel->setText(filelist[i]);
    this->progressBar->repaint();
    this->globalBar->repaint();
    this->show();
    this->update();
    this->repaint();

    m_DSpaceExporter.SetDownloadUrl(m_InitFile->GetMidasURL().c_str());
    m_DSpaceExporter.SetEmail(m_Email.c_str());
    m_DSpaceExporter.SetPassword(m_Password.c_str());
    std::string URL = "";
    for(unsigned int j=21;j<m_Importer.GetHandle(targetitem.toStdString()).size();j++)
      {
      URL += m_Importer.GetHandle(targetitem.toStdString()).c_str()[j];
      }
    URL += "/";
    URL += sequenceidlist[i].toStdString();

    if(!directory.isEmpty())
      {
      std::string filename = directory.toStdString();
      filename += "/";
      filename += filelist[i].toStdString();
      //m_DSpaceExporter.SetCancel(m_Cancel);
      fileTOdelete = filename;
      m_DSpaceExporter.DownloadBitstream(URL,filename);
      }
    
    }

  this->hide();
  this->update();
  if(!m_Cancel)
    {
    QMessageBox::information(this, tr("Download"),
                                   tr("Download Successful!"));
    }
  else
    {
    kwsys::SystemTools::RemoveFile(fileTOdelete.c_str());
    }
  
  this->close();
  }

void UpDownLoadUI::CancelUpload()
  {
  int Question=QMessageBox::question(this, tr("Cancel"),
                                tr("Are you sure to cancel this download ?"),
                                "ok","Cancel",0,1); 
                                 
           
            
  if(Question==0)
  {
    m_Cancel = true;
    //m_DSpaceExporter.SetCancel(m_Cancel);
  }
                        
  }

UpDownLoadUI::~UpDownLoadUI()
  {
  delete m_Command; 
  }

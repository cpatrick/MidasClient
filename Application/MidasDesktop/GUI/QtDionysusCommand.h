/*=========================================================================

  Program:   Dionysus
  Module:    QtDionysusCommand.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __QtDionysusCommand_h
#define __QtDionysusCommand_h

#include <DSpaceExporter.h>
#include <kwsys/SystemTools.hxx>
#include <iostream>
#include <stdio.h>
#include <Qt>
#include <QtGui> 

class QtDionysusCommand : public DSpaceExporterCommand
{ 
public:
  QtDionysusCommand():DSpaceExporterCommand()
    {
    m_Progress = NULL;
    m_Global = NULL;
    m_Update = NULL;
    m_Nbrfiles = 0;
    m_Iteractor = 0;
    m_Time = 0;
    };
  ~QtDionysusCommand(){};

  void SetQProgressBar(QProgressBar* progress,
    QProgressBar* global, int nbrfiles,int i)
    {
    m_Progress = progress;
    m_Global = global;
    m_Nbrfiles = nbrfiles;
    m_Iteractor = i;
    }
    
  void SetQLabel(QLabel* update)
    {
    m_Update = update;
    }
    
  virtual void UpdateDownloadSpeed(double val)
    {
    double currentTime = kwsys::SystemTools::GetTime();
    m_Update->setText("");
    if(m_Time>0)
      {
      double elapsed = currentTime-m_Time;
      
      std::string speed = "Downloading at ";
      
      if(elapsed > 0)
        {
        double rateInByte = (val-m_PreviousDataValue)/elapsed;
        double rateInKiloByte = rateInByte/1024; 
        double rateInMegaByte = rateInKiloByte/1024; 
        double rateInGigaByte = rateInMegaByte/1024; 
        if(rateInByte>0)
          {
          char* num = new char[10];
          if(rateInByte<1024)
            {
            sprintf(num,"%.3f",rateInByte);
            speed += num;
            speed += " b/s";
            m_Update->setText(speed.c_str());
            }
          if(rateInByte>=1024 && rateInKiloByte<1024)
            {
            sprintf(num,"%.3f",rateInKiloByte);
            speed += num;
            speed += " kb/s";
            m_Update->setText(speed.c_str());
            }
          if(rateInKiloByte>=1024 && rateInMegaByte<1024)
            {
            sprintf(num,"%.3f",rateInMegaByte);
            speed += num;
            speed += " Mb/s";
            m_Update->setText(speed.c_str());
            }
          if(rateInMegaByte>=1024 && rateInGigaByte<1024)
            {
            sprintf(num,"%.3f",rateInGigaByte);
            speed += num;
            speed += " Mb/s";
            m_Update->setText(speed.c_str());
            }
          delete [] num;
          }
        else
          {
          m_Update->setText("");
          }
        }
      }
       
    m_Update->update();
    m_Update->repaint();
    m_PreviousDataValue = val;
    m_Time = currentTime;
    qApp->processEvents();
    }
    
  virtual void UpdateUploadSpeed(double val)
    {
    double currentTime = kwsys::SystemTools::GetTime();
    m_Update->setText("");
    if(m_Time>0)
      {
      double elapsed = currentTime-m_Time;
      
      std::string speed = "Uploading at ";
      
      if(elapsed > 0)
        {
        double rateInByte = (val-m_PreviousDataValue)/elapsed;
        double rateInKiloByte = rateInByte/1024; 
        double rateInMegaByte = rateInKiloByte/1024; 
        double rateInGigaByte = rateInMegaByte/1024; 
        if(rateInByte>0)
          {
          char* num = new char[10];
          if(rateInByte<1024)
            {
            sprintf(num,"%.3f",rateInByte);
            speed += num;
            speed += " b/s";
            m_Update->setText(speed.c_str());
            }
          if(rateInByte>=1024 && rateInKiloByte<1024)
            {
            sprintf(num,"%.3f",rateInKiloByte);
            speed += num;
            speed += " kb/s";
            m_Update->setText(speed.c_str());
            }
          if(rateInKiloByte>=1024 && rateInMegaByte<1024)
            {
            sprintf(num,"%.3f",rateInMegaByte);
            speed += num;
            speed += " Mb/s";
            m_Update->setText(speed.c_str());
            }
          if(rateInMegaByte>=1024 && rateInGigaByte<1024)
            {
            sprintf(num,"%.3f",rateInGigaByte);
            speed += num;
            speed += " Mb/s";
            m_Update->setText(speed.c_str());
            }
          delete [] num;
          }
        else
          {
          m_Update->setText("");
          }
        }
      }
    m_Update->update();
    m_Update->repaint();
    m_PreviousDataValue = val;
    m_Time = currentTime;
    qApp->processEvents();
    }

  virtual void Execute(double val)
    {
    if(m_Progress)
      {
      m_Progress->setValue((int)val);
      m_Progress->update();
      m_Progress->repaint();
      m_Global->setValue((int)(val/m_Nbrfiles + 100*m_Iteractor/m_Nbrfiles));
      m_Global->update();
      m_Global->repaint();
      qApp->processEvents();
      }
    }

private:
  QProgressBar* m_Progress;
  QProgressBar* m_Global;
  QLabel* m_Update;
  double m_Time;
  int m_Nbrfiles;
  int m_Iteractor;
  double m_PreviousDataValue;
};
#endif

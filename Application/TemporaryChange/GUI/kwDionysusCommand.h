/*=========================================================================

  Program:   Dionysus
  Module:    kwDionysusCommand.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __KWDionysusCommand_h
#define __KWDionysusCommand_h

#include <iostream>
#include "vtkKWProgressGauge.h"


class KWDionysusCommand : public DSpaceExporterCommand
{ 
public:
  KWDionysusCommand():DSpaceExporterCommand()
    {
    m_Progress = NULL;
    m_Global = NULL;
    m_Nbrfiles = 0;
    m_Iteractor = 0;
    };
  ~KWDionysusCommand(){};

  void SetKWProgressGauge(vtkKWProgressGauge* progress,
    vtkKWProgressGauge* global, int nbrfiles,int i)
    {
    m_Progress = progress;
    m_Global = global;
    m_Nbrfiles = nbrfiles;
    m_Iteractor = i;
    }

  virtual void Execute(double val)
    {
    if(m_Progress)
      {
      m_Progress->SetValue(val);
      //std::cout<<val<<std::endl;
      m_Global->SetValue(val/m_Nbrfiles + 100*m_Iteractor/m_Nbrfiles);
      }
    }

private:
  vtkKWProgressGauge* m_Progress;
  vtkKWProgressGauge* m_Global;
  int m_Nbrfiles;
  int m_Iteractor;
};
#endif

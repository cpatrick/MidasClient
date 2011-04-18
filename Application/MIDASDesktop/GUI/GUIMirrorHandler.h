/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __GUIMirrorHandler_H
#define __GUIMirrorHandler_H

#include "mwsMirrorHandler.h"
#include <QObject>
#include <QWidget>

class MirrorPickerUI;

class GUIMirrorHandler : public QObject, public mws::MirrorHandler
{
  Q_OBJECT
public:
  GUIMirrorHandler(MirrorPickerUI* dialog);
  ~GUIMirrorHandler();

public slots:
  void dialogAccepted();
  mdo::Assetstore* HandleMirroredBitstream(mdo::Bitstream* bitstream);

signals:
  void prompt(mdo::Bitstream* bitstream);

protected:
  MirrorPickerUI* m_MirrorDialog;
  bool            m_Done;
  std::set<int>   m_PreferredLocations;
};

#endif

/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __GUIAgreement_H
#define __GUIAgreement_H

#include "midasAgreementHandler.h"
#include <QObject>

class AgreementUI;
class MIDASDesktopUI;
class midasSynchronizer;

class GUIAgreement : public QObject, public midasAgreementHandler
{
  Q_OBJECT
public:
  GUIAgreement(MIDASDesktopUI* parent);
  ~GUIAgreement();

  bool HandleAgreement(midasSynchronizer* synch);
  std::string getUrl();

public slots:
  void finish();
  void cancel();
signals:
  void displayDialog();
  void checkingAgreement();

protected:
  bool checkUserHasAgreed(midasSynchronizer* synch);

private:
  MIDASDesktopUI* m_Parent;
  bool m_Done;
  bool m_Canceled;
  std::string m_Url;
};

#endif

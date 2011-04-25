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
class midasSynchronizer;

class GUIAgreement : public QObject, public midasAgreementHandler
{
  Q_OBJECT
public:
  GUIAgreement(AgreementUI* dialog);
  ~GUIAgreement();

  bool HandleAgreement(midasSynchronizer* synch);

signals:
  void displayDialog();
  void checkingAgreement();
  void errorMessage(const QString&);

protected:
  bool checkUserHasAgreed(midasSynchronizer* synch);

private:
  AgreementUI* m_Dialog;
  std::string  m_Url;
};

#endif

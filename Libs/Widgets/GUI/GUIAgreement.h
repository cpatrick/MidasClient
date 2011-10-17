/******************************************************************************
 * Copyright 2011 Kitware Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/


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

  void errorMessage(const QString &);

protected:
  bool checkUserHasAgreed(midasSynchronizer* synch);

private:
  AgreementUI* m_Dialog;
  std::string  m_Url;
};

#endif

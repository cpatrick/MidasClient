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
#ifndef __AgreementUI_H
#define __AgreementUI_H

#include "ui_AgreementUI.h"
#include <QDialog>
#include <QString>

class AgreementUI : public QDialog, private Ui::AgreementDialog
{
  Q_OBJECT
public:
  AgreementUI(QWidget* parent);
  ~AgreementUI();

  void SetUrl(const QString& url);

  bool WasCanceled();

signals:
  void errorMessage(const QString &);

public slots:
  void reject();

  void exec();

  void openBrowser();

private:
  QString m_Url;
  bool    m_Canceled;
};

#endif

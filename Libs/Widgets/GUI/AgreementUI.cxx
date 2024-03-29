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
#include "AgreementUI.h"
#include "GUIAgreement.h"
#include <QString>
#include <QUrl>
#include <QDesktopServices>

AgreementUI::AgreementUI(QWidget* parent)
  : QDialog(parent), m_Canceled(false)
{
  setupUi(this);
  this->setModal(true);
  connect(openBrowserButton, SIGNAL(released() ),
          this, SLOT(openBrowser() ) );

  m_Url = "";
}

AgreementUI::~AgreementUI()
{
}

void AgreementUI::SetUrl(const QString& url)
{
  m_Url = url;
}

bool AgreementUI::WasCanceled()
{
  return m_Canceled;
}

void AgreementUI::exec()
{
  m_Canceled = false;
  QString labelText = "The selected resource is under a license agreement."
    " You must log in on the server and agree to the license agreement "
    "before you are allowed to pull this resource or its children.<br><br>"
    "URL: ";
  labelText += m_Url;

  labelText += "<br><br>Click the button below to login and agree "
    "to the license, then click OK once you have agreed.";

  this->agreementLabel->setText(labelText);
  QDialog::exec();
}

void AgreementUI::openBrowser()
{
  QUrl qurl(m_Url);

  if( !QDesktopServices::openUrl(qurl) )
    {
    std::stringstream text;
    text << "Error: could not open " << m_Url.toStdString() << " in the browser.";
    emit errorMessage(text.str().c_str() );
    }
}

void AgreementUI::reject()
{
  m_Canceled = true;
  QDialog::reject();
}


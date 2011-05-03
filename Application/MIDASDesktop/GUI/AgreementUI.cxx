#include "AgreementUI.h"
#include "GUIAgreement.h"
#include <QString>
#include <QUrl>
#include <QDesktopServices>

AgreementUI::AgreementUI(QWidget* parent)
: m_Canceled(false), QDialog(parent)
{
  setupUi(this);
  this->setModal(true);
  connect(openBrowserButton, SIGNAL(released()),
          this, SLOT(openBrowser()));

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
  if(!QDesktopServices::openUrl(qurl))
    {
    std::stringstream text;
    text << "Error: could not open " << m_Url.toStdString() << " in the browser.";
    emit errorMessage(text.str().c_str());
    }
}

void AgreementUI::reject()
{
  m_Canceled = true;
  QDialog::reject();
}

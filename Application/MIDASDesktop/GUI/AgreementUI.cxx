#include "AgreementUI.h"
#include "MIDASDesktopUI.h"
#include "GUIAgreement.h"
#include <QString>
#include <QUrl>
#include <QDesktopServices>

AgreementUI::AgreementUI(MIDASDesktopUI* parent, GUIAgreement* controller)
: m_Parent(parent), m_Controller(controller), QDialog(parent)
{
  setupUi(this);
  connect(this, SIGNAL(accepted()),
          controller, SLOT(finish()));
  connect(this, SIGNAL(rejected()),
          controller, SLOT(cancel()));
  connect(openBrowserButton, SIGNAL(released()),
          this, SLOT(openBrowser()));
}

void AgreementUI::exec()
{
  QString labelText = "The selected resource is under a license agreement."
    " You must log in on the server and agree to the license agreement "
    "before you are allowed to pull this resource or its children.<br><br>"
    "URL: ";
  labelText += m_Controller->getUrl().c_str();

  labelText += "<br><br>Click the button below to login and agree "
    "to the license, then click OK once you have agreed.";

  this->agreementLabel->setText(labelText);
  QDialog::exec();
}

void AgreementUI::openBrowser()
{
  std::string url = m_Controller->getUrl().c_str();
  QUrl qurl(url.c_str());
  if(!QDesktopServices::openUrl(qurl))
    {
    std::stringstream text;
    text << "Error: could not open " << url << " in the browser.";
    m_Parent->GetLog()->Error(text.str());
    }
}

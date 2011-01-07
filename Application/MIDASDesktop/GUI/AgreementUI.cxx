#include "AgreementUI.h"
#include "MIDASDesktopUI.h"
#include "GUIAgreement.h"

AgreementUI::AgreementUI(MIDASDesktopUI* parent, GUIAgreement* controller)
: m_Parent(parent), m_Controller(controller), QDialog(parent)
{
  connect(this, SIGNAL(accepted()),
          m_Controller, SLOT(finish()));
  connect(this, SIGNAL(rejected()),
          m_Controller, SLOT(cancel()));
  setupUi(this);
}

void AgreementUI::exec()
{
  QDialog::exec();
}

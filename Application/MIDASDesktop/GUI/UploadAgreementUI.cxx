#include "UploadAgreementUI.h"

#include "MIDASDesktopUI.h"
#include "mwsSettings.h"

UploadAgreementUI::UploadAgreementUI(MIDASDesktopUI *parent):
QDialog(parent), parent(parent)
{
  setupUi(this);
}

void UploadAgreementUI::reset()
{
  QPalette plt;
  plt.setColor(QPalette::WindowText, Qt::black);
  this->agreementCheckBox->setPalette(plt);
  this->agreementCheckBox->setChecked( mws::Settings::GetInstance()->GetReadUploadAgreement() ); 
}

int UploadAgreementUI::exec()
{
  this->reset();
  return QDialog::exec(); 
}

void UploadAgreementUI::accept()
{
  if ( !this->agreementCheckBox->isChecked() )
    {
    QPalette plt;
    plt.setColor( QPalette::WindowText, Qt::red );
    this->agreementCheckBox->setPalette( plt );
    }
  else 
    {
    mws::Settings::GetInstance()->SetReadUploadAgreement( true ); 
    mws::Settings::GetInstance()->Write(); 
    QDialog::accept(); 
    }
}

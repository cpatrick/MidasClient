#include "AboutUI.h"

#include "MidasClientGlobal.h"

AboutUI::AboutUI(QWidget* parent):
QDialog(parent)
{
  setupUi(this);
}

AboutUI::~AboutUI()
{
}

void AboutUI::reset()
{
  this->releaseLabel->setText( STR2QSTR(MIDAS_CLIENT_VERSION_STR) ); 
}

int AboutUI::exec()
{
  this->reset(); 
  return QDialog::exec(); 
}

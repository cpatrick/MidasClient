#include "AboutUI.h"

#include "DionysusUI.h"
#include "MidasClientGlobal.h"

AboutUI::AboutUI(DionysusUI *parent):
  QDialog(parent)
  {
  setupUi(this);
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

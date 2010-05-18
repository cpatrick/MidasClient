#include "AboutUI.h"

#include "MIDASDesktopUI.h"
#include "MidasClientGlobal.h"

AboutUI::AboutUI(MIDASDesktopUI *parent):
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

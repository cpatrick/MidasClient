#include "AboutUI.h"

#include "MIDASConfig.h"

AboutUI::AboutUI(QWidget* parent) :
  QDialog(parent)
{
  setupUi(this);
}

AboutUI::~AboutUI()
{
}

void AboutUI::reset()
{
  this->releaseLabel->setText( QString("MIDAS Desktop ") + MIDAS_CLIENT_VERSION );
}

int AboutUI::exec()
{
  this->reset();
  return QDialog::exec();
}


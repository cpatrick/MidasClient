#include "DeleteResourceUI.h"
#include "MIDASDesktopUI.h"
#include "midasDatabaseProxy.h"
#include "midasSynchronizer.h"

/** Constructor */
DeleteResourceUI::DeleteResourceUI(MIDASDesktopUI *parent):
  QDialog(parent), m_Parent(parent)
{
  setupUi(this);
}

DeleteResourceUI::~DeleteResourceUI()
{
}

/** */
void DeleteResourceUI::init()
{
}

/** */
int DeleteResourceUI::exec()
{
  this->init();
  return QDialog::exec();
}

/** */
void DeleteResourceUI::accept()
{
  QDialog::accept();
}

#include "DeleteResourceUI.h"

/** Constructor */
DeleteResourceUI::DeleteResourceUI(QWidget* parent, bool server) :
  QDialog(parent), m_Server(server)
{
  setupUi(this);
}

DeleteResourceUI::~DeleteResourceUI()
{
}

void DeleteResourceUI::init()
{
  if( this->m_Server )
    {
    this->deleteLabel->setText("<b>Are you sure you want to delete the selected resource on the server?</b>");
    this->deleteFileCheckBox->hide();
    }
  else
    {
    this->deleteLabel->setText("<b>Are you sure you want to delete the selected resource on the client?</b>");
    this->deleteFileCheckBox->show();
    }
}

int DeleteResourceUI::exec()
{
  this->init();
  return QDialog::exec();
}

void DeleteResourceUI::accept()
{
  emit deleteResource(this->deleteFileCheckBox->isChecked() );

  QDialog::accept();
}


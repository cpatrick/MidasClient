#include "PreferencesUI.h"

#include "MIDASDesktopUI.h"
#include "MidasClientGlobal.h"
#include "midasDatabaseProxy.h"
#include <QString>
#include <QFileDialog>

PreferencesUI::PreferencesUI(MIDASDesktopUI *parent):
  QDialog(parent), m_parent(parent)
{
  setupUi(this);
  connect(settingComboBox, SIGNAL(currentIndexChanged(int)), this,
    SLOT( enableActions(int) ) );
  connect(workingDirBrowseButton, SIGNAL(released()), this,
    SLOT( selectWorkingDir() ) );
}

void PreferencesUI::reset()
{
  m_parent->getDatabaseProxy()->Open();
  std::string interval = this->m_parent->getDatabaseProxy()->GetSetting(
    midasDatabaseProxy::AUTO_REFRESH_INTERVAL);
  std::string index = this->m_parent->getDatabaseProxy()->GetSetting(
    midasDatabaseProxy::AUTO_REFRESH_SETTING);
  std::string path = m_parent->getDatabaseProxy()->GetSetting(
    midasDatabaseProxy::ROOT_DIR);
  m_parent->getDatabaseProxy()->Close();

  if(index != "")
    {
    settingComboBox->setCurrentIndex(atoi(index.c_str()));
    }
  if(interval != "")
    {
    timeSpinBox->setValue(atoi(interval.c_str()));
    }
  enableActions(settingComboBox->currentIndex());

  if(path == "")
    {
    path = kwsys::SystemTools::GetCurrentWorkingDirectory();
    }

  workingDirEdit->setText(path.c_str());
}

void PreferencesUI::enableActions(int index)
{
  bool val = index < 2;
  refreshLabel1->setEnabled( val );
  refreshLabel2->setEnabled( val );
  timeSpinBox->setEnabled( val );
}

void PreferencesUI::selectWorkingDir()
{
  m_parent->getDatabaseProxy()->Open();
  std::string path = m_parent->getDatabaseProxy()->GetSetting(midasDatabaseProxy::ROOT_DIR);
  m_parent->getDatabaseProxy()->Close();

  if(path == "")
    {
    path = kwsys::SystemTools::GetCurrentWorkingDirectory();
    }

  QString dir = QFileDialog::getExistingDirectory(
    this, tr("Choose Root Directory"),
    workingDirEdit->text(),
    QFileDialog::ShowDirsOnly);
  if(dir != "")
    {
    workingDirEdit->setText(dir);
    }
}

int PreferencesUI::exec()
{
  this->reset();
  return QDialog::exec();
}

void PreferencesUI::accept()
{
  //Save the preferences
  m_parent->getDatabaseProxy()->Open();
  m_parent->getDatabaseProxy()->SetSetting(midasDatabaseProxy::AUTO_REFRESH_INTERVAL, timeSpinBox->value());
  m_parent->getDatabaseProxy()->SetSetting(midasDatabaseProxy::AUTO_REFRESH_SETTING, settingComboBox->currentIndex());

  if(kwsys::SystemTools::FileIsDirectory(workingDirEdit->text().toAscii()))
    {
    m_parent->getDatabaseProxy()->SetSetting(midasDatabaseProxy::ROOT_DIR, workingDirEdit->text().toStdString());
    }
  m_parent->getDatabaseProxy()->Close();

  emit intervalChanged();
  emit settingChanged();

  QDialog::accept();
}

#include "AutoRefreshUI.h"

#include "MIDASDesktopUI.h"
#include "MidasClientGlobal.h"
#include "midasDatabaseProxy.h"

AutoRefreshUI::AutoRefreshUI(MIDASDesktopUI *parent):
  QDialog(parent), m_parent(parent)
{
  setupUi(this);
  connect(settingComboBox, SIGNAL(currentIndexChanged(int)), this,
    SLOT( enableActions(int) ) );
}

void AutoRefreshUI::reset()
{
  m_parent->getDatabaseProxy()->Open();
  std::string interval = this->m_parent->getDatabaseProxy()->GetSetting(
    midasDatabaseProxy::AUTO_REFRESH_INTERVAL);
  std::string index = this->m_parent->getDatabaseProxy()->GetSetting(
    midasDatabaseProxy::AUTO_REFRESH_SETTING);
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
}

void AutoRefreshUI::enableActions(int index)
{
  bool val = index < 2;
  refreshLabel1->setEnabled( val );
  refreshLabel2->setEnabled( val );
  timeSpinBox->setEnabled( val );
}

int AutoRefreshUI::exec()
{
  this->reset();
  return QDialog::exec();
}

void AutoRefreshUI::accept()
{
  m_parent->getDatabaseProxy()->Open();
  m_parent->getDatabaseProxy()->SetSetting(midasDatabaseProxy::AUTO_REFRESH_INTERVAL, timeSpinBox->value());
  m_parent->getDatabaseProxy()->SetSetting(midasDatabaseProxy::AUTO_REFRESH_SETTING, settingComboBox->currentIndex());
  m_parent->getDatabaseProxy()->Close();

  emit intervalChanged();
  emit settingChanged();

  QDialog::accept();
}

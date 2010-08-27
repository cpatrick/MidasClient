#include "PreferencesUI.h"

#include "MIDASDesktopUI.h"
#include "MidasClientGlobal.h"
#include "midasDatabaseProxy.h"
#include "UnifyTreeThread.h"
#include <QString>
#include <QFileDialog>

PreferencesUI::PreferencesUI(MIDASDesktopUI *parent):
  QDialog(parent), m_parent(parent)
{
  setupUi(this);

  m_UnifyTreeThread = NULL;

  connect(settingComboBox, SIGNAL(currentIndexChanged(int)), this,
    SLOT( enableActions(int) ) );
  connect(workingDirBrowseButton, SIGNAL(released()), this,
    SLOT( selectWorkingDir() ) );
}

PreferencesUI::~PreferencesUI()
{
  delete m_UnifyTreeThread;
}

void PreferencesUI::reset()
{
  std::string interval = this->m_parent->getDatabaseProxy()->GetSetting(
    midasDatabaseProxy::AUTO_REFRESH_INTERVAL);
  std::string index = this->m_parent->getDatabaseProxy()->GetSetting(
    midasDatabaseProxy::AUTO_REFRESH_SETTING);
  std::string path = m_parent->getDatabaseProxy()->GetSetting(
    midasDatabaseProxy::ROOT_DIR);
  m_UnifiedTree = m_parent->getDatabaseProxy()->GetSettingBool(
    midasDatabaseProxy::UNIFIED_TREE);

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

  copyResourcesCheckBox->setChecked(m_UnifiedTree);
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
  std::string path = m_parent->getDatabaseProxy()->GetSetting(midasDatabaseProxy::ROOT_DIR);

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
  m_parent->getDatabaseProxy()->SetSetting(midasDatabaseProxy::AUTO_REFRESH_INTERVAL, timeSpinBox->value());
  m_parent->getDatabaseProxy()->SetSetting(midasDatabaseProxy::AUTO_REFRESH_SETTING, settingComboBox->currentIndex());
  m_parent->getDatabaseProxy()->SetSetting(midasDatabaseProxy::UNIFIED_TREE, copyResourcesCheckBox->isChecked());

  if(kwsys::SystemTools::FileIsDirectory(workingDirEdit->text().toAscii()))
    {
    m_parent->getDatabaseProxy()->SetSetting(midasDatabaseProxy::ROOT_DIR, workingDirEdit->text().toStdString());
    }

  emit intervalChanged();
  emit settingChanged();

  if(copyResourcesCheckBox->isChecked() && !m_UnifiedTree)
    {
    if(m_UnifyTreeThread)
      {
      disconnect(m_UnifyTreeThread);
      }
    delete m_UnifyTreeThread;

    m_UnifyTreeThread = new UnifyTreeThread(m_parent);
    
    connect(m_UnifyTreeThread, SIGNAL(threadComplete()), this, SLOT(unifyTreeDone()));

    m_parent->displayStatus("Copying resources into a single tree...");
    m_parent->setProgressIndeterminate();

    m_UnifyTreeThread->start();
    }

  QDialog::accept();
}

void PreferencesUI::unifyTreeDone()
{
  m_parent->displayStatus("Finished unifying resources on disk.");
  m_parent->GetLog()->Message("Finished unifying resources on disk.");
  m_parent->setProgressEmpty();
}

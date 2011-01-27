#include "PreferencesUI.h"

#include "MIDASDesktopUI.h"
#include "MidasClientGlobal.h"
#include "midasDatabaseProxy.h"
#include "UnifyTreeThread.h"
#include <QString>
#include <QFileDialog>
#include <QMessageBox>

PreferencesUI::PreferencesUI(MIDASDesktopUI *parent):
  QDialog(parent), m_parent(parent)
{
  setupUi(this);

  m_UnifyTreeThread = NULL;

  connect(settingComboBox, SIGNAL(currentIndexChanged(int)), this,
    SLOT( enableActions(int) ) );
  connect(workingDirBrowseButton, SIGNAL(released()), this,
    SLOT( selectWorkingDir() ) );
  connect(copyNowButton, SIGNAL(released()), this,
    SLOT( unifyTree() ) );
}

PreferencesUI::~PreferencesUI()
{
  delete m_UnifyTreeThread;
}

void PreferencesUI::unifyTree()
{
  bool copy = false;
  switch(QMessageBox::question(this, "Relocate Files",
    "Do you want to <b>move</b> the files, or <b>copy</b> them?",
    "Move", "Copy", "Cancel"))
    {
    case 0:
      copy = false;
      break;
    case 1:
      copy = true;
      break;
    default:
      return;
    }

  this->accept();

  if(m_UnifyTreeThread)
    {
    if(m_UnifyTreeThread->isRunning())
      {
      m_parent->GetLog()->Error("Tree copy thread is already running!");
      return;
      }
    disconnect(m_UnifyTreeThread);
    }
  delete m_UnifyTreeThread;

  m_UnifyTreeThread = new UnifyTreeThread(m_parent);
  m_UnifyTreeThread->setCopy(copy);
    
  connect(m_UnifyTreeThread, SIGNAL(threadComplete()), this, SLOT(unifyTreeDone()));

  m_parent->displayStatus("Copying resources into a single tree...");
  m_parent->setProgressIndeterminate();

  m_UnifyTreeThread->start();
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

  std::string dir = workingDirEdit->text().toStdString();
  kwsys::SystemTools::ConvertToUnixSlashes(dir);
  if(kwsys::SystemTools::FileIsDirectory(dir.c_str()))
    {
    m_parent->getDatabaseProxy()->SetSetting(midasDatabaseProxy::ROOT_DIR, dir);
    }

  emit intervalChanged();
  emit settingChanged();

  QDialog::accept();
}

void PreferencesUI::unifyTreeDone()
{
  m_parent->displayStatus("Finished unifying resources on disk.");
  m_parent->GetLog()->Message("Finished unifying resources on disk.");
  m_parent->setProgressEmpty();
  m_parent->updateClientTreeView();
}

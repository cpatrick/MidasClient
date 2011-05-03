#include "PreferencesUI.h"

#include "MidasClientGlobal.h"
#include "mdsDatabaseAPI.h"
#include "UnifyTreeThread.h"
#include "PollFilesystemThread.h"
#include <QString>
#include <QFileDialog>
#include <QMessageBox>

PreferencesUI::PreferencesUI(QWidget* parent)
: QDialog(parent)
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
      return;
      }
    disconnect(m_UnifyTreeThread);
    }
  delete m_UnifyTreeThread;

  m_UnifyTreeThread = new UnifyTreeThread;
  m_UnifyTreeThread->setCopy(copy);
    
  connect(m_UnifyTreeThread, SIGNAL( finished() ), this, SLOT(unifyTreeDone()));

  emit unifyingTree();

  m_UnifyTreeThread->start();
}

void PreferencesUI::reset()
{
  mds::DatabaseAPI db;
  std::string interval = db.GetSetting(
    mds::DatabaseAPI::AUTO_REFRESH_INTERVAL);
  std::string index = db.GetSetting(
    mds::DatabaseAPI::AUTO_REFRESH_SETTING);
  std::string path = db.GetSetting(
    mds::DatabaseAPI::ROOT_DIR);
  m_UnifiedTree = db.GetSettingBool(
    mds::DatabaseAPI::UNIFIED_TREE);

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
  mds::DatabaseAPI db;
  std::string path = db.GetSetting(mds::DatabaseAPI::ROOT_DIR);

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
  mds::DatabaseAPI db;
  db.SetSetting(mds::DatabaseAPI::AUTO_REFRESH_INTERVAL, timeSpinBox->value());
  db.SetSetting(mds::DatabaseAPI::AUTO_REFRESH_SETTING, settingComboBox->currentIndex());
  db.SetSetting(mds::DatabaseAPI::UNIFIED_TREE, copyResourcesCheckBox->isChecked());

  std::string dir = workingDirEdit->text().toStdString();
  kwsys::SystemTools::ConvertToUnixSlashes(dir);
  if(kwsys::SystemTools::FileIsDirectory(dir.c_str()))
    {
    db.SetSetting(mds::DatabaseAPI::ROOT_DIR, dir);
    }

  emit intervalChanged();
  emit settingChanged();

  QDialog::accept();
}

void PreferencesUI::unifyTreeDone()
{
  emit treeUnified();
}

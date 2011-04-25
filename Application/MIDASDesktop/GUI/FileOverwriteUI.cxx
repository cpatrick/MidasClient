#include "FileOverwriteUI.h"
#include "GUIFileOverwriteHandler.h"

FileOverwriteUI::FileOverwriteUI(QWidget* parent)
: QDialog(parent), m_Overwrite(true)
{
  setupUi(this);
  this->setModal(true);

  connect(overwriteButton, SIGNAL( released() ), this, SLOT( overwrite() ) );
  connect(useExistingButton, SIGNAL( released() ), this, SLOT( useExisting() ) );
}

FileOverwriteUI::~FileOverwriteUI()
{
}

bool FileOverwriteUI::ShouldOverwrite()
{
  return m_Overwrite;
}

bool FileOverwriteUI::ShouldApplyToAll()
{
  return this->applyToAllCheckbox->isChecked();
}

void FileOverwriteUI::setPath(const std::string& path)
{
  m_Path = path;
}

void FileOverwriteUI::overwrite()
{
  m_Overwrite = true;
  QDialog::accept();
}

void FileOverwriteUI::useExisting()
{
  m_Overwrite = false;
  QDialog::accept();
}

void FileOverwriteUI::exec()
{
  QString text = "<b>";
  text.append(m_Path.c_str());
  text.append("</b>");
  pathLabel->setText(text);
  QDialog::exec();
}

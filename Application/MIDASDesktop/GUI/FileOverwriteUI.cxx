#include "FileOverwriteUI.h"
#include "GUIFileOverwriteHandler.h"

FileOverwriteUI::FileOverwriteUI(MIDASDesktopUI* parent, GUIFileOverwriteHandler* controller)
: m_Parent(parent), m_Controller(controller)
{
  setupUi(this);
  connect(overwriteButton, SIGNAL( released() ), this, SLOT( overwrite() ) );
  connect(useExistingButton, SIGNAL( released() ), this, SLOT( useExisting() ) );
}

FileOverwriteUI::~FileOverwriteUI()
{
}

void FileOverwriteUI::setPath(const QString& path)
{
  m_Path = path.toStdString();
}

void FileOverwriteUI::overwrite()
{
  m_Controller->actionChosen(midasFileOverwriteHandler::Overwrite,
    applyToAllCheckbox->isChecked());
  QDialog::accept();
}

void FileOverwriteUI::useExisting()
{
  m_Controller->actionChosen(midasFileOverwriteHandler::UseExisting,
    applyToAllCheckbox->isChecked());
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

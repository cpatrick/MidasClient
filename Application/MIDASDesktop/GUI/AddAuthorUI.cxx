#include "AddAuthorUI.h"
#include "MIDASDesktopUI.h"

#include <QMessageBox>
#include <QString>

AddAuthorUI::AddAuthorUI(MIDASDesktopUI* parent)
: ButtonEditUI(parent)
{
  setupUi(this);
}

void AddAuthorUI::exec()
{
  firstNameEdit->setText("");
  lastNameEdit->setText("");
  firstNameEdit->setFocus(Qt::PopupFocusReason);
  QDialog::exec();
}

void AddAuthorUI::accept()
{
  QString first = firstNameEdit->text().trimmed();
  QString last = lastNameEdit->text().trimmed();

  if(last == "")
    {
    QMessageBox::critical(this, "Invalid last name",
      "You must enter a last name");
    return;
    }

  if(first.contains("/") || last.contains("/"))
    {
    QMessageBox::critical(this, "Invalid name", "Name cannot contain /");
    return;
    }

  QString author = first == "" ? last : last + ", " + first;
  emit text(author);
  
  QDialog::accept();
}
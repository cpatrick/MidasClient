#include "AddKeywordUI.h"

#include <QMessageBox>
#include <QString>

AddKeywordUI::AddKeywordUI(QWidget* parent)
: ButtonEditUI(parent)
{
  setupUi(this);
}

void AddKeywordUI::exec()
{
  keywordEdit->setText("");
  keywordEdit->setFocus(Qt::PopupFocusReason);
  QDialog::exec();
}

void AddKeywordUI::accept()
{
  QString keyword = keywordEdit->text().trimmed();

  if(keyword == "")
    {
    QMessageBox::critical(this, "Invalid keyword", "Keyword cannot be blank");
    return;
    }

  if(keyword.contains("/"))
    {
    QMessageBox::critical(this, "Invalid keyword", "Keyword cannot contain /");
    return;
    }

  emit text(keyword);

  QDialog::accept();
}

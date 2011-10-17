/******************************************************************************
 * Copyright 2011 Kitware Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
#include "AddAuthorUI.h"

#include <QMessageBox>
#include <QString>

AddAuthorUI::AddAuthorUI(QWidget* parent)
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

  if( last == "" )
    {
    QMessageBox::critical(this, "Invalid last name",
                          "You must enter a last name");
    return;
    }

  if( first.contains("/") || last.contains("/") )
    {
    QMessageBox::critical(this, "Invalid name", "Name cannot contain /");
    return;
    }

  QString author = first == "" ? last : last + ", " + first;
  emit    text(author);

  QDialog::accept();
}


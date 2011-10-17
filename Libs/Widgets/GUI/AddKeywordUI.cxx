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

  if( keyword == "" )
    {
    QMessageBox::critical(this, "Invalid keyword", "Keyword cannot be blank");
    return;
    }

  if( keyword.contains("/") )
    {
    QMessageBox::critical(this, "Invalid keyword", "Keyword cannot contain /");
    return;
    }

  emit text(keyword);

  QDialog::accept();
}


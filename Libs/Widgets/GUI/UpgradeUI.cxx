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
#include "UpgradeUI.h"

UpgradeUI::UpgradeUI(QWidget* parent)
  : QDialog(parent)
{
  this->setupUi(this);
}

UpgradeUI::~UpgradeUI()
{
}

void UpgradeUI::SetDbVersion(const std::string& version)
{
  m_DbVersion = version;
}

void UpgradeUI::SetProductVersion(const std::string& version)
{
  m_ProductVersion = version;
}

int UpgradeUI::exec()
{
  std::stringstream text;

  text << "You are using an outdated database version ("
       << m_DbVersion << "). You must upgrade your database schema "
  " to the current version (" << m_ProductVersion << ") before "
  " you can use it.  Your existing data will be preserved.<br><br>"
  "Would you like to upgrade the database now?";
  this->upgradeLabel->setText(text.str().c_str() );
  return QDialog::exec();
}


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

  text.append(m_Path.c_str() );
  text.append("</b>");
  pathLabel->setText(text);
  QDialog::exec();
}


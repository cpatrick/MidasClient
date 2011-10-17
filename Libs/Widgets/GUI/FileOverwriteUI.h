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
#ifndef __FileOverwriteUI_H
#define __FileOverwriteUI_H

#include "ui_FileOverwriteUI.h"
#include <QDialog>

class FileOverwriteUI : public QDialog, private Ui::FileOverwriteDialog
{
  Q_OBJECT
public:
  FileOverwriteUI(QWidget* parent);
  ~FileOverwriteUI();

  bool ShouldOverwrite();

  bool ShouldApplyToAll();

public slots:
  void setPath(const std::string& path);

  void overwrite();

  void useExisting();

  void exec();

signals:
  void selectionMade(int val, bool applyToAll);

private:
  std::string m_Path;
  bool        m_Overwrite;
};

#endif

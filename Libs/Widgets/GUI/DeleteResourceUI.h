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
#ifndef __DeleteResourceUI_H
#define __DeleteResourceUI_H

#include "ui_DeleteResourceUI.h"

class DeleteResourceUI :  public QDialog, private Ui::DeleteResourceDialog
{
  Q_OBJECT
public:

  DeleteResourceUI(QWidget* parent, bool server);
  ~DeleteResourceUI();

  void init();

signals:
  void deleteResource(bool deleteFile);

public slots:

  int exec();

  virtual void accept();

private:
  bool m_Server;
};

#endif // __DeleteResourceUI_H

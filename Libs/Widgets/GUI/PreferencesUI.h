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
#ifndef __PreferencesUI_H
#define __PreferencesUI_H

#include "ui_PreferencesUI.h"

class UnifyTreeThread;

class PreferencesUI :  public QDialog, private Ui::PreferencesDialog
{
  Q_OBJECT
public:
  PreferencesUI(QWidget* parent);
  ~PreferencesUI();
public slots:
  void enableActions(int index);

  void unifyTree();

  void selectWorkingDir();

  void unifyTreeDone();

  void reset();

  int exec();

  virtual void accept();

signals:
  void intervalChanged();

  void settingChanged();

  void unifyingTree(); // start

  void treeUnified();  // finish

protected:
  bool             m_UnifiedTree;
  UnifyTreeThread* m_UnifyTreeThread;
};

#endif

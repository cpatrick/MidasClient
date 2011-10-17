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
#ifndef __PullUI_H
#define __PullUI_H

#include "ui_PullUI.h"

class SynchronizerThread;
class midasSynchronizer;

class PullUI :  public QDialog, private Ui::PullDialog
{
  Q_OBJECT
public:

  PullUI(QWidget* parent, midasSynchronizer* synch);
  ~PullUI();

  void setPullId(int id);

  void setResourceType(int type);

  void setResourceName(std::string name);

  void setPull();

  void setClone();

  void setRecursive(bool value);

  SynchronizerThread * getSynchronizerThread();

  void init();

signals:
  void pulledResources();

  void enableActions(bool);

  void startingSynchronizer();

public slots:

  int exec();

  virtual void accept();

  void radioButtonChanged();

  void resetState();

  void pulled(int rc);

  void cloned(int rc);

private:

  SynchronizerThread* m_SynchronizerThread;
  midasSynchronizer*  m_Synch;
  std::string         m_TypeName;
  std::string         m_Name;
  int                 m_PullId;
  int                 m_ResourceType;

};

#endif // __PullUI_H

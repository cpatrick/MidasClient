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
#ifndef __PushUI_H
#define __PushUI_H

#include "ui_PushUI.h"

class midasSynchronizer;
class SynchronizerThread;

namespace mdo
{
class Object;
}

class PushUI :  public QDialog, private Ui::PushDialog
{
  Q_OBJECT
public:

  PushUI(QWidget* parent, midasSynchronizer* synch);
  ~PushUI();

  void setObject(mdo::Object* object);

  void setDelete(bool val);

  void init();

  SynchronizerThread * getSynchronizerThread();

signals:
  void pushedResources(int rc);

  void enableActions(bool);

public slots:

  int exec();

  void accept();

  void reject();

  void radioButtonChanged();

  void resetState();

  void deleteObject();

private:

  bool                m_Delete;
  mdo::Object*        m_Object;
  midasSynchronizer*  m_Synch;
  SynchronizerThread* m_SynchThread;
};

#endif // __PushUI_H

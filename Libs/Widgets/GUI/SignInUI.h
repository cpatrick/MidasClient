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
#ifndef __SignInUI_H
#define __SignInUI_H

#include "ui_SignInUI.h"

class SignInThread;
class midasSynchronizer;

class SignInUI :  public QDialog, private Ui::SignInDialog
{
  Q_OBJECT
public:

  SignInUI(QWidget* parent, midasSynchronizer* synch);
  ~SignInUI();

  void init();

signals:
  void createProfileRequest();

  void signingIn();

  void signedIn(bool);

public slots:

  int exec();

  virtual void accept();

  void showCreateProfileDialog();

  void profileCreated(std::string name);

  void removeProfile(std::string name);

  void signIn(bool ok);

private:
  SignInThread*      m_SignInThread;
  midasSynchronizer* m_Synch;

};

#endif // __SignInUI_H

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
#ifndef __SignInThread_H_
#define __SignInThread_H_

#include <QThread>
#include <QString>

class midasSynchronizer;

class SignInThread : public QThread
{
  Q_OBJECT
public:
  SignInThread(midasSynchronizer* synch);
  ~SignInThread();

  void SetProfile(QString profile);

  virtual void run();

signals:
  void initialized(bool ok);

private:
  midasSynchronizer* m_Synch;
  QString            m_Profile;
};

#endif

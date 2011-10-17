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
#ifndef __SynchronizerThread_H
#define __SynchronizerThread_H

#include <QThread>
#include "midasSynchronizer.h"

/**
 * Thread for pulling and pushing
 */
class SynchronizerThread : public QThread
{
  Q_OBJECT
public:
  SynchronizerThread() : m_Delete(false)
  {
  }
  ~SynchronizerThread()
  {
  }

  void SetSynchronizer(midasSynchronizer* synch)
  {
    m_Synchronizer = synch;
  }
  void SetDelete(bool shouldDelete)
  {
    m_Delete = shouldDelete;
  }

  virtual void run()
  {
    emit enableActions(false);
    emit performReturned(m_Synchronizer->Perform() );

    if( m_Delete )
      {
      // hack-ish: set this back to NULL so we can delete the auth safely
      m_Synchronizer->SetAuthenticator(NULL, false);
      delete m_Synchronizer;
      }
    emit enableActions(true);
  }

  void Cancel()
  {
    m_Synchronizer->Cancel();
  }
signals:
  void enableActions(bool val);

  void performReturned(int rc);

private:
  midasSynchronizer* m_Synchronizer;
  bool               m_Delete;
};

#endif

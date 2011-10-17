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
#ifndef __PollFilesystemThread_H
#define __PollFilesystemThread_H

#include <QThread>
class midasDatabaseProxy;

/**
 * This thread is always running while the GUI is not minimized.
 * It continually polls the filesystem to see if files were
 * deleted or modified, and emits the needToRefresh() signal
 * if changes were made to any bitstreams under the database's
 * control.
 */
class PollFilesystemThread : public QThread
{
  Q_OBJECT
public:
  PollFilesystemThread();
  ~PollFilesystemThread();

  virtual void run();

public slots:
  // Stop filesystem polling and wait for current poll to finish
  void Pause();

  // Resume filesystem polling
  void Resume();

  // End this thread permanently
  void Terminate();

signals:
  void needToRefresh();

  void paused();

private:
  bool m_Run;
  bool m_Terminate;
  bool m_DatabaseLocked;
};

#endif

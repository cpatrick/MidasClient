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
#ifndef __UpdateTreeViewThread_H
#define __UpdateTreeViewThread_H

#include <QThread>

class MidasTreeViewBase;

/**
 * Thread for updating a MIDAS tree view widget
 */
class UpdateTreeViewThread : public QThread
{
  Q_OBJECT
public:

  UpdateTreeViewThread(MidasTreeViewBase* treeView);
  ~UpdateTreeViewThread();

  virtual void run();

signals:
  void enableActions(bool val);

protected:
  MidasTreeViewBase* m_TreeView;
};

#endif

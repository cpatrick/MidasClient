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
#ifndef __ExpandTreeThread_H_
#define __ExpandTreeThread_H_

#include "midasStandardIncludes.h"

#include <QThread>
#include <QModelIndex>

class MidasTreeViewServer;
class MidasTreeModelServer;

class Midas3TreeViewServer;
class Midas3TreeModelServer;

class ExpandTreeThread : public QThread
{
  Q_OBJECT
public:
  ExpandTreeThread(MidasTreeViewServer* view, MidasTreeModelServer* model, std::string uuid, bool select);
  ExpandTreeThread(Midas3TreeViewServer* view, Midas3TreeModelServer* model, std::string uuid, bool select);
  ~ExpandTreeThread();

  virtual void run();

signals:
  void expand(const QModelIndex& index);

  void select(const QModelIndex& index);

  void enableActions(bool val);

private:
  MidasTreeViewServer*   m_ParentUI;
  MidasTreeModelServer*  m_ParentModel;
  Midas3TreeViewServer*  m_ParentUI3;
  Midas3TreeModelServer* m_ParentModel3;
  std::string            m_Uuid;
  bool                   m_Select;
  int                    m_Version;
};

#endif

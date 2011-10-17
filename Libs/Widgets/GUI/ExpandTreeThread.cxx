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
#include "ExpandTreeThread.h"
#include "MidasTreeViewServer.h"
#include "MidasTreeModelServer.h"
#include "mwsTreePath.h"

ExpandTreeThread::ExpandTreeThread(MidasTreeViewServer* view, MidasTreeModelServer* model, std::string uuid,
                                   bool select) :
  m_ParentUI(view),
  m_ParentModel(model),
  m_ParentUI3(NULL),
  m_ParentModel3(NULL),
  m_Uuid(uuid),
  m_Select(select),
  m_Version(2)
{
}

ExpandTreeThread::ExpandTreeThread(Midas3TreeViewServer* view, Midas3TreeModelServer* model, std::string uuid,
                                   bool select) :
  m_ParentUI(NULL),
  m_ParentModel(NULL),
  m_ParentUI3(view),
  m_ParentModel3(model),
  m_Uuid(uuid),
  m_Select(select),
  m_Version(3)
{
}

ExpandTreeThread::~ExpandTreeThread()
{
}

void ExpandTreeThread::run()
{
  emit enableActions(false);

  if( m_Version == 3 )
    {
    // TODO version 3 of this
    }
  else
    {
    std::vector<std::string> path =
      mws::TreePath::PathFromRoot(m_Uuid);
    for( std::vector<std::string>::iterator i = path.begin();
         i != path.end(); ++i )
      {
      QModelIndex index = m_ParentModel->GetIndexByUuid(*i);
      m_ParentModel->fetchMore(index);

      emit expand(index);
      }

    if( m_Select )
      {
      QModelIndex index = m_ParentModel->GetIndexByUuid(m_Uuid);
      if( index.isValid() )
        {
        emit select(index);
        }
      }
    }
  emit enableActions(true);
}


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
#include "UnifyTreeThread.h"
#include "PollFilesystemThread.h"
#include "mdsDatabaseAPI.h"

UnifyTreeThread::UnifyTreeThread()
{
  m_Copy = false;
}

UnifyTreeThread::~UnifyTreeThread()
{
}

void UnifyTreeThread::setCopy(bool val)
{
  m_Copy = val;
}

bool UnifyTreeThread::isCopy()
{
  return m_Copy;
}

void UnifyTreeThread::run()
{
  mds::DatabaseAPI db;

  if( db.UnifyTree(m_Copy) )
    {
    db.GetLog()->Message("Finished relocating resources");
    }
  else
    {
    db.GetLog()->Error("Errors occurred while relocating resources");
    }
}


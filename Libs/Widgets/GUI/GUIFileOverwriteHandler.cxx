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
#include "GUIFileOverwriteHandler.h"
#include "FileOverwriteUI.h"

GUIFileOverwriteHandler::GUIFileOverwriteHandler(FileOverwriteUI* dialog)
  : m_Dialog(dialog), m_ApplyToAll(false)
{
  connect(this, SIGNAL( displayDialog() ),
          dialog, SLOT( exec() ),
          Qt::BlockingQueuedConnection);
}

GUIFileOverwriteHandler::~GUIFileOverwriteHandler()
{
}

midasFileOverwriteHandler::Action GUIFileOverwriteHandler::HandleConflict(
  std::string path)
{
  if( !m_ApplyToAll )
    {
    m_Dialog->setPath(path);
    emit displayDialog(); // modal dialog, blocking connection
    m_ApplyToAll = m_Dialog->ShouldApplyToAll();
    }
  return m_Dialog->ShouldOverwrite() ? Overwrite : UseExisting;
}


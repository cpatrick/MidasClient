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
#include "TreeViewUpdateHandler.h"

#include "mdoObject.h"

TreeViewUpdateHandler::TreeViewUpdateHandler(QTreeView* view)
  : m_View(view)
{
  connect(this, SIGNAL(ResourceAdded(mdo::Object *) ),
          m_View, SLOT(addResource(mdo::Object *) ), Qt::DirectConnection);
  connect(this, SIGNAL(ResourceUpdated(mdo::Object *) ),
          m_View, SLOT(updateResource(mdo::Object *) ) );
  // Delete operation requires a blocking queued connection since we use another
  // thread to delete resources
  connect(this, SIGNAL(ResourceDeleted(mdo::Object *) ),
          m_View, SLOT(deleteResource(mdo::Object *) ), Qt::BlockingQueuedConnection);
}

TreeViewUpdateHandler::~TreeViewUpdateHandler()
{
}

void TreeViewUpdateHandler::AddedResource(mdo::Object* resource)
{
  emit ResourceAdded(resource);
}

void TreeViewUpdateHandler::UpdatedResource(mdo::Object* resource)
{
  emit ResourceUpdated(resource);
}

void TreeViewUpdateHandler::DeletedResource(mdo::Object* resource)
{
  emit ResourceDeleted(resource);
}


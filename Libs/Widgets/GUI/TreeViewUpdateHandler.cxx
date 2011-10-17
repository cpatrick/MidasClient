#include "TreeViewUpdateHandler.h"

#include "mdoObject.h"

TreeViewUpdateHandler::TreeViewUpdateHandler(QTreeView* view)
: m_View(view)
{
  connect(this, SIGNAL(ResourceAdded(mdo::Object*)),
    m_View, SLOT(addResource(mdo::Object*)), Qt::DirectConnection);
  connect(this, SIGNAL(ResourceUpdated(mdo::Object*)),
          m_View, SLOT(updateResource(mdo::Object*)));
  // Delete operation requires a blocking queued connection since we use another
  // thread to delete resources
  connect(this, SIGNAL(ResourceDeleted(mdo::Object*)),
          m_View, SLOT(deleteResource(mdo::Object*)), Qt::BlockingQueuedConnection);
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

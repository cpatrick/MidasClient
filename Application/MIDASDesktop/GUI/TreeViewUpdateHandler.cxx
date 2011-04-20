#include "TreeViewUpdateHandler.h"

#include "MidasTreeView.h"
#include "mdoObject.h"

TreeViewUpdateHandler::TreeViewUpdateHandler(MidasTreeView* view)
: m_View(view)
{
  connect(this, SIGNAL( ResourceAdded(mdo::Object*) ),
          m_View, SLOT( addResource(mdo::Object*) ), Qt::BlockingQueuedConnection );
  connect(this, SIGNAL( ResourceUpdated(mdo::Object*) ),
          m_View, SLOT( updateResource(mdo::Object*) ), Qt::BlockingQueuedConnection );
  connect(this, SIGNAL( ResourceDeleted(mdo::Object*) ),
          m_View, SLOT( deleteResource(mdo::Object*) ), Qt::BlockingQueuedConnection );
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

#include "TreeViewUpdateHandler.h"

#include "MidasTreeView.h"
#include "mdoObject.h"

TreeViewUpdateHandler::TreeViewUpdateHandler(MidasTreeView* view)
: m_View(view)
{
  connect(this, SIGNAL( ResourceAdded(mdo::Object*) ),
          m_View, SLOT( AddResource(mdo::Object*) ) );
  connect(this, SIGNAL( ResourceUpdated(mdo::Object*) ),
          m_View, SLOT( UpdateResource(mdo::Object*) ) );
  connect(this, SIGNAL( ResourceDeleted(mdo::Object*) ),
          m_View, SLOT( DeleteResource(mdo::Object*) ) );
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

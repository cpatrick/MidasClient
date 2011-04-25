#include "DeleteThread.h"

#include "MidasTreeItem.h"
#include "MidasCommunityTreeItem.h"
#include "MidasCollectionTreeItem.h"
#include "MidasItemTreeItem.h"
#include "MidasBitstreamTreeItem.h"
#include "mdsCommunity.h"
#include "mdsCollection.h"
#include "mdsItem.h"
#include "mdsBitstream.h"
#include "mdoCommunity.h"
#include "mdoCollection.h"
#include "mdoItem.h"
#include "mdoBitstream.h"

DeleteThread::DeleteThread()
{
  m_DeleteOnDisk = false;
  m_Resource = NULL;
}

DeleteThread::~DeleteThread()
{
}

void DeleteThread::SetResource(MidasTreeItem* resource)
{
  m_Resource = resource; 
}

void DeleteThread::SetDeleteOnDisk(bool val)
{
  m_DeleteOnDisk = val;
}

void DeleteThread::run()
{
  emit enableActions(false);
  
  mds::DatabaseAPI db;
  if(!db.DeleteResource(m_Resource->getUuid(), m_DeleteOnDisk))
    {
    emit errorMessage("Error deleting resource " + m_Resource->data(0).toString());
    }
  emit enableActions(true);
}

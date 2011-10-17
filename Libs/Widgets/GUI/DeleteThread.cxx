#include "DeleteThread.h"

#include "MidasTreeItem.h"
#include "Midas3TreeItem.h"
#include "Midas3FolderTreeItem.h"
#include "Midas3ItemTreeItem.h"

#include "m3doFolder.h"
#include "m3dsFolder.h"
#include "m3doItem.h"
#include "m3dsItem.h"

DeleteThread::DeleteThread()
{
  m_DeleteOnDisk = false;
  m_Resource = NULL;
  m_Resource3 = NULL;
}

DeleteThread::~DeleteThread()
{
}

void DeleteThread::SetResource(MidasTreeItem* resource)
{
  m_Resource = resource;
}

void DeleteThread::SetResource3(Midas3TreeItem* resource)
{
  m_Resource3 = resource;
}

void DeleteThread::SetDeleteOnDisk(bool val)
{
  m_DeleteOnDisk = val;
}

void DeleteThread::run()
{
  emit enableActions(false);

  if( m_Resource )
    {
    mds::DatabaseAPI db;
    if( !db.DeleteResource(m_Resource->GetUuid(), m_DeleteOnDisk) )
      {
      emit errorMessage("Error deleting resource " + m_Resource->GetData(0).toString() );
      }
    }
  else if( m_Resource3 )
    {
    Midas3FolderTreeItem* folderTreeItem = NULL;
    Midas3ItemTreeItem*   itemTreeItem = NULL;

    if( (folderTreeItem = dynamic_cast<Midas3FolderTreeItem *>(m_Resource3) ) != NULL )
      {
      m3ds::Folder folder;
      folder.SetObject(folderTreeItem->GetFolder() );
      if( !folder.Delete(m_DeleteOnDisk) )
        {
        emit errorMessage("Error deleting folder " + QString(folderTreeItem->GetFolder()->GetName().c_str() ) );
        }
      }
    else if( (itemTreeItem = dynamic_cast<Midas3ItemTreeItem *>(m_Resource3) ) != NULL )
      {
      m3ds::Item item;
      item.SetObject(itemTreeItem->GetItem() );
      if( !item.Delete(m_DeleteOnDisk) )
        {
        emit errorMessage("Error deleting item " + QString(itemTreeItem->GetItem()->GetName().c_str() ) );
        }
      }
    }
  emit enableActions(true);
}


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


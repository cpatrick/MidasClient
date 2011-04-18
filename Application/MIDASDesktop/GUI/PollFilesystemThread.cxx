#include "PollFilesystemThread.h"
#include "mdsDatabaseAPI.h"

PollFilesystemThread::PollFilesystemThread()
{
  m_Run = true;
  m_DatabaseLocked = false;
  m_Terminate = false;
}

PollFilesystemThread::~PollFilesystemThread()
{
}

void PollFilesystemThread::Pause()
{
  m_Run = false;
  while(m_DatabaseLocked) {}
}

void PollFilesystemThread::Resume()
{
  m_Run = true;
}

void PollFilesystemThread::Terminate()
{
  m_Terminate = true;
}

void PollFilesystemThread::run()
{
  while(!m_Terminate)
    {
    if(m_Run)
      {
      m_DatabaseLocked = true;
      mds::DatabaseAPI db;
      if(m_Run && db.CheckModifiedBitstreams())
        {
        emit needToRefresh();
        }
      m_DatabaseLocked = false;
      }
    this->sleep(5);
    }
}
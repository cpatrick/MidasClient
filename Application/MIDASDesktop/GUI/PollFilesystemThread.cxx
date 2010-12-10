#include "PollFilesystemThread.h"
#include "midasDatabaseProxy.h"

PollFilesystemThread::PollFilesystemThread()
{
  m_Run = true;
  m_DatabaseLocked = false;
}

PollFilesystemThread::~PollFilesystemThread()
{
  delete m_Database;
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

void PollFilesystemThread::run()
{
  while(true)
    {
    if(m_Run)
      {
      m_DatabaseLocked = true;
      if(m_Run && m_Database->CheckModifiedBitstreams())
        {
        emit needToRefresh();
        }
      m_DatabaseLocked = false;
      }
    this->sleep(5);
    }
}
#include "PollFilesystemThread.h"
#include "midasDatabaseProxy.h"

PollFilesystemThread::PollFilesystemThread()
{
  m_Run = true;
}

PollFilesystemThread::~PollFilesystemThread()
{
  delete m_Database;
}

void PollFilesystemThread::Pause()
{
  m_Run = false;
}

void PollFilesystemThread::Resume()
{
  m_Run = true;
}

void PollFilesystemThread::run()
{
  while(true)
    {
    if(m_Run && m_Database->CheckModifiedBitstreams())
      {
      emit needToRefresh();
      }
    this->sleep(5);
    }
}
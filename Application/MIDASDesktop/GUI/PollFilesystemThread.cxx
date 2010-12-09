#include "PollFilesystemThread.h"
#include "midasDatabaseProxy.h"

PollFilesystemThread::PollFilesystemThread()
{
}

PollFilesystemThread::~PollFilesystemThread()
{
  delete m_Database;
}

void PollFilesystemThread::run()
{
  while(true)
    {
    if(m_Database->CheckModifiedBitstreams())
      {
      emit needToRefresh();
      }
    this->sleep(5);
    }
}
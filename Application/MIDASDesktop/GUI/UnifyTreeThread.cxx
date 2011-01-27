#include "UnifyTreeThread.h"
#include "MIDASDesktopUI.h"
#include "midasDatabaseProxy.h"
#include "PollFilesystemThread.h"

UnifyTreeThread::UnifyTreeThread(MIDASDesktopUI* parent) : m_Parent(parent)
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
  midasDatabaseProxy tempDbProxy(m_Parent->getDatabaseProxy()->GetDatabasePath());
  tempDbProxy.SetLog(m_Parent->GetLog());
  m_Parent->getPollFilesystemThread()->Pause();

  if(tempDbProxy.UnifyTree(m_Copy))
    {
    m_Parent->GetLog()->Message("Finished relocating resources");
    }
  else
    {
    m_Parent->GetLog()->Error("Errors occurred while relocating resources");
    }
  m_Parent->getPollFilesystemThread()->Resume();

  emit threadComplete();
}

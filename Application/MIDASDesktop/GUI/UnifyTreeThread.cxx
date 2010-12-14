#include "UnifyTreeThread.h"
#include "MIDASDesktopUI.h"
#include "midasDatabaseProxy.h"
#include "PollFilesystemThread.h"

UnifyTreeThread::UnifyTreeThread(MIDASDesktopUI* parent) : m_Parent(parent)
{
}

UnifyTreeThread::~UnifyTreeThread()
{
}

void UnifyTreeThread::run()
{
  midasDatabaseProxy tempDbProxy(m_Parent->getDatabaseProxy()->GetDatabasePath());
  tempDbProxy.SetLog(m_Parent->GetLog());
  m_Parent->getPollFilesystemThread()->Pause();

  if(tempDbProxy.UnifyTree())
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

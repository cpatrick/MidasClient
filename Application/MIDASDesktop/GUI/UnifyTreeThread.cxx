#include "UnifyTreeThread.h"
#include "MIDASDesktopUI.h"
#include "PollFilesystemThread.h"
#include "mdsDatabaseAPI.h"

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
  m_Parent->getPollFilesystemThread()->Pause();

  if(mds::DatabaseAPI::Instance()->UnifyTree(m_Copy))
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

#include "UnifyTreeThread.h"
#include "MIDASDesktopUI.h"
#include "midasDatabaseProxy.h"

UnifyTreeThread::UnifyTreeThread(MIDASDesktopUI* parent) : m_Parent(parent)
{
}

UnifyTreeThread::~UnifyTreeThread()
{
}

void UnifyTreeThread::run()
{
  m_Parent->getDatabaseProxy()->UnifyTree();

  emit threadComplete();
}

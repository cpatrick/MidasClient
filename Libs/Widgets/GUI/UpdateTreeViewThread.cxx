#include "UpdateTreeViewThread.h"
#include "MidasTreeViewBase.h"

UpdateTreeViewThread::UpdateTreeViewThread(MidasTreeViewBase* treeView)
: m_TreeView(treeView)
{
}

UpdateTreeViewThread::~UpdateTreeViewThread()
{
}

void UpdateTreeViewThread::run()
{
  emit enableActions(false);
  m_TreeView->Update();
  emit enableActions(true);
}
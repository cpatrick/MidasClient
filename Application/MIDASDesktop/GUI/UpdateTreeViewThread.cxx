#include "UpdateTreeViewThread.h"
#include "MidasTreeView.h"

UpdateTreeViewThread::UpdateTreeViewThread(MidasTreeView* treeView)
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
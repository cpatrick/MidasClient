#include "ReadDatabaseThread.h"
#include "MIDASDesktopUI.h"
#include "mdsDatabaseAPI.h"

void ReadDatabaseThread::run()
{
  emit enableActions(false);
  m_Parent->getTreeViewClient()->Update();
  emit enableActions(true);
  emit threadComplete();
}
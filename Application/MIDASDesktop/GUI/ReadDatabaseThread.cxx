#include "ReadDatabaseThread.h"
#include "MIDASDesktopUI.h"
#include "midasDatabaseProxy.h"

void ReadDatabaseThread::run()
{
  emit enableActions(false);
  midasDatabaseProxy* oldDbProxy = m_Parent->getTreeViewClient()->GetDatabaseProxy();

  midasDatabaseProxy* tempDbProxy = new midasDatabaseProxy(oldDbProxy->GetDatabasePath());
  tempDbProxy->SetLog(oldDbProxy->GetLog());
  m_Parent->getTreeViewClient()->SetDatabaseProxy(tempDbProxy);
  m_Parent->getTreeViewClient()->Update();
  m_Parent->getTreeViewClient()->SetDatabaseProxy(oldDbProxy);

  delete tempDbProxy;
  emit enableActions(true);
  emit threadComplete();
}
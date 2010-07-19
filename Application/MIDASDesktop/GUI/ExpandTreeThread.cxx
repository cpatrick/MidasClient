#include "ExpandTreeThread.h"
#include "MidasTreeViewServer.h"
#include "MidasTreeModelServer.h"
#include "mwsTreePath.h"

ExpandTreeThread::ExpandTreeThread(MidasTreeViewServer* view,
                                   MidasTreeModelServer* model,
                                   std::string uuid):
m_ParentUI(view),
m_ParentModel(model),
m_Uuid(uuid)
{
}

ExpandTreeThread::~ExpandTreeThread()
{
}

void ExpandTreeThread::run()
{
  std::vector<std::string> path = 
    mws::TreePath::PathFromRoot(m_Uuid);

  for(std::vector<std::string>::iterator i = path.begin();
      i != path.end(); ++i)
    {
    QModelIndex index = m_ParentModel->getIndexByUuid(*i);
    m_ParentModel->fetchMore(index);
    emit expand(index);
    }

  QModelIndex index = m_ParentModel->getIndexByUuid(m_Uuid);
  if(index.isValid())
    {
    emit select(index);
    }

  emit threadComplete();
}
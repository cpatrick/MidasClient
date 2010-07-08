#include "ExpandTreeThread.h"
#include "MidasTreeViewServer.h"
#include "MidasTreeModelServer.h"
#include "mwsTreePath.h"

ExpandTreeThread::ExpandTreeThread()
{
  m_ParentUI = NULL;
  m_ParentModel = NULL;
}

ExpandTreeThread::~ExpandTreeThread()
{
}

void ExpandTreeThread::SetParentUI(MidasTreeViewServer* parent)
{
  m_ParentUI = parent;
}

void ExpandTreeThread::SetParentModel(MidasTreeModelServer* parent)
{
  m_ParentModel = parent;
}

void ExpandTreeThread::SetUuid(std::string uuid)
{
  m_Uuid = uuid;
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
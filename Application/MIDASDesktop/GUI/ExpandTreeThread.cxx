#include "ExpandTreeThread.h"
#include "MidasTreeView.h"
#include "MidasTreeModel.h"
#include "mdoObject.h"
#include "mdoCommunity.h"
#include "mdoCollection.h"
#include "mdoItem.h"
#include "mdoBitstream.h"
#include "mwsCommunity.h"
#include "mwsCollection.h"
#include "mwsItem.h"
#include "mwsBitstream.h"

ExpandTreeThread::ExpandTreeThread()
{
  m_ParentUI = NULL;
  m_ParentModel = NULL;
  m_Object = NULL;
}

ExpandTreeThread::~ExpandTreeThread()
{
}

void ExpandTreeThread::SetParentUI(MidasTreeView* parent)
{
  m_ParentUI = parent;
}

void ExpandTreeThread::SetParentModel(MidasTreeModel* parent)
{
  m_ParentModel = parent;
}

void ExpandTreeThread::SetObject(mdo::Object* object)
{
  m_Object = object;
}

void ExpandTreeThread::run()
{
  std::vector<std::string> path; //path of uuids to the root

  mdo::Community* comm = NULL;
  mdo::Collection* coll = NULL;
  mdo::Item* item = NULL;
  mdo::Bitstream* bitstream = NULL;

  while(true)
    {
    if((comm = dynamic_cast<mdo::Community*>(m_Object)) != NULL)
      {
      mws::Community remote;
      remote.SetWebAPI(mws::WebAPI::Instance());
      remote.SetObject(comm);
      if(path.size() == 0)
        {
        remote.Fetch();
        }
      remote.FetchParent();
      m_Object = comm->GetParentCommunity();
      path.push_back(comm->GetUuid());
      }
    else if((coll = dynamic_cast<mdo::Collection*>(m_Object)) != NULL)
      {
      mws::Collection remote;
      remote.SetWebAPI(mws::WebAPI::Instance());
      remote.SetObject(coll);
      if(path.size() == 0)
        {
        remote.Fetch();
        }
      remote.FetchParent();
      m_Object = coll->GetParentCommunity();
      path.push_back(coll->GetUuid());
      }
    else if((item = dynamic_cast<mdo::Item*>(m_Object)) != NULL)
      {
      mws::Item remote;
      remote.SetWebAPI(mws::WebAPI::Instance());
      remote.SetObject(item);
      if(path.size() == 0)
        {
        remote.Fetch();
        }
      remote.FetchParent();
      m_Object = item->GetParentCollection();
      path.push_back(item->GetUuid());
      }
    else if((bitstream = dynamic_cast<mdo::Bitstream*>(m_Object)) != NULL)
      {
      mws::Bitstream remote;
      remote.SetWebAPI(mws::WebAPI::Instance());
      remote.SetObject(bitstream);
      if(path.size() == 0)
        {
        remote.Fetch();
        }
      remote.FetchParent();
      m_Object = bitstream->GetParentItem();
      path.push_back(bitstream->GetUuid());
      }
    if(m_Object == NULL)
      {
      break;
      }
    }

  for(std::vector<std::string>::reverse_iterator i = path.rbegin();
      i != path.rend(); ++i)
    {
    QModelIndex index = m_ParentModel->getIndexByUuid(*i);
    m_ParentModel->fetchMore(index);
    emit expand(index);
    }

  QModelIndex index = m_ParentModel->getIndexByUuid(*(path.begin()));
  if(index.isValid())
    {
    emit select(index);
    }

  emit threadComplete();
}
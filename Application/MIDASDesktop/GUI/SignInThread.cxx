#include "SignInThread.h"

#include "MIDASDesktopUI.h"
#include "mdsDatabaseAPI.h"
#include "midasAuthenticator.h"
#include "midasSynchronizer.h"
#include "mwsWebAPI.h"

SignInThread::SignInThread(MIDASDesktopUI* parent) : m_Parent(parent)
{
}

SignInThread::~SignInThread()
{
}

void SignInThread::SetProfile(QString profile)
{
  this->m_Profile = profile;
}

void SignInThread::run()
{
  mds::DatabaseAPI db;
  std::string url = db.GetAuthProfile(m_Profile.toStdString()).Url;
  mws::WebAPI::Instance()->SetServerUrl(url.c_str());

  if(mws::WebAPI::Instance()->CheckConnection())
    {
    m_Parent->getSynchronizer()->GetAuthenticator()->SetProfile(m_Profile.toStdString());
    
    if(!m_Parent->getSynchronizer()->GetAuthenticator()->Login())
      {
      emit initialized(false);
      return;
      }

    m_Parent->getTreeViewServer()->Initialize();

    emit initialized(true);
    }
  else
    {
    emit initialized(false);
    }
}

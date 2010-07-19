#include "SignInThread.h"

#include "MIDASDesktopUI.h"
#include "midasDatabaseProxy.h"
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
  m_Parent->getDatabaseProxy()->Open();
  std::string url = m_Parent->getDatabaseProxy()->GetAuthProfile(m_Profile.toStdString()).Url;
  m_Parent->getDatabaseProxy()->Close();

  if(mws::WebAPI::Instance()->CheckConnection())
    {
    m_Parent->setServerURL(url);
    m_Parent->getDatabaseProxy()->Open();
    m_Parent->getAuthenticator()->SetProfile(m_Profile.toStdString());
    m_Parent->getDatabaseProxy()->Close();
    m_Parent->getSynchronizer()->GetAuthenticator()->SetProfile(m_Profile.toStdString());
    
    m_Parent->getTreeViewServer()->SetWebAPI(mws::WebAPI::Instance());
    m_Parent->getTreeViewServer()->Initialize();

    emit initialized(true);
    }
  else
    {
    emit initialized(false);
    }
}

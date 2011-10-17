#include "SignInThread.h"

#include "mdsDatabaseAPI.h"
#include "midasAuthenticator.h"
#include "midasSynchronizer.h"
#include "mwsWebAPI.h"

SignInThread::SignInThread(midasSynchronizer* synch)
  : m_Synch(synch)
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
  std::string      url = db.GetAuthProfile(m_Profile.toStdString() ).Url;

  mws::WebAPI::Instance()->SetServerUrl(url.c_str() );

  if( mws::WebAPI::Instance()->CheckConnection() )
    {
    m_Synch->GetAuthenticator()->SetProfile(m_Profile.toStdString() );

    if( !m_Synch->GetAuthenticator()->Login() )
      {
      emit initialized(false);
      return;
      }

    emit initialized(true);
    }
  else
    {
    emit initialized(false);
    }
}


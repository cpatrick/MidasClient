/******************************************************************************
 * Copyright 2011 Kitware Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
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


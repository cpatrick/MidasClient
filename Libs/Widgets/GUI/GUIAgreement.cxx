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
#include "GUIAgreement.h"
#include "mwsRestResponseParser.h"
#include "midasSynchronizer.h"
#include "midasAuthenticator.h"
#include "AgreementUI.h"

GUIAgreement::GUIAgreement(AgreementUI* dialog)
  : m_Dialog(dialog)
{
  connect(this, SIGNAL(displayDialog() ),
          m_Dialog, SLOT(exec() ),
          Qt::BlockingQueuedConnection);
}

GUIAgreement::~GUIAgreement()
{
}

bool GUIAgreement::HandleAgreement(midasSynchronizer *synch)
{
  // first make sure user isn't anonymous
  if( synch->GetAuthenticator()->IsAnonymous() )
    {
    std::stringstream text;
    text << "Error: Anonymous users cannot pull this resource "
         << "because it has a license agreement. Please create a user "
         << "via the web interface and make a profile for that user.";
    emit errorMessage(text.str().c_str() );
    return false;
    }

  m_Url = mws::WebAPI::Instance()->GetServerUrl();
  QString filteredString(m_Url.c_str() );
  m_Url = filteredString.replace("/api/rest", "").toStdString();
  m_Url = filteredString.replace("?method=", "").toStdString();
  m_Url = midasUtils::TrimTrailingSlash(m_Url);
  m_Url += "/";
  m_Url += QString(midasUtils::GetTypeName(
                     synch->GetResourceType() ).c_str() ).toLower().toStdString();
  m_Url += "/view/";
  m_Url += synch->GetServerHandle();

  m_Dialog->SetUrl(m_Url.c_str() );

  // as long as they hit ok, keep presenting the dialog until
  // they are verified as a member of the agreed group
  while( true )
    {
    if( this->checkUserHasAgreed(synch) )
      {
      return true;
      }
    emit displayDialog(); // blocking signal, await return from modal dialog
    if( m_Dialog->WasCanceled() )
      {
      return false;
      }
    }
}

bool GUIAgreement::checkUserHasAgreed(midasSynchronizer* synch)
{
  emit        checkingAgreement();
  std::string hasAgreed;

  if( !mws::WebAPI::Instance()->CheckUserAgreement(
        synch->GetResourceType(), atoi(synch->GetServerHandle().c_str() ),
        hasAgreed) )
    {
    emit errorMessage("Failed when querying server for user agreement validation");
    return true;
    }
  return hasAgreed == "1";
}


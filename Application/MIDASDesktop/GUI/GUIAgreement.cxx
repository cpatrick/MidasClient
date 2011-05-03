#include "GUIAgreement.h"
#include "mwsRestXMLParser.h"
#include "midasSynchronizer.h"
#include "midasAuthenticator.h"
#include "AgreementUI.h"

GUIAgreement::GUIAgreement(AgreementUI* dialog)
: m_Dialog(dialog)
{
  connect(this, SIGNAL(displayDialog()),
          m_Dialog, SLOT(exec()),
          Qt::BlockingQueuedConnection);
}

GUIAgreement::~GUIAgreement()
{
}

bool GUIAgreement::HandleAgreement(midasSynchronizer *synch)
{
  //first make sure user isn't anonymous
  if(synch->GetAuthenticator()->IsAnonymous())
    {
    std::stringstream text;
    text << "Error: Anonymous users cannot pull this resource "
      << "because it has a license agreement. Please create a user "
      << "via the web interface and make a profile for that user.";
    emit errorMessage(text.str().c_str());
    return false;
    }

  m_Url = mws::WebAPI::Instance()->GetServerUrl();
  kwsys::SystemTools::ReplaceString(m_Url, "/api/rest", "");
  m_Url = midasUtils::TrimTrailingSlash(m_Url);
  m_Url += "/";
  m_Url += kwsys::SystemTools::LowerCase(
    midasUtils::GetTypeName(synch->GetResourceType()));
  m_Url += "/view/";
  m_Url += synch->GetServerHandle();

  m_Dialog->SetUrl(m_Url.c_str());

  //as long as they hit ok, keep presenting the dialog until
  //they are verified as a member of the agreed group
  while(true)
    {
    if(this->checkUserHasAgreed(synch))
      {
      return true;
      }
    emit displayDialog(); //blocking signal, await return from modal dialog
    if(m_Dialog->WasCanceled())
      {
      return false;
      }
    }
}

bool GUIAgreement::checkUserHasAgreed(midasSynchronizer* synch)
{
  emit checkingAgreement();
  std::string hasAgreed;
  
  if(!mws::WebAPI::Instance()->CheckUserAgreement(
     synch->GetResourceType(), atoi(synch->GetServerHandle().c_str()),
     hasAgreed))
    {
    emit errorMessage("Failed when querying server for user agreement validation");
    return true;
    }
  return hasAgreed == "1";
}

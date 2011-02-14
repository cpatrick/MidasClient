#include "GUIAgreement.h"
#include "MIDASDesktopUI.h"
#include "mwsRestXMLParser.h"
#include "midasSynchronizer.h"
#include "midasAuthenticator.h"

#if defined(_WIN32)
# include <windows.h>
#else
# include <unistd.h>
#endif

GUIAgreement::GUIAgreement(MIDASDesktopUI* parent)
: m_Parent(parent)
{
  connect(this, SIGNAL(checkingAgreement()),
          m_Parent, SLOT(checkingUserAgreement()));
  connect(this, SIGNAL(displayDialog()),
          m_Parent, SLOT(showUserAgreementDialog()));
}

GUIAgreement::~GUIAgreement()
{
}

bool GUIAgreement::HandleAgreement(midasSynchronizer *synch)
{
  //first make sure user isn't anonymous
  if(m_Parent->getSynchronizer()->GetAuthenticator()->IsAnonymous())
    {
    std::stringstream text;
    text << "Error: Anonymous users cannot pull this resource "
      << "because it has a license agreement. Please create a user "
      << "via the web interface and make a profile for that user.";
    m_Parent->GetLog()->Error(text.str());
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

  //as long as they hit ok, keep presenting the dialog until
  //they are verified as a member of the agreed group
  while(true)
    {
    if(this->checkUserHasAgreed(synch))
      {
      return true;
      }
    m_Done = false;
    m_Canceled = false;
    emit displayDialog();

    while(!m_Canceled && !m_Done)
      {
      #if defined(_WIN32)
        Sleep(25);
      #else
        usleep(25000);
      #endif
      }

    if(m_Canceled)
      {
      return false;
      }
    }
}

void GUIAgreement::finish()
{
  m_Done = true;
}

void GUIAgreement::cancel()
{
  m_Canceled = true;
}

bool GUIAgreement::checkUserHasAgreed(midasSynchronizer* synch)
{
  emit checkingAgreement();

  std::stringstream url;
  url << "midas.check.user.agreement?id=" << synch->GetServerHandle()
    << "&type=" << synch->GetResourceType();
  mws::RestXMLParser parser;
  std::string hasAgreed;
  parser.AddTag("/rsp/hasAgreed", hasAgreed);
  mws::WebAPI::Instance()->GetRestAPI()->SetXMLParser(&parser);

  if(!mws::WebAPI::Instance()->Execute(url.str().c_str(),
     m_Parent->getSynchronizer()->GetAuthenticator()))
    {
    std::stringstream error;
    error << "Failed when querying server for user agreement validation: "
      << mws::WebAPI::Instance()->GetErrorMessage() << std::endl;
    m_Parent->GetLog()->Error(error.str());
    return true;
    }
  return hasAgreed == "1";
}

std::string GUIAgreement::getUrl()
{
  return m_Url;
}

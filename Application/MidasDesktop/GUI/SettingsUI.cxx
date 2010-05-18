#include "SettingsUI.h"

#include "Utils.h"
#include "mwsSettings.h"
#include "MIDASDesktopUI.h"
#include "mwsWebAPI.h"

SettingsUI::SettingsUI(DionysusUI *parent):parent(parent)
{
  setupUi(this); // this sets up GUI
  connect( checkConnectionSettingsButton, SIGNAL( clicked() ), this, SLOT( checkConnectionSettings() ) );
}

/** Destructor */
SettingsUI::~SettingsUI()
{
}

void SettingsUI::init(int currenttab)
{
  settingTab->setCurrentIndex(currenttab);

  midasURLEdit->setText(mws::Settings::GetInstance()->GetServerURL().c_str());

  searchInAllWordRadioButton->setChecked(mws::Settings::GetInstance()->GetSearchInWords());
  searchInAtLeastRadioButton->setChecked(!mws::Settings::GetInstance()->GetSearchInWords());
  searchInTitlesCheckBox->setChecked(mws::Settings::GetInstance()->GetSearchInTitles());
  searchInKeywordsCheckBox->setChecked(mws::Settings::GetInstance()->GetSearchInKeywords());
  searchInAbstractsCheckBox->setChecked(mws::Settings::GetInstance()->GetSearchInAbstracts());
  searchInSponsorsCheckBox->setChecked(mws::Settings::GetInstance()->GetSearchInSponsors());
  searchInAuthorsCheckBox->setChecked(mws::Settings::GetInstance()->GetSearchInAuthors());
  searchInDescriptionsCheckBox->setChecked(mws::Settings::GetInstance()->GetSearchInDescriptions());
  searchInBitstreamsCheckBox->setChecked(mws::Settings::GetInstance()->GetSearchInBitstreams());
 
  QPixmap unknownPixmap(":icons/QuestionMark_48x48.png"); 
  checkServerConnectivityLabel->setPixmap( unknownPixmap );
}

/** */
int SettingsUI::exec(int currenttab)
{
  this->init(currenttab); 
  return QDialog::exec(); 
}
  
/** */
void SettingsUI::accept()
{  
  mws::Settings::GetInstance()->SetServerURL( midasURLEdit->text().toStdString().c_str() );
  
  // ------------- search settings ------------- 
  mws::Settings::GetInstance()->SetSearchInWords( searchInAllWordRadioButton->isChecked() );
  mws::Settings::GetInstance()->SetSearchInTitles( searchInTitlesCheckBox->isChecked() );
  mws::Settings::GetInstance()->SetSearchInKeywords( searchInKeywordsCheckBox->isChecked() );
  mws::Settings::GetInstance()->SetSearchInAbstracts( searchInAbstractsCheckBox->isChecked() );
  mws::Settings::GetInstance()->SetSearchInSponsors( searchInSponsorsCheckBox->isChecked() );
  mws::Settings::GetInstance()->SetSearchInAuthors( searchInAuthorsCheckBox->isChecked() );
  mws::Settings::GetInstance()->SetSearchInDescriptions( searchInDescriptionsCheckBox->isChecked() );
  mws::Settings::GetInstance()->SetSearchInBitstreams( searchInBitstreamsCheckBox->isChecked() );
  // ------------- search settings ------------- 
  
  mws::Settings::GetInstance()->Write();

  QDialog::accept();
}

void SettingsUI::checkConnectionSettings()
{ 
  std::string midasURL = midasURLEdit->text().toStdString();

  QPixmap passTestPixmap(":icons/check.png"); 
  QPixmap failTestPixmap(":icons/delete.png"); 

  mws::WebAPI* webAPI = mws::WebAPI::Instance();
  webAPI->SetServerUrl(midasURL.c_str());
  
  // Check server connectivity
  if(webAPI->CheckConnection())
    {
    checkServerConnectivityLabel->setPixmap( passTestPixmap );

    //if(can log in?
      {
      checkLoginLabel->setPixmap( passTestPixmap ); 
      }
    /*else
      {  
      checkLoginLabel->setPixmap( failTestPixmap );
      }*/
    }
  else
    {
    checkServerConnectivityLabel->setPixmap( failTestPixmap );
    checkLoginLabel->setPixmap( failTestPixmap ); 
    }
}

#include <iostream>
#include <fstream>
#include <stdio.h>

#include <QtGui> 
#include <Qt>

#include "mwsSettings.h" 

#include "UserUI.h"
#include "SignInUI.h"

UserUI::UserUI(QWidget *parent): QDialog(parent)
{
  setupUi(this); // this sets up GUI 
  
  m_Settings = mws::Settings::GetInstance();

  // signals/slots mechanism in action
  connect( okButton, SIGNAL( clicked()), this, SLOT(SignIn()));
}

void UserUI::SignIn()
{
  bool error = false;

  // Check if the disclaimer has been accepted by the user
  if(!error && !agreeButton->isChecked())
    {
    QMessageBox::warning(this, tr("MidasClient User Registration"),
                            tr("You must agree with the terms and conditions."));
    error = true;
    }
  else if(firstNameEdit->text().toStdString().size() == 0)
    {
    QMessageBox::warning(this, tr("MidasClient User Registration"),
                                   tr("Please set your firstname."));
    error = true;
    }
  else if(lastNameEdit->text().toStdString().size() == 0)
    {
    QMessageBox::warning(this, tr("MidasClient User Registration"),
                                   tr("Please set your lastname."));
    error = true;
    }
  else if(affiliationEdit->text().toStdString().size() == 0)
    {
    QMessageBox::warning(this, tr("MidasClient User Registration"),
                                   tr("Please set your affiliation."));
    error = true;
    }
  else if(emailEdit->text().toStdString().size() == 0)
    {
    QMessageBox::warning(this, tr("MidasClient User Registration"),
                                   tr("Please set your email."));
    error = true;
    }
  // Compare if the two passwords are equal
  else if(!strcmp(emailEdit->text().toStdString().c_str(),
            retypeEmailEdit->text().toStdString().c_str()))
    {
    emailEdit->setText(emailEdit->text().toStdString().c_str());
    }
  else
    {
    QMessageBox::information(this, tr("WARNING"),
                                   tr("The two emails don't match!"));
    retypeEmailEdit->clear();
    error = true;
    }

  if(!error)
    {
    if(!m_Settings)
      {
      std::cout << "m_InitFile is not set!" << std::endl;
      }
/*
    m_Settings->SetLastName(lastNameEdit->text().toStdString().c_str());
    m_Settings->SetFirstName(firstNameEdit->text().toStdString().c_str());
    m_Settings->SetAffiliation(affiliationEdit->text().toStdString().c_str());
    m_Settings->SetUserEmail(emailEdit->text().toStdString().c_str());
    m_Settings->WriteFile();
*/
    this->accept();
    }
  else 
    {
    this->show();
    }
}

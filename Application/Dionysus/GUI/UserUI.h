#ifndef UserUI_h
#define UserUI_h

#include "ui_UserUI.h"

namespace mws {
  class Settings;
} 
class SignInUI; 

class UserUI :  public QDialog, private Ui::UserDialog
{
  Q_OBJECT
 
public:

  UserUI(QWidget *parent = 0);
  ~UserUI(){}

public slots:
 
  void SignIn();

protected:

  mws::Settings *   m_Settings;
  SignInUI *        signinUI;

};

#endif

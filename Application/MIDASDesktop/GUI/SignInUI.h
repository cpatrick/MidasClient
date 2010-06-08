#ifndef __SignInUI_H
#define __SignInUI_H
 
#include "ui_SignInUI.h"

class MIDASDesktopUI;
class SignInThread;

class SignInUI :  public QDialog, private Ui::SignInDialog
{
  Q_OBJECT
 
public:

  SignInUI(MIDASDesktopUI *parent);
  ~SignInUI();

  void init(); 

signals:
  void signedIn();
  void createProfileRequest();

public slots:

  int exec();
  virtual void accept();
  void showCreateProfileDialog();
  void profileCreated(std::string name);

private:
  SignInThread* m_SignInThread;
  MIDASDesktopUI* parent;
  
};

#endif //__SignInUI_H

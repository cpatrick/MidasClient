#ifndef __SignInUI_H
#define __SignInUI_H

#include "ui_SignInUI.h"

class SignInThread;
class midasSynchronizer;

class SignInUI :  public QDialog, private Ui::SignInDialog
{
  Q_OBJECT
public:

  SignInUI(QWidget* parent, midasSynchronizer* synch);
  ~SignInUI();

  void init();

signals:
  void createProfileRequest();

  void signingIn();

  void signedIn(bool);

public slots:

  int exec();

  virtual void accept();

  void showCreateProfileDialog();

  void profileCreated(std::string name);

  void removeProfile(std::string name);

  void signIn(bool ok);

private:
  SignInThread*      m_SignInThread;
  midasSynchronizer* m_Synch;

};

#endif // __SignInUI_H

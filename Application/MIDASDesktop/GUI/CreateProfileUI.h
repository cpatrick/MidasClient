#ifndef __CreateProfileUI_H
#define __CreateProfileUI_H
 
#include "ui_CreateProfileUI.h"

class MIDASDesktopUI;

class CreateProfileUI :  public QDialog, private Ui::CreateProfileDialog
{
  Q_OBJECT
 
public:

  CreateProfileUI(MIDASDesktopUI *parent);
  ~CreateProfileUI(){};

  void init(); 

signals:
  void createdProfile(std::string name, std::string email,
                      std::string apiName, std::string apiKey);
  void deletedProfile(std::string name);
  void serverURLSet(std::string url);

public slots:

  int exec();
  virtual void accept();
  void fillData(const QString& profileName);
  void anonymousChanged(int state);
  void deleteProfile();

private:

  MIDASDesktopUI* parent;
  
};

#endif //__CreateProfileUI_H

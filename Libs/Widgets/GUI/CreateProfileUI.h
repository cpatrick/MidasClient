#ifndef __CreateProfileUI_H
#define __CreateProfileUI_H
 
#include "ui_CreateProfileUI.h"

class CreateProfileUI :  public QDialog, private Ui::CreateProfileDialog
{
  Q_OBJECT
 
public:

  CreateProfileUI(QWidget* parent);
  ~CreateProfileUI();

  void init(); 

signals:
  void createdProfile(std::string name, std::string email,
                      std::string apiName, std::string password,
                      std::string rootDir,
                      std::string serverURL);
  void deletedProfile(std::string name);

public slots:

  int exec();
  virtual void accept();
  void fillData(const QString& profileName);
  void anonymousChanged(int state);
  void rootDirChecked(int state);
  void deleteProfile();
  void browseRootDir();
  
};

#endif //__CreateProfileUI_H

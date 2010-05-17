#ifndef __CreateProfileUI_H
#define __CreateProfileUI_H
 
#include "ui_CreateProfileUI.h"

class DionysusUI;

class CreateProfileUI :  public QDialog, private Ui::CreateProfileDialog
{
  Q_OBJECT
 
public:

  CreateProfileUI(DionysusUI *parent);
  ~CreateProfileUI(){};

  void init(); 

signals:
  void createdProfile(std::string name, std::string email,
                      std::string apiName, std::string apiKey);
  void serverURLSet(std::string url);

public slots:

  int exec();
  virtual void accept();

private:

  DionysusUI* parent;
  
};

#endif //__CreateProfileUI_H

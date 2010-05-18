#ifndef __SettingsUI_H
#define __SettingsUI_H

#include "ui_SettingsUI.h"

class DionysusUI; 
class SettingsUI; 
 
class SettingsUI :  public QDialog, private Ui::SettingsDialog
{
  Q_OBJECT
 
public:

  SettingsUI(DionysusUI *parent);
  ~SettingsUI();

  void init(int currenttab = 0); 

public slots:

  int exec(int currenttab = 0);
  virtual void accept();

  void checkConnectionSettings(); 

private:
  DionysusUI*       parent;
};

#endif //__SettingsUI_H

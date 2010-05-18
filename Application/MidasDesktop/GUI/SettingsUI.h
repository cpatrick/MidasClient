#ifndef __SettingsUI_H
#define __SettingsUI_H

#include "ui_SettingsUI.h"

class MIDASDesktopUI; 
class SettingsUI; 
 
class SettingsUI :  public QDialog, private Ui::SettingsDialog
{
  Q_OBJECT
 
public:

  SettingsUI(MIDASDesktopUI *parent);
  ~SettingsUI();

  void init(int currenttab = 0); 

public slots:

  int exec(int currenttab = 0);
  virtual void accept();

  void checkConnectionSettings(); 

private:
  MIDASDesktopUI*       parent;
};

#endif //__SettingsUI_H

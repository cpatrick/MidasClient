#ifndef __DeleteResourceUI_H
#define __DeleteResourceUI_H
 
#include "ui_DeleteResourceUI.h"

class MIDASDesktopUI;

class DeleteResourceUI :  public QDialog, private Ui::DeleteResourceDialog
{
  Q_OBJECT
 
public:

  DeleteResourceUI(MIDASDesktopUI *parent);
  ~DeleteResourceUI();

  void init(); 

signals:
  

public slots:

  int exec();
  virtual void accept();

private:

  MIDASDesktopUI* m_Parent;
  
};

#endif //__DeleteResourceUI_H

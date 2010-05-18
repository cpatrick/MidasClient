#ifndef __UploadAgreementUI_H
#define __UploadAgreementUI_H

#include "ui_UploadAgreementUI.h"

class MIDASDesktopUI; 

class UploadAgreementUI :  public QDialog, private Ui::UploadAgreementDialog
{
  Q_OBJECT
 
public:
  UploadAgreementUI(MIDASDesktopUI *parent);
  ~UploadAgreementUI(){}

public slots:
  void reset(); 
  int exec();
  virtual void accept(); 

protected:
 MIDASDesktopUI * parent; 

};

#endif //__UploadAgreementUI_H

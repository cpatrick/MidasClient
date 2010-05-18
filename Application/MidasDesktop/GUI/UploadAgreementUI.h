#ifndef __UploadAgreementUI_H
#define __UploadAgreementUI_H

#include "ui_UploadAgreementUI.h"

class DionysusUI; 

class UploadAgreementUI :  public QDialog, private Ui::UploadAgreementDialog
{
  Q_OBJECT
 
public:
  UploadAgreementUI(DionysusUI *parent);
  ~UploadAgreementUI(){}

public slots:
  void reset(); 
  int exec();
  virtual void accept(); 

protected:
 DionysusUI * parent; 

};

#endif //__UploadAgreementUI_H

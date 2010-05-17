#ifndef __CreateMidasCommunityUI_H
#define __CreateMidasCommunityUI_H
 
#include "ui_CreateMidasCommunityUI.h"

#include <QFlags>

class DionysusUI; 

class CreateMidasCommunityUI :  public QDialog, private Ui::CreateMidasCommunityDialog
{
  Q_OBJECT
 
public:
  enum Type
    {
    Community = 0x0,
    SubCommunity = 0x1,
    ClientCommunity = 0x2,
    ClientSubCommunity = 0x3
    };
  Q_DECLARE_FLAGS(Types, Type)
  
  CreateMidasCommunityUI(DionysusUI *parent, CreateMidasCommunityUI::Types type = CreateMidasCommunityUI::Community);
  ~CreateMidasCommunityUI(){}

  void setType(Types type);

public slots:
  void reset(); 
  int exec();
  virtual void accept(); 

private:
  DionysusUI*       parent; 
  Types type; 
};

Q_DECLARE_OPERATORS_FOR_FLAGS( CreateMidasCommunityUI::Types )

#endif // __CreateMidasCommunityUI_H

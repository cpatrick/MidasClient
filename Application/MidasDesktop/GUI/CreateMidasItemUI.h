#ifndef __CreateMidasItemUI_H
#define __CreateMidasItemUI_H
 
#include "ui_CreateMidasItemUI.h"

class DionysusUI; 

class CreateMidasItemUI :  public QDialog, private Ui::CreateMidasItemDialog
{
  Q_OBJECT
 
public:
  enum Type
    {
    Item = 0x0,
    ClientItem = 0x1
    };
  Q_DECLARE_FLAGS(Types, Type)

  CreateMidasItemUI(DionysusUI *parent, CreateMidasItemUI::Types type = CreateMidasItemUI::Item);
  ~CreateMidasItemUI(){}
 
  void setType(Types type);

public slots:
  void reset(); 
  int exec();
  virtual void accept(); 

  void addAuthor(); 
  void addKeyword();

protected:
  DionysusUI*       parent;
  Types             type;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( CreateMidasItemUI::Types )

#endif //__CreateMidasItemUI_H

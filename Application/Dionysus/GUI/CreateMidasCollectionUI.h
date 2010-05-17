#ifndef __CreateMidasCollectionUI_H
#define __CreateMidasCollectionUI_H

#include "ui_CreateMidasCollectionUI.h"

class DionysusUI; 

class CreateMidasCollectionUI :  public QDialog, private Ui::CreateMidasCollectionDialog
{
  Q_OBJECT

public:
  enum Type
    {
    Collection = 0x0,
    ClientCollection = 0x1
    };
  Q_DECLARE_FLAGS(Types, Type)

  CreateMidasCollectionUI(DionysusUI *parent, CreateMidasCollectionUI::Types type = CreateMidasCollectionUI::Collection);
  ~CreateMidasCollectionUI(){}

  void setType(Types type);

public slots:
  void reset();
  int exec();
  virtual void accept();

protected:
  DionysusUI * parent;
  Types type; 
};

Q_DECLARE_OPERATORS_FOR_FLAGS( CreateMidasCollectionUI::Types )

#endif //__CreateMidasCollectionUI_H

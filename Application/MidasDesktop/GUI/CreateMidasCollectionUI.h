#ifndef __CreateMidasCollectionUI_H
#define __CreateMidasCollectionUI_H

#include "ui_CreateMidasCollectionUI.h"

class MIDASDesktopUI; 

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

  CreateMidasCollectionUI(MIDASDesktopUI *parent, CreateMidasCollectionUI::Types type = CreateMidasCollectionUI::Collection);
  ~CreateMidasCollectionUI(){}

  void setType(Types type);

public slots:
  void reset();
  int exec();
  virtual void accept();

protected:
  MIDASDesktopUI * parent;
  Types type; 
};

Q_DECLARE_OPERATORS_FOR_FLAGS( CreateMidasCollectionUI::Types )

#endif //__CreateMidasCollectionUI_H

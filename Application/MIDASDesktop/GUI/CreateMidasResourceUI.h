#ifndef __CreateMidasResourceUI_H
#define __CreateMidasResourceUI_H
 
#include "ui_CreateMidasResourceUI.h"

#include <QFlags>

class MIDASDesktopUI; 

class CreateMidasResourceUI :  public QDialog, private Ui::CreateMidasResourceDialog
{
  Q_OBJECT
 
public:
  enum Type
    {
    Community = 0x0,
    SubCommunity = 0x1,
    Collection = 0x2,
    Item = 0x3
    };
  Q_DECLARE_FLAGS(Types, Type)
  
  CreateMidasResourceUI(MIDASDesktopUI *parent);
  ~CreateMidasResourceUI(){}

  void AddCommunity(std::string path);
  void AddSubCommunity();
  void AddCollection();
  void AddItem();

  virtual bool ValidateName() const;
  virtual void SetType(Types type);

public slots:
  void reset(); 
  int exec();
  virtual void accept(); 

private:
  MIDASDesktopUI* m_Parent; 
  Types m_Type;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( CreateMidasResourceUI::Types )

#endif // __CreateMidasResourceUI_H

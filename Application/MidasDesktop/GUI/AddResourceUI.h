#ifndef __AddResourceUI_H
#define __AddResourceUI_H
 
#include "ui_AddResourceUI.h"

class MIDASDesktopUI;

class AddResourceUI :  public QDialog, private Ui::AddResourceDialog
{
  Q_OBJECT
 
public:

  AddResourceUI(MIDASDesktopUI *parent);
  ~AddResourceUI(){};

  void init(); 

signals:
  void addedResource();

public slots:
  void setParentId(std::string parentId);
  void chooseFile();
  int exec();
  virtual void accept();

  void handleRadioSelection();
private:

  MIDASDesktopUI* parent;
  
};

#endif //__AddResourceUI_H

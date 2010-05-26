#ifndef __AutoRefreshUI_H
#define __AutoRefreshUI_H
 
#include "ui_AutoRefreshUI.h"

class MIDASDesktopUI;

class AutoRefreshUI :  public QDialog, private Ui::AutoRefreshDialog
{
  Q_OBJECT
 
public:
  AutoRefreshUI(MIDASDesktopUI *parent);
  ~AutoRefreshUI(){}

public slots:
  void enableActions(int index);
  void reset(); 
  int exec();
  virtual void accept();

private:
  MIDASDesktopUI* m_parent;
};

#endif

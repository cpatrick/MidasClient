#ifndef __PullUI_H
#define __PullUI_H
 
#include "ui_PullUI.h"

class MIDASDesktopUI;

class PullUI :  public QDialog, private Ui::PullDialog
{
  Q_OBJECT
 
public:

  PullUI(MIDASDesktopUI *parent);
  ~PullUI(){};

  void setPullId(int id);
  void setResourceTypeIndex(int index);

  void init(); 

signals:
  void pulledResources();

public slots:

  int exec();
  virtual void accept();
  void radioButtonChanged();
  void resetState();

private:

  MIDASDesktopUI* parent;
  int pullId;
  int resourceTypeIndex;
  
};

#endif //__PullUI_H

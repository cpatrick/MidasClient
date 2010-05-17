#ifndef __PullUI_H
#define __PullUI_H
 
#include "ui_PullUI.h"

class DionysusUI;

class PullUI :  public QDialog, private Ui::PullDialog
{
  Q_OBJECT
 
public:

  PullUI(DionysusUI *parent);
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

  DionysusUI* parent;
  int pullId;
  int resourceTypeIndex;
  
};

#endif //__PullUI_H

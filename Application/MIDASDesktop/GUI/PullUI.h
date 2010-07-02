#ifndef __PullUI_H
#define __PullUI_H
 
#include "ui_PullUI.h"

class MIDASDesktopUI;
class SynchronizerThread;

class PullUI :  public QDialog, private Ui::PullDialog
{
  Q_OBJECT
 
public:

  PullUI(MIDASDesktopUI *parent);
  ~PullUI();

  void setPullId(int id);
  void setResourceType(int type);
  void setPull();
  void setClone();
  void setRecursive(bool value);

  void init(); 

signals:
  void pulledResources();

public slots:

  int exec();
  virtual void accept();
  void radioButtonChanged();
  void resetState();
  void pulled(int rc);
  void cloned(int rc);

private:

  MIDASDesktopUI* m_Parent;
  SynchronizerThread* m_SynchronizerThread;
  std::string m_TypeName;
  int m_PullId;
  int m_ResourceType;
  
};

#endif //__PullUI_H

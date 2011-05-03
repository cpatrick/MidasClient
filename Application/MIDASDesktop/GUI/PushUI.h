#ifndef __PushUI_H
#define __PushUI_H
 
#include "ui_PushUI.h"

class midasSynchronizer;
class SynchronizerThread;

namespace mdo
{
  class Object;
}

class PushUI :  public QDialog, private Ui::PushDialog
{
  Q_OBJECT
 
public:

  PushUI(QWidget* parent, midasSynchronizer* synch);
  ~PushUI();

  void setObject(mdo::Object* object);
  void setDelete(bool val);

  void init();

signals:
  void pushedResources(int rc);
  void enableActions(bool);

public slots:

  int exec();
  void accept();
  void reject();
  void radioButtonChanged();
  void resetState();
  void deleteObject();

private:

  bool                m_Delete;
  mdo::Object*        m_Object;
  midasSynchronizer*  m_Synch;
  SynchronizerThread* m_SynchThread;
};

#endif //__PushUI_H

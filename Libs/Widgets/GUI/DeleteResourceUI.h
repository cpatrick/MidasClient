#ifndef __DeleteResourceUI_H
#define __DeleteResourceUI_H
 
#include "ui_DeleteResourceUI.h"

class DeleteResourceUI :  public QDialog, private Ui::DeleteResourceDialog
{
  Q_OBJECT
 
public:

  DeleteResourceUI(QWidget* parent, bool server);
  ~DeleteResourceUI();

  void init(); 

signals:
  void deleteResource(bool deleteFile);

public slots:

  int exec();
  virtual void accept();

private:
  bool m_Server;
};

#endif //__DeleteResourceUI_H

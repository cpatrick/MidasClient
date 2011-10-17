#ifndef __AboutUI_H
#define __AboutUI_H

#include "ui_AboutUI.h"

class AboutUI :  public QDialog, private Ui::AboutDialog
{
  Q_OBJECT
public:
  AboutUI(QWidget* parent);
  ~AboutUI();
public slots:
  void reset();

  int exec();

private:
};

#endif // __AboutUI_H

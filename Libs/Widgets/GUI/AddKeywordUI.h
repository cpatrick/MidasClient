#ifndef __AddKeywordUI_H
#define __AddKeywordUI_H

#include "ui_AddKeywordUI.h"
#include "ButtonEditUI.h"

class AddKeywordUI : public ButtonEditUI, private Ui::AddKeywordDialog
{
  Q_OBJECT
public:
  AddKeywordUI(QWidget* parent);
public slots:
  void exec();

  void accept();

};

#endif

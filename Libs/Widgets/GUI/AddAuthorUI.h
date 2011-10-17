#ifndef __AddAuthorUI_H
#define __AddAuthorUI_H

#include "ui_AddAuthorUI.h"
#include "ButtonEditUI.h"

class AddAuthorUI : public ButtonEditUI, private Ui::AddAuthorDialog
{
  Q_OBJECT
public:
  AddAuthorUI(QWidget* parent);

public slots:
  void exec();
  void accept();
};

#endif

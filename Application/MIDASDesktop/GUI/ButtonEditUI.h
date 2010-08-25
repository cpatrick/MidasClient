#ifndef __ButtonEditUI_H
#define __ButtonEditUI_H

#include <QString>
#include <QDialog>

#include "MIDASDesktopUI.h"

class ButtonEditUI : public QDialog
{
Q_OBJECT
public:
  ButtonEditUI(MIDASDesktopUI* parent) : QDialog(parent) {}
signals:
  void text(const QString& text);
};

#endif

#ifndef __ButtonEditUI_H
#define __ButtonEditUI_H

#include <QString>
#include <QDialog>

class ButtonEditUI : public QDialog
{
  Q_OBJECT
public:
  ButtonEditUI(QWidget* parent) : QDialog(parent)
  {
  }
signals:
  void text(const QString& text);

};

#endif

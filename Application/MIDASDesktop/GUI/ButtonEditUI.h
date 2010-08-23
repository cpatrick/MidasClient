#ifndef __ButtonEditUI_H
#define __ButtonEditUI_H

#include <QString>
#include <QDialog>

class ButtonEditUI : public QDialog
{
Q_OBJECT

signals:
  void text(const QString& text);
};

#endif
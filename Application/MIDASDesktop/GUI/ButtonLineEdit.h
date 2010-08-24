#ifndef __ButtonLineEdit_H
#define __ButtonLineEdit_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

#include "MidasResourceDescTable.h"

class MidasItemTreeItem;
class ButtonEditUI;

class ButtonLineEdit : public QWidget
{
  Q_OBJECT

public:
  ButtonLineEdit(MidasItemTreeItem* item, MIDASFields field,
    ButtonEditUI* handler,
    QWidget* parent = 0, std::string text = "Add");
  ~ButtonLineEdit();

  QString getData();
  void setData(const QString& value);

public slots:
  void appendText(const QString& text);

protected:
  QLineEdit* m_TextEdit;
  QPushButton* m_AddButton;

  MIDASFields m_Field;
  MidasItemTreeItem* m_Item;
};

#endif
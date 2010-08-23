#include "ButtonLineEdit.h"
#include "ButtonEditUI.h"
#include "MidasItemTreeItem.h"

#include <QBoxLayout>

ButtonLineEdit::ButtonLineEdit(MidasItemTreeItem* item,
                               MIDASFields field,
                               ButtonEditUI* handler,
                               QWidget* parent, std::string text)
  : QWidget(parent), m_Field(field), m_Item(item)
{
  m_TextEdit = new QLineEdit();
  m_TextEdit->setReadOnly(true);

  m_AddButton = new QPushButton();
  m_AddButton->setText(text.c_str());

  QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight, parent);
  layout->setSpacing(0);
  layout->setMargin(0);
  layout->addWidget(m_TextEdit);
  layout->addWidget(m_AddButton);
  
  setLayout(layout);

  connect(m_AddButton, SIGNAL( released() ),
    handler, SLOT( exec() ) );
  connect(handler, SIGNAL( text(const QString&) ),
    this, SLOT( appendText(const QString&) ) );
  connect(m_TextEdit, SIGNAL(textChanged(const QString&) ),
    this, SLOT( saveItem(const QString&) ) );
}

ButtonLineEdit::~ButtonLineEdit()
{
  delete this->layout();
  delete m_TextEdit;
  delete m_AddButton;
}

void ButtonLineEdit::appendText(const QString& text)
{
  QString value = m_TextEdit->text();
  value += " / " + text;

  m_TextEdit->setText(value);
}

void ButtonLineEdit::saveItem(const QString& text)
{
  switch(m_Field)
    {
    case ITEM_AUTHORS:
      
      break;
    case ITEM_KEYWORDS:
      break;
    }
}

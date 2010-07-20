#include "MidasResourceDescTable.h"

/** Constructory */
QTableWidgetDescriptionItem::QTableWidgetDescriptionItem(const char* text, 
                                                         QTableWidgetDescriptionItem::Options options)
{
  if ( options & Tooltip )
    {
    this->setToolTip( text ); 
    }
  this->setText( text );
  if ( options & Bold )
    {
    QFont boldfont;
    boldfont.setBold(true);
    this->setFont(boldfont); 
    }
  this->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
  if ( !(options & Editable) )
    {
    this->setFlags(~Qt::ItemFlags(Qt::ItemIsEditable));
    }
}

void MidasResourceDescTable::contextMenuEvent ( QContextMenuEvent * e )
{
  emit midasTableWidgetContextMenu( e );
}

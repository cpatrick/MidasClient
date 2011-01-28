#include "MidasResourceDescTable.h"
#include "midasUtils.h"

/** Constructory */
QTableWidgetDescriptionItem::QTableWidgetDescriptionItem(const char* text, 
  QTableWidgetDescriptionItem::Options options)
{
  /*if ( options & Tooltip )
    {
    this->setToolTip( text ); 
    }*/
  this->setText( text );
  if ( options & Bold )
    {
    QFont boldfont;
    boldfont.setBold(true);
    this->setFont(boldfont); 
    }
  if ( !(options & Editable) )
    {
    this->setFlags(~Qt::ItemFlags(Qt::ItemIsEditable));
    }
  /*if ( options & AlignLeft )
    {
    this->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    }
  else
    {
    this->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }*/
}

void MidasResourceDescTable::contextMenuEvent ( QContextMenuEvent * e )
{
  emit midasTableWidgetContextMenu( e );
}

#include "MidasSearchResultList.h"

#include "MidasItemTreeItem.h"
#include "mdoObject.h"
#include "mdoBitstream.h"
#include <QPixmap>

QListWidgetItemMidasItem::QListWidgetItemMidasItem(QListWidget * parent, mdo::Object* object):
  QListWidgetItem(parent, QListWidgetItem::UserType), m_Object(object)
{
  std::string statusTip = object->GetTypeName() + " " + object->GetName();
  this->setText(object->GetName().c_str());
  this->setStatusTip(statusTip.c_str());
  this->setToolTip(statusTip.c_str());
  
  mdo::Bitstream* bs = NULL;
  if((bs = dynamic_cast<mdo::Bitstream*>(object)) != NULL)
    {
    this->setIcon(QPixmap(":icons/gpl_document.png"));
    }
  else
    {
    this->setIcon(QPixmap(":icons/gpl_folder.png"));
    }
}

MidasSearchResultList::MidasSearchResultList( QWidget * parent ):QListWidget(parent)
{
  connect(this, SIGNAL( itemClicked( QListWidgetItem*) ), this, SLOT( listItemClicked ( QListWidgetItem *) ) );
}

void MidasSearchResultList::contextMenuEvent ( QContextMenuEvent * e )
{
  emit midasListWidgetContextMenu(e);
}

void MidasSearchResultList::listItemClicked ( QListWidgetItem * item )
{
  QListWidgetItemMidasItem * midasItem = dynamic_cast<QListWidgetItemMidasItem*>( item );
  emit midasListWidgetItemClicked ( midasItem );
}

void MidasSearchResultList::clear()
{
  for(int i = 0; i < count(); i++)
    {
    QListWidgetItemMidasItem* listItem = reinterpret_cast<QListWidgetItemMidasItem*>(item(i));
    delete listItem->getObject();
    }
  QListWidget::clear();
}

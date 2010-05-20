#ifndef __MidasSearchResultList_H
#define __MidasSearchResultList_H

#include <QListWidget>

namespace mdo {
  class Object;
}

// ------------- QListWidgetItemMidasItem -------------
class QListWidgetItemMidasItem : public QListWidgetItem
{
public:

  QListWidgetItemMidasItem(QListWidget * parent, mdo::Object* data); 
  ~QListWidgetItemMidasItem(){}

  mdo::Object* getObject() { return m_Object; }

private:
  mdo::Object* m_Object;
}; 
// ------------- QListWidgetItemMidasItem -------------

// ------------- MidasSearchResultList -------------
class MidasSearchResultList : public QListWidget
{
  Q_OBJECT
public:
  MidasSearchResultList(QWidget * parent);
  ~MidasSearchResultList(){}

signals:
  void midasListWidgetContextMenu( QContextMenuEvent * e );
  void midasListWidgetItemClicked ( QListWidgetItemMidasItem * item );

protected:
  void contextMenuEvent ( QContextMenuEvent * e );

public slots:
  void listItemClicked ( QListWidgetItem * item );
  void clear();
}; 
// ------------- MidasSearchResultList -------------

#endif //__MidasSearchResultList_H
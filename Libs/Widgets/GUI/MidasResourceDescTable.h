#ifndef __MidasResourceDescTable_H
#define __MidasResourceDescTable_H

#include <QTableWidget>
#include <QFlags>

enum MIDASFields
  {
  COMMUNITY_NAME,
  COMMUNITY_DESCRIPTION,
  COMMUNITY_INTRODUCTORY,
  COMMUNITY_COPYRIGHT,
  COMMUNITY_LINKS,
  COMMUNITY_SIZE,
  COLLECTION_NAME,
  COLLECTION_DESCRIPTION,
  COLLECTION_LINKS,
  COLLECTION_COPYRIGHT,
  COLLECTION_INTRODUCTORY,
  COLLECTION_SIZE,
  ITEM_TITLE,
  ITEM_ABSTRACT,
  ITEM_AUTHORS,
  ITEM_KEYWORDS,
  ITEM_DESCRIPTION,
  ITEM_SIZE,
  BITSTREAM_NAME,
  BITSTREAM_SIZE,
  BITSTREAM_PATH,

  FOLDER3_NAME,
  FOLDER3_DESCRIPTION,
  ITEM3_NAME,
  ITEM3_DESCRIPTION,
  BITSTREAM3_NAME,
  BITSTREAM3_CHECKSUM,
  BITSTREAM3_SIZE,
  BITSTREAM3_PATH
  };

class QContextMenuEvent;
namespace mdo
{
class Community;
class Collection;
class Item;
class Bitstream;
}

namespace m3do
{
class Folder;
class Item;
class Bitstream;
}

class QTableWidgetDescriptionItem : public QTableWidgetItem
{
public:
  enum Option
    {
    NoOptions = 0x0,
    Bold = 0x1,
    Tooltip = 0x2,
    Editable = 0x4,
    AlignLeft = 0x8
    };
  Q_DECLARE_FLAGS(Options, Option) QTableWidgetDescriptionItem(const char* text,
                                                               QTableWidgetDescriptionItem::Options options = Bold);
  const static unsigned int rowHeight = 20;
private:

};
Q_DECLARE_OPERATORS_FOR_FLAGS( QTableWidgetDescriptionItem::Options )

class QTableWidgetMidasCommunityDescItem : public QTableWidgetDescriptionItem
{
public:
  QTableWidgetMidasCommunityDescItem(mdo::Community * modeldata, const char* text, MIDASFields field,
                                     QTableWidgetDescriptionItem::Options options = Tooltip | Editable) :
    QTableWidgetDescriptionItem(text, options), modeldata(modeldata)
  {
    m_Field = field;
  }

  mdo::Community * getModelData()
  {
    return this->modeldata;
  }

  MIDASFields getField()
  {
    return this->m_Field;
  }

private:
  mdo::Community * modeldata;
  MIDASFields      m_Field;
};

class QTableWidgetMidasCollectionDescItem : public QTableWidgetDescriptionItem
{
public:
  QTableWidgetMidasCollectionDescItem(mdo::Collection * modeldata, const char* text, MIDASFields field,
                                      QTableWidgetDescriptionItem::Options options = Tooltip | Editable) :
    QTableWidgetDescriptionItem(text, options), modeldata(modeldata)
  {
    m_Field = field;
  }

  mdo::Collection * getModelData()
  {
    return this->modeldata;
  }

  MIDASFields getField()
  {
    return this->m_Field;
  }

private:
  mdo::Collection * modeldata;
  MIDASFields       m_Field;
};

class QTableWidgetMidasItemDescItem : public QTableWidgetDescriptionItem
{
public:
  QTableWidgetMidasItemDescItem(mdo::Item * modeldata, const char* text, MIDASFields field,
                                QTableWidgetDescriptionItem::Options options = Tooltip | Editable) :
    QTableWidgetDescriptionItem(text, options), modeldata(modeldata)
  {
    m_Field = field;
  }

  mdo::Item * getModelData()
  {
    return this->modeldata;
  }

  MIDASFields getField()
  {
    return this->m_Field;
  }

private:
  mdo::Item * modeldata;
  MIDASFields m_Field;
};

class QTableWidgetMidasBitstreamDescItem : public QTableWidgetDescriptionItem
{
public:
  QTableWidgetMidasBitstreamDescItem(mdo::Bitstream * modeldata, const char* text, MIDASFields field,
                                     QTableWidgetDescriptionItem::Options options = Tooltip | Editable) :
    QTableWidgetDescriptionItem(text, options), modeldata(modeldata)
  {
    m_Field = field;
  }

  mdo::Bitstream * getModelData()
  {
    return this->modeldata;
  }

  MIDASFields getField()
  {
    return this->m_Field;
  }

private:
  mdo::Bitstream * modeldata;
  MIDASFields      m_Field;
};

class QTableWidgetMidas3FolderDescItem : public QTableWidgetDescriptionItem
{
public:
  QTableWidgetMidas3FolderDescItem(m3do::Folder* modeldata, const char* text, MIDASFields field,
                                   QTableWidgetDescriptionItem::Options options = Tooltip | Editable) :
    QTableWidgetDescriptionItem(text, options), modeldata(modeldata)
  {
    m_Field = field;
  }

  m3do::Folder * getModelData()
  {
    return this->modeldata;
  }

  MIDASFields getField()
  {
    return this->m_Field;
  }

private:
  m3do::Folder* modeldata;
  MIDASFields   m_Field;
};

class QTableWidgetMidas3ItemDescItem : public QTableWidgetDescriptionItem
{
public:
  QTableWidgetMidas3ItemDescItem(m3do::Item* modeldata, const char* text, MIDASFields field,
                                 QTableWidgetDescriptionItem::Options options = Tooltip | Editable) :
    QTableWidgetDescriptionItem(text, options), modeldata(modeldata)
  {
    m_Field = field;
  }

  m3do::Item * getModelData()
  {
    return this->modeldata;
  }

  MIDASFields getField()
  {
    return this->m_Field;
  }

private:
  m3do::Item* modeldata;
  MIDASFields m_Field;
};

class QTableWidgetMidas3BitstreamDescItem : public QTableWidgetDescriptionItem
{
public:
  QTableWidgetMidas3BitstreamDescItem(m3do::Bitstream* modeldata, const char* text, MIDASFields field,
                                      QTableWidgetDescriptionItem::Options options = Tooltip | Editable) :
    QTableWidgetDescriptionItem(text, options), modeldata(modeldata)
  {
    m_Field = field;
  }

  m3do::Bitstream * getModelData()
  {
    return this->modeldata;
  }

  MIDASFields getField()
  {
    return this->m_Field;
  }

private:
  m3do::Bitstream* modeldata;
  MIDASFields      m_Field;
};

class MidasResourceDescTable : public QTableWidget
{
  Q_OBJECT
public:
  MidasResourceDescTable(QWidget * parent) : QTableWidget(parent)
  {
  }
  ~MidasResourceDescTable()
  {
  }
signals:
  void midasTableWidgetContextMenu( QContextMenuEvent * e );

protected:
  void contextMenuEvent( QContextMenuEvent * e );

};

#endif // __MidasResourceDescTable_H

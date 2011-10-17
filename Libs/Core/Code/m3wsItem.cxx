/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "m3wsItem.h"
#include "m3doItem.h"
#include "m3doFolder.h"
#include "m3wsFolder.h"
#include "m3doBitstream.h"
#include "mwsRestResponseParser.h"

#include <QScriptEngine>
#include <QScriptValueIterator>

namespace m3ws
{

/** Custom Response parser for midas.item.get */
class ItemResponseParser : public mws::RestResponseParser
{
public:
  ItemResponseParser()
  {
    m_Item = NULL;
  }

  ~ItemResponseParser()
  {
  }

  virtual bool Parse(const QString& response)
  {
    if( !RestResponseParser::Parse(response) )
      {
      return false;
      }

    QScriptEngine engine;
    QScriptValue  revisions = engine.evaluate(response).property("data").property("revisions");
    if( revisions.isArray() )
      {
      QScriptValueIterator rev(revisions);
      while( rev.hasNext() )
        {
        rev.next();
        QScriptValue bitstreams = rev.value().property("bitstreams");
        if( bitstreams.isArray() )
          {
          QScriptValueIterator bitstream(bitstreams);
          while( bitstream.hasNext() )
            {
            bitstream.next();
            if( bitstream.value().property("bitstream_id").toInt32() <= 0 )
              {
              break;
              }
            m3do::Bitstream* b = new m3do::Bitstream();
            b->SetId(bitstream.value().property("bitstream_id").toInt32() );
            b->SetName(bitstream.value().property("name").toString().toStdString().c_str() );
            b->SetSize(bitstream.value().property("sizebytes").toString().toStdString() );
            b->SetChecksum(bitstream.value().property("checksum").toString().toStdString() );
            m_Item->AddBitstream(b);
            }
          }
        }
      }
    return true;
  }

  /** Set the item object */
  void SetItem(m3do::Item* item)
  {
    m_Item = item;
  }

protected:
  m3do::Item* m_Item;
};
// ------------------------------------------------------------------

/** Constructor */
Item::Item()
{
  m_Item = NULL;
}

/** Destructor */
Item::~Item()
{
}

// Set the object
void Item::SetObject(mdo::Object* object)
{
  m_Item = static_cast<m3do::Item *>(object);
}

/** Fetch */
bool Item::Fetch()
{
  // this get method also fetches the subtree, so the behavior is the same
  return this->FetchTree();
}

/** Fetch the children of a folder
  * If the id of the current folder is set to 0,
  * returns the top level communities and user folders
  */
bool Item::FetchTree()
{
  if( !m_Item )
    {
    std::cerr << "Item::Fetch() : Item not set" << std::endl;
    return false;
    }

  if( m_Item->IsFetched() )
    {
    return true;
    }

  if( !m_Item->GetId() )
    {
    std::cerr << "Item::Fetch() : Item id not set" << std::endl;
    return false;
    }

  ItemResponseParser parser;
  parser.SetItem(m_Item);
  m_Item->Clear();
  parser.AddTag("name", m_Item->GetName() );
  parser.AddTag("description", m_Item->GetDescription() );
  parser.AddTag("uuid", m_Item->GetUuid() );
  parser.AddTag("folder_id", m_Item->GetParentStr() );
  // parser.AddTag("hasAgreement", m_Folder->RefAgreement());
  // parser.AddTag("size", m_Folder->GetSize());

  std::stringstream url;
  url << "midas.item.get&head&id=" << m_Item->GetId();
  if( !mws::WebAPI::Instance()->Execute(url.str().c_str(), &parser) )
    {
    return false;
    }
  m_Item->SetFetched(true);
  return true;
}

bool Item::FetchParent()
{
  int id = m_Item->GetParentId();

  if( id )
    {
    m3do::Folder* parent = new m3do::Folder;
    m_Item->SetParentFolder(parent);
    parent->SetId(id);

    m3ws::Folder remote;
    remote.SetObject(parent);
    return remote.Fetch();
    }
  return true;
}

bool Item::Delete()
{
  if( !m_Item )
    {
    std::cerr << "Item::Delete() : Item not set" << std::endl;
    return false;
    }

  if( !m_Item->GetId() )
    {
    std::cerr << "Item::Delete() : Item id not set" << std::endl;
    return false;
    }

  std::stringstream url;
  url << "midas.item.delete&id=" << m_Item->GetId();
  if( !mws::WebAPI::Instance()->Execute(url.str().c_str() ) )
    {
    return false;
    }
  return true;
}

bool Item::Commit()
{
  std::stringstream postData;

  postData << "uuid=" << m_Item->GetUuid()
           << "&parentid=" << m_Item->GetParentId()
           << "&name=" << m_Item->GetName()
           << "&description=" << m_Item->GetDescription();

  return mws::WebAPI::Instance()->Execute("midas.item.create", NULL,
                                          postData.str().c_str() );
}

} // end namespace

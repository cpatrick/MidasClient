/******************************************************************************
 * Copyright 2011 Kitware Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#include "m3wsFolder.h"
#include "m3doFolder.h"
#include "m3doCommunity.h"
#include "m3doItem.h"
#include "mwsRestResponseParser.h"

#include <QScriptEngine>
#include <QScriptValueIterator>

namespace m3ws
{

/** Custom Response parser for midas.community.list */
class CommunityListResponseParser : public mws::RestResponseParser
{
public:

  CommunityListResponseParser()
  {
    m_Folder = NULL;
  }

  virtual ~CommunityListResponseParser()
  {
  }

  virtual bool Parse(const QString& response)
  {
    if( !RestResponseParser::Parse(response) )
      {
      return false;
      }

    QScriptEngine engine;
    QScriptValue  data = engine.evaluate(response).property("data");
    if( data.isArray() )
      {
      QScriptValueIterator comms(data);
      while( comms.hasNext() )
        {
        comms.next();
        if( comms.value().property("community_id").toInt32() <= 0 )
          {
          break;
          }
        m3do::Community* comm = new m3do::Community();
        comm->SetId(comms.value().property("community_id").toInt32() );
        comm->SetUuid(comms.value().property("uuid").toString().toStdString().c_str() );
        comm->SetName(comms.value().property("name").toString().toStdString().c_str() );
        comm->SetDescription(comms.value().property("description").toString().toStdString().c_str() );
        comm->SetFolderId(comms.value().property("folder_id").toInt32() );
        m_Folder->AddFolder(comm);
        }
      }
    return true;
  }

  /** Set the Folder object */
  void SetFolder(m3do::Folder* folder)
  {
    m_Folder = folder;
  }

protected:
  m3do::Folder* m_Folder;
};

/** Custom Response parser for midas.user.folders */
class UserFoldersResponseParser : public CommunityListResponseParser
{
  virtual bool Parse(const QString& response)
  {
    if( !RestResponseParser::Parse(response) )
      {
      return false;
      }

    QScriptEngine engine;
    QScriptValue  data = engine.evaluate(response).property("data");
    if( data.isArray() )
      {
      QScriptValueIterator comms(data);
      while( comms.hasNext() )
        {
        comms.next();
        if( comms.value().property("folder_id").toInt32() <= 0 )
          {
          break;
          }
        m3do::Folder* folder = new m3do::Folder();
        folder->SetId(comms.value().property("folder_id").toInt32() );
        folder->SetUuid(comms.value().property("uuid").toString().toStdString().c_str() );
        folder->SetName(comms.value().property("name").toString().toStdString().c_str() );
        folder->SetDescription(comms.value().property("description").toString().toStdString().c_str() );
        m_Folder->AddFolder(folder);
        }
      }
    return true;
  }

};

/** Custom Response parser for midas.folder.children and
  midas.community.children */
class FolderResponseParser : public mws::RestResponseParser
{
public:
  FolderResponseParser()
  {
    m_Folder = NULL;
  }

  ~FolderResponseParser()
  {
  }

  virtual bool Parse(const QString& response)
  {
    if( !RestResponseParser::Parse(response) )
      {
      return false;
      }

    QScriptEngine engine;
    QScriptValue  data = engine.evaluate(response).property("data");
    QScriptValue  folders = data.property("folders");
    if( folders.isArray() )
      {
      QScriptValueIterator f(folders);
      while( f.hasNext() )
        {
        f.next();
        if( f.value().property("folder_id").toInt32() <= 0 )
          {
          break;
          }
        m3do::Folder* folder = new m3do::Folder();
        folder->SetId(f.value().property("folder_id").toInt32() );
        folder->SetUuid(f.value().property("uuid").toString().toStdString().c_str() );
        folder->SetName(f.value().property("name").toString().toStdString().c_str() );
        folder->SetDescription(f.value().property("description").toString().toStdString().c_str() );
        m_Folder->AddFolder(folder);
        }
      }

    QScriptValue items = data.property("items");
    if( items.isArray() )
      {
      QScriptValueIterator i(items);
      while( i.hasNext() )
        {
        i.next();
        if( i.value().property("item_id").toInt32() <= 0 )
          {
          break;
          }
        m3do::Item* item = new m3do::Item();
        item->SetId(i.value().property("item_id").toInt32() );
        item->SetUuid(i.value().property("uuid").toString().toStdString().c_str() );
        item->SetName(i.value().property("name").toString().toStdString().c_str() );
        item->SetDescription(i.value().property("description").toString().toStdString().c_str() );
        m_Folder->AddItem(item);
        }
      }
    return true;
  }

  /** Set the Folder object */
  void SetFolder(m3do::Folder* folder)
  {
    m_Folder = folder;
  }

protected:
  m3do::Folder* m_Folder;
};
// ------------------------------------------------------------------

/** Constructor */
Folder::Folder()
{
  m_Folder = NULL;
}

/** Destructor */
Folder::~Folder()
{
}

// Add the object
void Folder::SetObject(mdo::Object* object)
{
  m_Folder = static_cast<m3do::Folder *>(object);
}

/** Fetch */
bool Folder::Fetch()
{
  if( !m_Folder )
    {
    std::cerr << "Folder::Fetch() : Folder not set" << std::endl;
    return false;
    }

  if( m_Folder->IsFetched() )
    {
    return true;
    }

  if( !m_Folder->GetId() )
    {
    std::cerr << "Folder::Fetch() : Folder id not set" << std::endl;
    return false;
    }

  mws::RestResponseParser parser;
  m_Folder->Clear();
  parser.AddTag("name", m_Folder->GetName() );
  parser.AddTag("description", m_Folder->GetDescription() );
  parser.AddTag("uuid", m_Folder->GetUuid() );
  // parser.AddTag("hasAgreement", m_Folder->RefAgreement());
  // parser.AddTag("size", m_Folder->GetSize());

  std::stringstream url;
  if( m_Folder->GetResourceType() == midas3ResourceType::COMMUNITY )
    {
    url << "midas.community.get&id=" << m_Folder->GetId();
    if( !mws::WebAPI::Instance()->Execute(url.str().c_str(), &parser) )
      {
      return false;
      }
    }
  else
    {
    parser.AddTag("parent_id", m_Folder->GetParentStr() );
    url << "midas.folder.get&id=" << m_Folder->GetId();
    if( !mws::WebAPI::Instance()->Execute(url.str().c_str(), &parser) )
      {
      return false;
      }
    }

  m_Folder->SetFetched(true);
  return true;
}

/** Fetch the children of a folder
  * If the id of the current folder is set to 0,
  * returns the top level communities and user folders
  */
bool Folder::FetchTree()
{
  if( m_Folder->GetId() == 0 )
    {
    CommunityListResponseParser commParser;
    commParser.SetFolder(m_Folder);
    if( !mws::WebAPI::Instance()->Execute("midas.community.list", &commParser) )
      {
      return false;
      }

    UserFoldersResponseParser folderParser;
    folderParser.SetFolder(m_Folder);
    if( !mws::WebAPI::Instance()->Execute("midas.user.folders", &folderParser) )
      {
      return false;
      }
    }
  else
    {
    FolderResponseParser parser;
    parser.SetFolder(m_Folder);
    std::stringstream url;
    url << "midas." << m_Folder->GetTypeName() << ".children&id="
        << m_Folder->GetId();
    if( !mws::WebAPI::Instance()->Execute(url.str().c_str(), &parser) )
      {
      return false;
      }
    }
  return true;
}

bool Folder::FetchParent()
{
  int id = m_Folder->GetParentId();

  if( id )
    {
    m3do::Folder* parent = new m3do::Folder;
    m_Folder->SetParentFolder(parent);
    parent->SetId(m_Folder->GetParentId() );

    m3ws::Folder remote;
    remote.SetObject(parent);
    return remote.Fetch();
    }
  return true;
}

bool Folder::Delete()
{
  if( !m_Folder )
    {
    std::cerr << "Folder::Delete() : Folder not set" << std::endl;
    return false;
    }

  if( !m_Folder->GetId() )
    {
    std::cerr << "Folder::Delete() : Folder id not set" << std::endl;
    return false;
    }

  std::stringstream url;
  url << "midas.folder.delete&id=" << m_Folder->GetId();
  if( !mws::WebAPI::Instance()->Execute(url.str().c_str() ) )
    {
    return false;
    }
  return true;
}

bool Folder::Commit()
{
  std::stringstream postData;

  postData << "uuid=" << m_Folder->GetUuid()
           << "&name=" << m_Folder->GetName()
           << "&description=" << m_Folder->GetDescription();

  if( m_Folder->GetResourceType() == midas3ResourceType::COMMUNITY )
    {
    return mws::WebAPI::Instance()->Execute("midas.community.create", NULL,
                                            postData.str().c_str() );
    }
  else
    {
    postData << "&parentid=" << m_Folder->GetParentId();
    return mws::WebAPI::Instance()->Execute("midas.folder.create", NULL,
                                            postData.str().c_str() );
    }
}

} // end namespace

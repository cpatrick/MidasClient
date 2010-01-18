/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mdsItem.h"
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <time.h>

#include "mdoItem.h"
#include "mdoBitstream.h"
#include "kwsys/SystemTools.hxx"

namespace mds{

/** Constructor */
Item::Item()
{
  m_Item = 0;
}
  
/** Destructor */
Item::~Item()
{
}
  
/** Fecth */
bool Item::Fetch()
{
  if(!m_Item)
    {
    std::cerr << "Item::Fecth : Item not set" << std::endl;
    return false;
    }
    
  if(m_Item->GetId() == 0)
    {
    std::cerr << "Item::Fetch : ItemId not set" << std::endl;
    return false;
    }    
  return true;
}

/** Commit */
bool Item::Commit()
{
  return true;
}

/** Create a unique filename in the assetstore as well as the directories */
std::string Item::GenerateAssetstoreFilename(const char* assetstore)
{

  srand ( (unsigned int)time(NULL) );
  // Generate a 40 characters long string
  std::stringstream randomchars;
  for(unsigned int i=0;i<40;i++)
    {
    randomchars << rand() % 9;
    }
  
  std::string filename = randomchars.str();
  std::string directory = assetstore;
  directory  += "/"; 
  directory += filename.substr(0,2);
  directory  += "/"; 
  directory += filename.substr(2,2);
  directory  += "/";
  directory += filename.substr(4,2);
  directory  += "/";
  directory += filename;
  
  if(kwsys::SystemTools::FileExists(directory.c_str()))
    {
    return this->GenerateAssetstoreFilename(assetstore);
    }
 
  // Create the directories if needed
  directory = assetstore;
  directory  += "/"; 
  directory += filename.substr(0,2);
  if(!kwsys::SystemTools::FileExists(directory.c_str()))
    {
    kwsys::SystemTools::MakeDirectory(directory.c_str());
    }
  directory  += "/"; 
  directory += filename.substr(2,2);
  if(!kwsys::SystemTools::FileExists(directory.c_str()))
    {
    kwsys::SystemTools::MakeDirectory(directory.c_str());
    }
  directory  += "/"; 
  directory += filename.substr(4,2);
  if(!kwsys::SystemTools::FileExists(directory.c_str()))
    {
    kwsys::SystemTools::MakeDirectory(directory.c_str());
    } 
  directory  += "/";
  directory += filename;

  return directory;
}
  
// Upload a bitstream
const char* Item::UploadBitstream(const char* filename,const char* uuid)
{
  std::cout << "Uploading bitream to the database" << std::endl;
  
  // We need to make sure the bitstream is not already there.
  
  // Generate a three levels structure (like MIDAS)
  std::string assetstorename = this->GenerateAssetstoreFilename("C:/Julien/MidasDesktop/assetstore");
  
  if(kwsys::SystemTools::CopyFileIfDifferent(filename,assetstorename.c_str()))
    {
    // Insert in the database
    std::stringstream query;
    query << "INSERT INTO bitstream (name,internal_id,uuid) VALUES ('";
    query << kwsys::SystemTools::GetFilenameName(filename);
    query << "','";
    query << kwsys::SystemTools::GetFilenameName(assetstorename);
    query << "','";
    query << uuid;
    query << "')";
    m_Database->ExecuteQuery(query.str().c_str());
    }

  return uuid;
}
 
// Download a bitstream
std::string Item::DownloadBitstream(const char* uuid,const char* filename)
{
  std::stringstream query;
  query << "SELECT b.internal_id,b.name,bh.handle,bh.priority FROM bitstream AS b ";
  query << "LEFT OUTER JOIN bitstream2handle AS bh ON b.bitstream_id=bh.bitstream_id WHERE b.uuid='";
  query << uuid;
  query << "' ORDER BY bh.priority DESC";

  if(!m_Database->ExecuteQuery(query.str().c_str()))
    {
    std::cerr << m_Database->GetErrorMessage() << std::endl;
    return "";
    }
  
  bool firstrow = true;
  std::string internalHandle = "";
  
  while(m_Database->GetNextRow())
    {
    if(firstrow)
      {
      std::string internalId = m_Database->GetValueAsString(0);
      internalHandle = "assetstore/";
      internalHandle += internalId.substr(0,2);
      internalHandle += "/";
      internalHandle += internalId.substr(2,2);
      internalHandle += "/";
      internalHandle += internalId.substr(4,2);
      internalHandle += "/";
      internalHandle += internalId;
      firstrow = false;
      }
      
    // If the priority is more than 0 that mean we need to open it first
    int priority = m_Database->GetValueAsInt(3);
    if(priority == 0)
      {
      return internalHandle;
      }
    else
      {
      // Check if we can read the handle
      std::string handle = m_Database->GetValueAsString(2);
      if(kwsys::SystemTools::FileExists(handle.c_str(),true))
        {
        return handle;
        }
      }
    }
  
  // If we have an internal id we do it   
  return internalHandle;
}

// Add the object
void Item::SetObject(mdo::Object* object)
{  
  m_Item = static_cast<mdo::Item*>(object);
}

} // end namespace

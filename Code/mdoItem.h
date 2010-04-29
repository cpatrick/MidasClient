/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _mdoItem_h_
#define _mdoItem_h_

#include <string>
#include <vector>

#include "mdoObject.h"
#include "mdoBitstream.h"

namespace mdo{

/** This class represent an item on the MIDAS server. 
 *  An item has many bitstreams. */
class Item : public Object
{
public:

  Item();
  ~Item();

  // Load
  bool Load();

  // Set/Get the ID of the item
  void SetId(int id) { this->m_Id = id; }
  const int GetId() {return this->m_Id; }

  // Set/Get the ID of the parent
  void SetParent(std::string id) { m_Parent = id; }
  std::string & GetParent() {return m_Parent;}
  const int GetParentId() {return atoi(m_Parent.c_str());}

  // Set/Get the uuid
  void SetUuid(const char* uuid) { m_Uuid = uuid; }
  std::string & GetUuid() {return m_Uuid;}

  // Set/Get the title of the item
  void SetTitle(const char* title) { m_Title = title; }
  std::string & GetTitle() {return m_Title;}

  // Set/Get the abstract of the item
  void SetAbstract(const char* abstract) { m_Abstract = abstract; }
  std::string & GetAbstract() {return m_Abstract;}

  // Upload a bitstream to MIDAS
  unsigned int UploadBitstream(unsigned int itemid,const char* filename);
  
  // Download a bitstream from MIDAS
  // If the bitstream can be access locally then a new filename is returned
  std::string DownloadBitstream(const char* uuid,const char* filename);
  
  // Get the list of bitstreams
  const std::vector<Bitstream*>& GetBitstreams() { return m_Bitstreams; }

  // Add a bitstream to the list
  void AddBitstream(Bitstream* bitstream) {m_Bitstreams.push_back(bitstream);}
  
  // Display all the tags and their respective names and values.
  void Print(std::ostream &os, int indent=-1);
 
protected:

  friend class ItemXMLParser;
  
  unsigned int m_Id;
  std::string  m_Parent;
  std::string  m_Title;
  std::string  m_Abstract;
  std::string  m_Uuid;
  
  std::vector<Bitstream*> m_Bitstreams;
};

} //end namespace

#endif //_mdoItem_h_

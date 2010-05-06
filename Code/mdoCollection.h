/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _mdoCollection_h_
#define _mdoCollection_h_

#include <string>
#include <vector>

#include "mdoObject.h"
#include "mdoItem.h"

namespace mdo{

/** This class represent an collection on the MIDAS server. 
 *  An collection has many bitstreams. */
class Collection : public Object
{
public:

  Collection();
  ~Collection();

  // Fill the collection given the id
  bool Load();
  void Clear();

  // Set/Get the ID of the collection
  void SetId(int id) { this->m_Id = id; }
  const int GetId() {return this->m_Id; }

  // Set/Get the ID of the parent
  void SetParent(std::string id) { m_Parent = id; }
  std::string & GetParent() {return m_Parent;}
  const int GetParentId() {return atoi(m_Parent.c_str());}

  // Set/Get the uuid
  void SetUuid(const char* uuid) { m_Uuid = uuid; }
  std::string & GetUuid() {return m_Uuid;}

  // Set/Get the name of the collection
  void SetName(const char* name) { m_Name = name; }
  std::string & GetName() {return m_Name;} 
 
  // Set/Get the description of the collection
  void SetDescription(const char* description) { m_Description = description; }
  std::string & GetDescription() {return m_Description;}

  // Set/Get introductory text
  void SetIntroductoryText(const char* text) { m_IntroductoryText = text; }
  std::string & GetIntroductoryText() {return m_IntroductoryText;}
  
  // Set/Get copyright
  void SetCopyright(const char* copyright) { m_Copyright = copyright; }
  std::string & GetCopyright() {return m_Copyright;}
  
  // Return the list of items
  const std::vector<Item*>& GetItems() { return m_Items; }

  // Display all the tags and their respective names and values.
  void Print(std::ostream &os, int indent=-1);
 
  // Add an item
  void AddItem(Item* item) {m_Items.push_back(item);}
  
protected:

  friend class CollectionXMLParser;
  
  unsigned int m_Id;
  std::string  m_Parent;
  std::string  m_Name;
  std::string  m_Description;
  std::string  m_Copyright;
  std::string  m_IntroductoryText;
  std::string  m_Uuid;
  
  std::vector<Item*> m_Items;
};

} //end namespace

#endif //_mdoCollection_h_

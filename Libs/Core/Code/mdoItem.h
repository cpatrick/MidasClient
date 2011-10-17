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

namespace mdo
{

class Collection;

/** This class represent an item on the MIDAS server.
 *  An item has many bitstreams. */
class Item : public Object
{
public:

  Item();
  ~Item();

  // Load
  bool Load();

  void Clear();

  // Set/Get the title of the item
  void SetTitle(const char* title);
  std::string & GetTitle();

  std::string & GetName();

  // Set/Get the description of the item
  void SetDescription(const char* desc);
  std::string & GetDescription();

  // Set/Get the abstract of the item
  void SetAbstract(const char* abstract);
  std::string & GetAbstract();

  void SetAuthors(std::vector<std::string> authors);
  void AddAuthor(std::string author);
  std::vector<std::string> & GetAuthors();
  std::string GetAuthorsString();

  void SetKeywords(std::vector<std::string> keywords);
  void AddKeyword(std::string keyword);
  std::vector<std::string> & GetKeywords();
  std::string GetKeywordsString();

  // Get the list of bitstreams
  const std::vector<Bitstream *> & GetBitstreams();

  // Add a bitstream to the list
  void AddBitstream(Bitstream* bitstream);

  void SetBitstreamCount(unsigned int count);
  unsigned int GetBitstreamCount();

  std::string GetTypeName();
  int GetResourceType();

  Collection * GetParentCollection();
  void SetParentCollection(Collection* coll);

  bool SetValue(std::string key, std::string value, bool append = false);

protected:

  friend class ItemXMLParser;

  std::string              m_Title;
  std::string              m_Abstract;
  std::string              m_Description;
  std::vector<std::string> m_Authors;
  std::vector<std::string> m_Keywords;
  std::vector<Bitstream *> m_Bitstreams;
  unsigned int             m_BitstreamCount;

  Collection* m_ParentCollection;
};

} // end namespace

#endif // _mdoItem_h_

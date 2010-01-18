/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mwsBitstream.h"
#include <sstream>
#include <iostream>
#include "mwsRestXMLParser.h"

namespace mws{

/** Custom XML parser */
class BitstreamXMLParser : public RestXMLParser
{
public:
   
  BitstreamXMLParser()
    {
    m_Bitstream = NULL;
    };
  ~BitstreamXMLParser() {};  

  /// Callback function -- called from XML parser with start-of-element
  /// information.
  virtual void StartElement(const char * name,const char **atts)
    {
    RestXMLParser::StartElement(name,atts);
    std::cout << "StartElement: " << name << std::endl;
    }

  /// Callback function -- called from XML parser when ending tag
  /// encountered
  virtual void EndElement(const char *name)
    {
    RestXMLParser::EndElement(name);
    std::cout << "EndElement: " << name << std::endl;
    }
    
  /// Callback function -- called from XML parser with the character data
  /// for an XML element
  virtual void CharacterDataHandler(const char *inData, int inLength)
    {
     RestXMLParser::CharacterDataHandler(inData,inLength);
    //std::cout << "CharacterDataHandler: " << inData << std::endl;
    }
  
  /** Set the bitstream object */
  void SetBitstream(Bitstream* item) {m_Bitstream = item;}
  
protected:

  Bitstream* m_Bitstream;
  
};


/** Constructor */
Bitstream::Bitstream()
{
  m_Bitstream = 0;
}
  
/** Destructor */
Bitstream::~Bitstream()
{
}

/** Fetch the object */
bool Bitstream::Fetch()
{
  return true;
}

/** Commit the object */
bool Bitstream::Commit()
{
  return true;
}

// Add the object
void Bitstream::SetObject(mdo::Object* object)
{  
  m_Bitstream = reinterpret_cast<mdo::Bitstream*>(object);
}

} // end namespace
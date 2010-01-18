/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mwsDicom.h"
#include <sstream>
#include <iostream>
#include "mwsRestXMLParser.h"

#include "mdoDicom.h"

namespace mws{

/** Custom XML parser */
class DicomXMLParser : public RestXMLParser
{
public:
   
  DicomXMLParser()
    {
    m_Dicom = NULL;
    };
    
  ~DicomXMLParser() 
    {
    };  

  /// Callback function -- called from XML parser with start-of-element
  /// information.
  virtual void StartElement(const char * name,const char **atts)
    { 
    RestXMLParser::StartElement(name,atts);
    m_CurrentValue = "";
    }

  /// Callback function -- called from XML parser when ending tag
  /// encountered
  virtual void EndElement(const char *name)
    {
    if(!m_CurrentTag.compare("/rsp/itemid"))
      {
      // convert std::string to int
      unsigned int itemid;
      std::stringstream(m_CurrentValue) >> itemid;
      m_Dicom->SetItemId(itemid);
      }
    RestXMLParser::EndElement(name);
    }
    
  /// Callback function -- called from XML parser with the character data
  /// for an XML element
  virtual void CharacterDataHandler(const char *inData, int inLength)
    {
    RestXMLParser::CharacterDataHandler(inData,inLength);
    m_CurrentValue.append(inData,inLength);
    }
  
  /** Set the item object */
  void SetDicom(mdo::Dicom* dicom) {m_Dicom = dicom;}
  
protected:
  mdo::Dicom* m_Dicom;
  std::string m_CurrentValue;
};


/** Constructor */
Dicom::Dicom()
{
  m_Dicom = 0;
}
  
/** Destructor */
Dicom::~Dicom()
{
}
  
/** Fetch */
bool Dicom::Fetch()
{
  if(!m_Dicom)
    {
    std::cerr << "Dicom::Fetch : Dicom not set" << std::endl;
    return false;
    }
  
  if(m_Dicom->GetAETitle() == "")
    {
    std::cerr << "Dicom::Fetch : AETitle not set" << std::endl;
    return false;
    }

  DicomXMLParser parser;
  parser.SetDicom(m_Dicom);
  
  m_WebAPI->GetRestAPI()->SetXMLParser(&parser);
  
  std::stringstream url;
  url << "dicom.expose.getItemIdByAETitle?aetitle=" << m_Dicom->GetAETitle();

  if(!m_WebAPI->Execute(url.str().c_str()))
    {
    std::cerr << m_WebAPI->GetErrorMessage() << std::endl;
    return false;
    }

  return true;
}

/** Commit */
bool Dicom::Commit()
{
  return true;
}

// Download a bitstream
const unsigned int Dicom::GetItemId()
{
  return m_Dicom->GetItemId();
}

// Add the object
void Dicom::SetObject(mdo::Object* object)
{  
  m_Dicom = static_cast<mdo::Dicom*>(object);
}

} // end namespace

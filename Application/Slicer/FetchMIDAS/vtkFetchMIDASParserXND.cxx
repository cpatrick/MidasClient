#include "vtkObjectFactory.h"
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vtksys/SystemTools.hxx>

#include "vtkFetchMIDASParserXND.h"
#include "vtkXNDTagTable.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkFetchMIDASParserXND );
vtkCxxRevisionMacro ( vtkFetchMIDASParserXND, "");


//---------------------------------------------------------------------------
vtkFetchMIDASParserXND::vtkFetchMIDASParserXND ( )
{
  this->Parser = vtkXMLDataParser::New();
}


//---------------------------------------------------------------------------
vtkFetchMIDASParserXND::~vtkFetchMIDASParserXND ( )
{
  if ( this->Parser )
    {
    this->Parser->Delete();
    this->Parser = NULL;
    }
}



//---------------------------------------------------------------------------
void vtkFetchMIDASParserXND::PrintSelf ( ostream& os, vtkIndent indent )
{
    this->vtkObject::PrintSelf ( os, indent );
    if ( this->Parser )
      {      
      os << indent << "Parser: " << this->GetParser ( ) << "\n";
      }
    else
      {
      os << indent << "Parser: NULL\n";
      }
}



//---------------------------------------------------------------------------
int vtkFetchMIDASParserXND::ParseMetadataQueryResponse(const char *filename )
{

  
  if ( filename == NULL || !(strcmp(filename, "") ) )
    {
    vtkErrorMacro ( "vtkFetchMIDASParserXND::ParseMetadataQueryResponse: response filename for parsing is NULL.");
    return 0;
    }
  vtkDebugMacro ("--------------------Parsing metadata query response.");
  if ( this->GetFetchMIDASNode() == NULL )
    {
    vtkErrorMacro ( "vtkFetchMIDASParserXND: FetchMIDASNode is NULL.");
    return 0;
    }
  if ( this->FetchMIDASNode->GetTagTableCollection() == NULL )
    {
    vtkErrorMacro ( "vtkFetchMIDASParserXND: FetchMIDASNode's TagTableCollection is NULL.");
    return 0;
    }
  if ( this->Parser == NULL )
    {
    this->Parser = vtkXMLDataParser::New();
    }

  this->ClearMetadataInformation ( );
  this->FetchMIDASNode->SetErrorMessage ( NULL );

  //--- look at xml file to see if there's an error message
  //--- parse the error and fill the node's error message.
  //--- if no, parse response into table.
  vtkXNDTagTable *t = vtkXNDTagTable::SafeDownCast ( this->FetchMIDASNode->GetTagTableCollection()->FindTagTableByName ( "XND" ));
  if ( t != NULL )
    {
    //--- check for file.
    this->Parser->SetFileName ( filename );
    this->Parser->SetIgnoreCharacterData ( 0 );
    int retval = this->Parser->Parse();
    if ( retval == 0 )
      {
      this->FetchMIDASNode->SetErrorMessage ("Unable to parse tag query response.");
      this->FetchMIDASNode->InvokeEvent ( vtkMRMLFetchMIDASNode::RemoteIOErrorEvent );
      return 0;
      }

    //--- first check for errors
    this->NumberOfElements = 0;
    vtkXMLDataElement *root = this->Parser->GetRootElement();
    this->ErrorChecking = 1;
    this->ErrorFlag = 0;
    if ( root != NULL )
      {
      this->GetNumberOfXMLElements( root );
      if ( this->NumberOfElements > 0 )
        {
        root = this->Parser->GetRootElement();
        this->GetXMLElement ( root );
        }
      this->ErrorChecking = 0;
      if ( this->ErrorFlag )
        {
        this->FetchMIDASNode->SetErrorMessage ("The server returned an error response to the query for all known values for its tags.");
        this->FetchMIDASNode->InvokeEvent ( vtkMRMLFetchMIDASNode::RemoteIOErrorEvent );          
        this->ErrorFlag = 0;
        return 0;
        }
      }

    //--- If no need to keep tag selection state around, then
    //--- clear out mrml tag table to be refilled by this query.
    if ( t->GetRestoreSelectionState() == 0 )
      {
      t->ClearTagTable();
      }
      
    //--- Set the response type to help in pulling out XML elements properly.
    this->ResponseType = vtkFetchMIDASParser::TagAttributesQueryResponse;

    //---
    this->NumberOfElements = 0;
    root = this->Parser->GetRootElement();
    if ( root != NULL )
      {
      this->GetNumberOfXMLElements( root );
      if ( this->NumberOfElements > 0 )
        {
        //--- If response is good, parse response into table.
        root = this->Parser->GetRootElement();
        this->GetXMLElement ( root );
        }
      }
    else
      {
      // let user know the parsing didn't go well.
      this->FetchMIDASNode->SetErrorMessage ("Unable to parse tag query response. Displayed list of tags has not been updated.");
      this->FetchMIDASNode->InvokeEvent ( vtkMRMLFetchMIDASNode::RemoteIOErrorEvent );          
      return 0;
      }
    }
  return 1;

}

//---------------------------------------------------------------------------
int vtkFetchMIDASParserXND::ParseMetadataPostResponse( const char *filename )
{

  if ( filename == NULL || !(strcmp(filename, "" )) )
    {
    vtkErrorMacro ( "vtkFetchMIDASParserXND::ParseMetadataPostResponse: got null or empty response filename." );
    return 0;
    }

  std::ifstream pfile;
  std::string line;
#ifdef _WIN32
  pfile.open ( filename, ios::binary | ios::in );
#else
  pfile.open ( filename, ios::in );
#endif
  
  if ( !pfile.is_open() )
    {
    vtkErrorMacro ( "vtkFetchMIDASParserXND::ParseMetadataPostResponse: error opening file to parse." );
    return 0;
    }

  int found = 0;
  size_t pos;
  while ( !pfile.eof() )
    {
    line.clear();
    pfile >> line;
    //--- look for uri created and reserved for the resource to be posted.
    //--- XND webservices returns this uri on its own line...
    //--- get lines and search each one.
    //--- when found, set found to 1.    
    if ( ( pos = line.find ( "http://", 0) ) != std::string::npos )
      {
      if ( pos == 0 )
        {
        found = 1;
        break;
        }
      }
    }

  pfile.close();

  if ( found )
    {
    this->SetResponse(line.c_str());
    return 1;
    }
  return 0;
}


//---------------------------------------------------------------------------
int vtkFetchMIDASParserXND::ParseValuesForAttributeQueryResponse(const char *filename, const char *att )
{
    

  if ( filename == NULL || !(strcmp(filename, "" )) )
    {
    vtkErrorMacro ( "vtkFetchMIDASParserXND::ParseMetadataValuesQueryResponse: got null or empty response filename." );
    return 0;
    }
  if ( this->FetchMIDASNode == NULL )
    {
    vtkErrorMacro ( "vtkFetchMIDASParserXND::ParseMetadataValuesQueryResponse: got null FetchMIDASNode.");
    return 0;
    }
  if ( this->Parser == NULL )
    {
    this->Parser = vtkXMLDataParser::New();
    }

  this->FetchMIDASNode->SetErrorMessage ( NULL );
  this->ClearValuesForAttribute( att );

  //--- check for file.
  this->Parser->SetFileName ( filename );
  this->Parser->SetIgnoreCharacterData ( 0 );
  int retval = this->Parser->Parse();
  //--- throw an error event if parsing failed.
  if ( retval == 0 )
    {
    this->FetchMIDASNode->SetErrorMessage ("Unable to parse tag value query response.");
    this->FetchMIDASNode->InvokeEvent ( vtkMRMLFetchMIDASNode::RemoteIOErrorEvent );
    return 0;
    }

  //--- first check for errors
  this->NumberOfElements = 0;
  vtkXMLDataElement *root = this->Parser->GetRootElement();
  this->ErrorChecking = 1;
  this->ErrorFlag = 0;
  if ( root != NULL )
    {
    this->GetNumberOfXMLElements( root );
    if ( this->NumberOfElements > 0 )
      {
      root = this->Parser->GetRootElement();
      this->GetXMLElement ( root );
      }
    this->ErrorChecking = 0;
    if ( this->ErrorFlag )
      {
      this->FetchMIDASNode->SetErrorMessage ("The server returned an error response to the query for its tags.");
      this->FetchMIDASNode->InvokeEvent ( vtkMRMLFetchMIDASNode::RemoteIOErrorEvent );          
      this->ErrorFlag = 0;
      return 0;
      }
    }

  //--- Set response type for correct parsing.
  this->ResponseType = vtkFetchMIDASParser::TagValuesQueryResponse;
  //---
  this->NumberOfElements = 0;
  root = this->Parser->GetRootElement();
  if ( root != NULL )
    {
    this->GetNumberOfXMLElements( root );
    if ( this->NumberOfElements > 0 )
      {
      root = this->Parser->GetRootElement();
      this->GetXMLElement ( root );
      }
    }
  else
    {
    // let user know the parsing didn't go well.
    this->FetchMIDASNode->SetErrorMessage ("Unable to parse tag query response. Displayed list of tags has not been updated.");
    this->FetchMIDASNode->InvokeEvent ( vtkMRMLFetchMIDASNode::RemoteIOErrorEvent );          
    }

  return 1;
}


//---------------------------------------------------------------------------
int vtkFetchMIDASParserXND::ParseResourceQueryResponse(const char *filename )
{
  
    if ( this->GetFetchMIDASNode() == NULL )
    {
    vtkErrorMacro ( "vtkFetchMIDASParserXND::ParseResourceQueryResponse: FetchMIDASNode is NULL.");
    return 0;
    }
  if ( this->Parser == NULL )
    {
    this->Parser = vtkXMLDataParser::New();
    }
  if ( filename == NULL || !(strcmp(filename, "")) )
    {
    vtkErrorMacro ( "vtkFetchMIDASParserXND::ParseResourceQueryResponse: No XMLResponse filename is set." );
    return 0;
    }

  this->FetchMIDASNode->SetErrorMessage(NULL );
  if  (this->FetchMIDASNode->GetResourceDescription() != NULL) 
    {
    //--- check for file.
    this->Parser->SetFileName ( filename );
    this->Parser->SetIgnoreCharacterData ( 0 );
    int retval = this->Parser->Parse();
    if ( retval == 0 )
      {
      this->FetchMIDASNode->SetErrorMessage("Unable to parse resource query response.");
      this->FetchMIDASNode->InvokeEvent ( vtkMRMLFetchMIDASNode::RemoteIOErrorEvent );
      return 0;
      }

    //--- first check for errors
    this->NumberOfElements = 0;
    vtkXMLDataElement *root = this->Parser->GetRootElement();
    this->ErrorChecking = 1;
    this->ErrorFlag = 0;
    if ( root != NULL )
      {
      this->GetNumberOfXMLElements( root );
      if ( this->NumberOfElements > 0 )
        {
        root = this->Parser->GetRootElement();
        this->GetXMLElement ( root );
        }
      this->ErrorChecking = 0;
      if ( this->ErrorFlag )
        {
        this->FetchMIDASNode->SetErrorMessage ("The server returned an error response to the query for matching resources.");
        this->FetchMIDASNode->InvokeEvent ( vtkMRMLFetchMIDASNode::RemoteIOErrorEvent );          
        this->ErrorFlag = 0;
        return 0;
        }
      }

    //--- make sure ResponseType is set so parsing is done correctly.
    this->ResponseType = vtkFetchMIDASParser::ResourceQueryResponse;

    this->NumberOfElements = 0;
    root = this->Parser->GetRootElement();
    if ( root != NULL )
      {
      this->GetNumberOfXMLElements( root );
      if ( this->NumberOfElements > 0 )
        {
        //--- If response is good, parse response into table.
        root = this->Parser->GetRootElement();
        this->GetXMLElement ( root );
        }
      }
    else
      {
      // let user know the parsing didn't go well.
      this->FetchMIDASNode->SetErrorMessage("Unable to parse tag resource response.");
      this->FetchMIDASNode->InvokeEvent ( vtkMRMLFetchMIDASNode::RemoteIOErrorEvent );          
      return 0;
      }
    
    this->FetchMIDASNode->InvokeEvent ( vtkMRMLFetchMIDASNode::ResourceResponseReadyEvent );
    }
  return 1;
}



//---------------------------------------------------------------------------
int vtkFetchMIDASParserXND::ParseResourcePostResponse(const char *filename )
{
  return 0;
}


//---------------------------------------------------------------------------
int vtkFetchMIDASParserXND::ParseResourceDownloadResponse(const char *filename )
{
  return 0;
}

//---------------------------------------------------------------------------
int vtkFetchMIDASParserXND::ParseMetatdataDeleteResponse(const char *filename )
{
  return 0;
}

//---------------------------------------------------------------------------
int vtkFetchMIDASParserXND::ParseResourceDeleteResponse(const char *filename )
{
  return 0;
}





  
//---------------------------------------------------------------------------
int vtkFetchMIDASParserXND::ParseForErrors (const char *val )
{
  return 0;
}



//---------------------------------------------------------------------------
void vtkFetchMIDASParserXND::GetXMLEntry ( vtkXMLDataElement *element )
{
    if (element ==  NULL )
    {
    vtkErrorMacro ( "vtkFetchMIDASParserXND::GetXMLEntry: called with null vtkXMLDataElement.");
    return;
    }

  const char *name = element->GetName();

  if ( name == NULL )
    {
    return;
    }

  const char *attName;
  int numAtts;
  const char *value;
  const char *dtype = "unknown";
  const char *tagValue;
  vtkXMLDataElement *nestedElement;
  std::string currentURI;


  // check to see if this is an item of interest.
  // process appropriately, based on what elements we find.


  //---
  //--- ERROR CHECKING
  //---
  if ( this->ErrorChecking)
    {
    if ((!(strcmp(name, "title" ))) ||
        (!(strcmp(name, "Title" ))) )
      {
      std::string errorString = element->GetCharacterData();
      std::string target = "error";
      size_t found = errorString.find ( target );
      if ( found != std::string::npos)
        {
        this->ErrorFlag = 1;
        }
      target = "Error";
      if ( found != std::string::npos)
        {
        this->ErrorFlag = 1;
        }
      target = "ERROR";
      if ( found != std::string::npos)
        {
        this->ErrorFlag = 1;
        }
      }
    }

  
  //---
  // RESOURCES have nested uris and tags (attribute/value pairs)
  // ONLY get these if the ResourceQuery flag is on. Otherwise query was
  // for tags or tag values, so ignore.
  //---
  if ( this->ResponseType == vtkFetchMIDASParser::ResourceQueryResponse )
    {    
    if ((!(strcmp(name, "resource" ))) ||
        (!(strcmp(name, "Resource" ))) ||
        (!(strcmp(name, "RESOURCE" ))) )
      {
      // parse the xml file and put result into ResourceDescription table.
      // looking for the SlicerDataType to pair with uri
      int nnested = element->GetNumberOfNestedElements();

      //--- get uri
      for ( int i=0; i < nnested; i++)
        {
        nestedElement = element->GetNestedElement ( i );
        if (nestedElement == NULL)
          {
          break;
          }

        if (!(strcmp(nestedElement->GetName(), "uri")) ||
            !(strcmp(nestedElement->GetName(), "Uri")) ||
            !(strcmp(nestedElement->GetName(), "URI")) )            
          {
          value = nestedElement->GetCharacterData();
          if ( value != NULL )
            {
            currentURI.clear();
            currentURI = value;
            this->FetchMIDASNode->GetResourceDescription()->AddOrUpdateTag ( currentURI.c_str(), dtype, 0 );        
            }
          }

        //--- get data type
        if (((!(strcmp(nestedElement->GetName(), "tag" ))) ||
             (!(strcmp(nestedElement->GetName(), "Tag" ))) ||
             (!(strcmp(nestedElement->GetName(), "TAG" )))) &&
            ( currentURI.c_str() != NULL  && (strcmp(currentURI.c_str(), "" )) ))
          {
          // see if the attribute in this tag is called Label, and if its value is SlicerDataType
          numAtts = nestedElement->GetNumberOfAttributes();
          for ( int j=0; j < numAtts; j++ )
            {
            attName = nestedElement->GetAttributeName(j);        
            value = nestedElement->GetAttributeValue(j);
            if ( attName != NULL && value != NULL )
              {            
              if ( !strcmp(value, "SlicerDataType" ) )
                {
                // if yes, then get the nested Value Element          
                int nnested2 = nestedElement->GetNumberOfNestedElements();
                vtkXMLDataElement *nested2Element = nestedElement;
                for ( int k=0; k < nnested2; k++)
                  {
                  nested2Element = nestedElement->GetNestedElement ( k );
                  if ( nested2Element == NULL )
                    {
                    break;
                    }
                  value = nested2Element->GetName();
                  if ( value != NULL )
                    {
                    if ( !(strcmp (value, "Value" )) ||
                         !(strcmp (value, "value" )) )
                      {
                      dtype = nested2Element->GetCharacterData();              
                      //now pair it up with currentURI and add to ResourceDescription.
                      this->FetchMIDASNode->GetResourceDescription()->AddOrUpdateTag ( currentURI.c_str(), dtype, 0 );
                      currentURI.clear();
                      break;              
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }


  //---
  // TAGS -- expect to find a list of elements called label containing tag attribute names.
  //---
  if (  this->ResponseType == vtkFetchMIDASParser::TagAttributesQueryResponse )
    {
    if ( (!(strcmp(name, "Label" ))) ||
         (!(strcmp(name, "label" ))) )
      {
      // parse the xml file and put result into node's XND tag table.
      // and into logic's CurrentWebServiceMetadata map.
      value = element->GetCharacterData();
      vtkXNDTagTable* t = vtkXNDTagTable::SafeDownCast (this->FetchMIDASNode->GetTagTableCollection()->FindTagTableByName ( "XND" ));
      //--- we are choosing to set SlicerDataType a default
      //--- value of 'MRML' in the XNDTagTable.
      //--- This promotes tagging of scenes with a
      //--- consistent attribute/value, and searching for scenes
      //--- with that consistent tag.
      if ( t != NULL && value != NULL )
        {
        //--- add to logic's map of tag attributes and all values
        this->AddUniqueTag ( value );
        //--- Add to node's tag table of tag attributes.
        //--- Keep selection status if we are just refreshing the
        //--- SAME server's metadata, and tag's already present.
        //--- If a NEW server has been selected, then t->RestoreSelectionState = 0
        //--- and we should just call AddOrUpdate with the new tag.
        //---
        //--- NOTE: this behavior may be friendly or annoying;
        //--- not sure. If we don't want to restore the tag's
        //--- selected state, then just get rid of the check on
        //--- t->RestoreSelectionState and always call AddOrUpdateTag
        //---
        if ( t->GetRestoreSelectionState() )
          {
//            if ( t->IsTagSelected(value) <= 0 )            
          if ( t->CheckTableForTag(value) < 0 )
            {
            //--- no need to update tag value or selected state.
            t->AddOrUpdateTag ( value, "no values found", 0 );
            }
          }
        else
          {
          //--- otherwise leave tag attribute, value, selected state as is.
          t->AddOrUpdateTag ( value, "no values found", 0 );
          }
        }
      }
    }

  
    //---
    // TAG VALUES -- expect long list of resources with nested
    // uris that we ignore here, and then the target list of tag
    // VALUEs for a given ATTRIBUTE at end.
    //---
    //--- Look for this guy:
    //---<Tag Label="ATTRIBUTE">
    //---<Value>VALUE_1</Value>
    //---<Value>VALUE_2</Value>
    //---</Tag>
    //---
  if ( this->ResponseType == vtkFetchMIDASParser::TagValuesQueryResponse )
    {
    if (((!(strcmp(name, "tag" ))) ||
         (!(strcmp(name, "Tag" ))) ||
         (!(strcmp(name, "TAG" )))) )
      {
      numAtts = element->GetNumberOfAttributes();
      for ( int j=0; j < numAtts; j++ )
        {
        attName = element->GetAttributeName(j);        
        value = element->GetAttributeValue(j);
        if ( attName != NULL && value != NULL )
          {
          if ( !strcmp(attName, "Label" ))
            {
            //--- get all nested values.
            int numnested = element->GetNumberOfNestedElements();
            vtkXMLDataElement *nestedElement;
            //--- get tag value
            for ( int i=0; i < numnested; i++)
              {
              nestedElement = element->GetNestedElement ( i );
              if (nestedElement != NULL)
                {
                if (!(strcmp(nestedElement->GetName(), "Value")) ||
                    !(strcmp(nestedElement->GetName(), "value")) )
                  {
                  tagValue = nestedElement->GetCharacterData();
                  this->AddUniqueValueForTag ( value, tagValue );
                  }
                }
              }
            }
          }
        }
      }
    }

}






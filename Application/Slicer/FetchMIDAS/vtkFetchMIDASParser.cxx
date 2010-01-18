#include "vtkObjectFactory.h"
#include "vtkFetchMIDASParser.h"


//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkFetchMIDASParser );
vtkCxxRevisionMacro ( vtkFetchMIDASParser, "");


//---------------------------------------------------------------------------
vtkFetchMIDASParser::vtkFetchMIDASParser ( )
{
  this->NumberOfElements = 0;
  this->ErrorFlag = 0;
  this->ErrorChecking = 0;
  this->ResponseType = 0;
  this->Name = NULL;
  this->Response = NULL;
  this->FetchMIDASNode = NULL;

}


//---------------------------------------------------------------------------
vtkFetchMIDASParser::~vtkFetchMIDASParser ( )
{
  this->NumberOfElements = 0;
  this->ResponseType = 0;
  this->ErrorFlag = 0;
  this->ErrorChecking = 0;
  this->ClearMetadataInformation();
  if ( this->FetchMIDASNode )
    {
    this->SetFetchMIDASNode(NULL);
    }
  if ( this->Response )
    {
    delete [] this->Response;
    this->Response = NULL;
    }
  if ( this->Name )
    {
    this->Name = NULL;
    }
}




//---------------------------------------------------------------------------
void vtkFetchMIDASParser::ClearMetadataInformation ( )
{
  std::map<std::string, std::vector<std::string> >::iterator iter;
  for ( iter = this->MetadataInformation.begin();
        iter != this->MetadataInformation.end();
        iter++ )
    {
    iter->second.clear();
    }
  this->MetadataInformation.clear();
}


//---------------------------------------------------------------------------
void vtkFetchMIDASParser::ClearValuesForAttribute ( const char *att)
{
 //--- first find the tag, then clear all the values.
  std::map<std::string, std::vector<std::string> >::iterator iter;
  for ( iter = this->MetadataInformation.begin();
        iter != this->MetadataInformation.end();
        iter++ )
    {
    if ( !(strcmp (iter->first.c_str(), att) ) )
      {
      iter->second.clear();
      break;
      }
    }
}


//---------------------------------------------------------------------------
void vtkFetchMIDASParser::PrintSelf ( ostream& os, vtkIndent indent )
{
    this->vtkObject::PrintSelf ( os, indent );
    os << indent << "NumberOfElements: " << this->GetNumberOfElements ( ) << "\n";    
    os << indent << "ErrorFlag: " << this->GetErrorFlag ( ) << "\n";    
    os << indent << "ErrorChecking: " << this->GetErrorChecking ( ) << "\n";    
    os << indent << "ResponseType: " << this->GetResponseType ( ) << "\n";    
    if ( this->FetchMIDASNode )
      {      
      os << indent << "FetchMIDASNode: " << this->GetFetchMIDASNode ( ) << "\n";
      }
    else
      {
      os << indent << "FetchMIDASNode: NULL\n";
      }

    if ( this->Name )
      {
      os << indent << "Name: " << this->GetName ( ) << "\n";
      }
    else
      {
      os << indent << "Name: NULL\n";
      }

}


//---------------------------------------------------------------------------
const char* vtkFetchMIDASParser::GetErrorString()
{
  return ( this->ErrorString.c_str() );
}

//---------------------------------------------------------------------------
void vtkFetchMIDASParser::SetErrorString( const char *errorMessage)
{
  this->ErrorString.clear();
  this->ErrorString = errorMessage;
}


//---------------------------------------------------------------------------
void vtkFetchMIDASParser::ClearErrorString()
{
  this->ErrorString.clear();
}




//----------------------------------------------------------------------------
void vtkFetchMIDASParser::GetXMLElement( vtkXMLDataElement *element )
{

  if (element ==  NULL )
    {
    vtkErrorMacro ( "vtkFetchMIDASParser: GetXMLElement called with null vtkXMLDataElement.");
    return;
    }

  this->GetXMLEntry ( element );

  int nnested = element->GetNumberOfNestedElements();
  for ( int i=0; i < nnested; i++)
    {
    vtkXMLDataElement *nestedElement = element->GetNestedElement ( i );
    if (nestedElement != NULL )
      {
      this->GetXMLElement (nestedElement);
      }
    }
}



//----------------------------------------------------------------------------
void vtkFetchMIDASParser::GetNumberOfXMLElements( vtkXMLDataElement *element )
{
  if (element ==  NULL )
    {
    vtkErrorMacro ( "vtkFetchMIDASParser: GetNumberOfXMLElements called with null vtkXMLDataElement.");
    return;
    }

  this->NumberOfElements++;
  int nnested = element->GetNumberOfNestedElements();
  for ( int i=0; i < nnested; i++ )
    {
    vtkXMLDataElement *e = element->GetNestedElement ( i );
    if ( e != NULL )
      {
      this->GetNumberOfXMLElements( e );
      }
    }
}


//----------------------------------------------------------------------------
void vtkFetchMIDASParser::AddUniqueTag ( const char *tagname )
{
  //--- search thru vector of strings to see if we can find the tag.
  int unique = 1;
  std::map<std::string, std::vector<std::string> >::iterator iter;
  for ( iter = this->MetadataInformation.begin();
        iter != this->MetadataInformation.end();
        iter++ )
    {
    if ( ! (strcmp (iter->first.c_str(), tagname ) ) )
      {
      unique = 0;
      break;
      }
    }

  //--- add tag if it's not already in list.
  if ( unique )
    {
    // TODO: check to see if this works
    // create and initialize a vector of strings 
    std::vector<std::string> v;
    v.clear();
    // create an entry for it in the map.
    this->MetadataInformation.insert ( std::make_pair (tagname, v ));
    }
}


//----------------------------------------------------------------------------
void vtkFetchMIDASParser::AddUniqueValueForTag ( const char *tagname, const char *val )
{
  //--- search thru vector of strings for tag to see if we can find the tag.
  int tagFound = 0;
  std::map<std::string, std::vector<std::string> >::iterator iter;
  for ( iter = this->MetadataInformation.begin();
        iter != this->MetadataInformation.end();
        iter++ )
    {
    if ( ! (strcmp (iter->first.c_str(), tagname ) ) )
      {
      tagFound=1;
      break;
      }
    }

  //--- add value for tag if it's not already in list.
  if ( tagFound )
    {
    int valFound = 0;
    for (unsigned int i=0; i < iter->second.size(); i++ )
      {
      if ( ! (strcmp(iter->second[i].c_str(), val)) )
        {
        valFound = 1;
        break;
        }
      }
    if ( !valFound )
      {
      iter->second.push_back ( val );
      }
    }
}




//----------------------------------------------------------------------------
int vtkFetchMIDASParser::GetNumberOfTagValues( const char *tagname)
{

  unsigned int numValues = 0;
  std::map<std::string, std::vector<std::string> >::iterator iter;
  for ( iter = this->MetadataInformation.begin();
        iter != this->MetadataInformation.end();
        iter++ )
    {
    if ( ! (strcmp (iter->first.c_str(), tagname ) ) )
      {
      numValues = iter->second.size();
      break;
      }
    }
  if ( numValues <= 0 )
    {
    return 0;
    }
  else
    {
    return ((int) numValues);
    }
}


//----------------------------------------------------------------------------
int vtkFetchMIDASParser::GetNumberOfTagsOnServer ( )
{

  unsigned int numTags = this->MetadataInformation.size();
  return ( numTags );

}



//----------------------------------------------------------------------------
const char * vtkFetchMIDASParser::GetNthTagAttribute ( int n)
{

  int cnt = 0;
  std::map<std::string, std::vector<std::string> >::iterator iter;
  for ( iter = this->MetadataInformation.begin();
        iter != this->MetadataInformation.end();
        iter++ )
    {
    if ( cnt < n )
      {
      cnt++;
      }
    else
      {
      return ( iter->first.c_str() );
      }
    }
  return ( NULL );

}


//----------------------------------------------------------------------------
const char * vtkFetchMIDASParser::GetNthValueForTag(const char *tagname, int n)
{

  std::map<std::string, std::vector<std::string> >::iterator iter;
  for ( iter = this->MetadataInformation.begin();
        iter != this->MetadataInformation.end();
        iter++ )  
    {
    iter->second.clear();
    }
  this->MetadataInformation.clear();

    {
    if ( ! (strcmp (iter->first.c_str(), tagname ) ) )
      {
      if ( n <= (int)(iter->second.size()-1) )
        {
        return ( iter->second[n].c_str() );
        }
      else
        {
        return NULL;
        }
      }
    }
  return NULL;
}



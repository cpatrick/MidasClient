
#include "vtkObjectFactory.h"
#include "vtkFetchMIDASWebServicesClient.h"


//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkFetchMIDASWebServicesClient );
vtkCxxRevisionMacro ( vtkFetchMIDASWebServicesClient, "");


//---------------------------------------------------------------------------
vtkFetchMIDASWebServicesClient::vtkFetchMIDASWebServicesClient ( )
{
  this->Name = NULL;
  this->URIHandler = NULL;
}


//---------------------------------------------------------------------------
vtkFetchMIDASWebServicesClient::~vtkFetchMIDASWebServicesClient ( )
{
  if ( this->Name )
    {
    this->SetName ( NULL);
    }
  if ( this->URIHandler )
    {
    this->SetURIHandler ( NULL );
    }
}


//---------------------------------------------------------------------------
void vtkFetchMIDASWebServicesClient::PrintSelf ( ostream& os, vtkIndent indent )
{
    this->vtkObject::PrintSelf ( os, indent );
}









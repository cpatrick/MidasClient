#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkFetchMIDASServerCollection.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro ( vtkFetchMIDASServerCollection );
vtkCxxRevisionMacro(vtkFetchMIDASServerCollection, "");

//---------------------------------------------------------------------------
vtkFetchMIDASServerCollection::vtkFetchMIDASServerCollection ( ) {
}


//----------------------------------------------------------------------------
void vtkFetchMIDASServerCollection::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf ( os, indent );
}

//---------------------------------------------------------------------------
vtkFetchMIDASServerCollection::~vtkFetchMIDASServerCollection ( )
{
  this->RemoveAllItems();
}



//---------------------------------------------------------------------------
int vtkFetchMIDASServerCollection::IsKnownServiceType ( const char* svctype )
{
  // NOTE TO DEVELOPERS: build this out as new web service types are added.
  // add your new service types here.
  int retval = 0;
  if ( !(strcmp(svctype,"XND") ) )
    {
    retval = 1;
    }
  if ( !(strcmp(svctype,"HID")))
    {
    retval = 1;
    }
  if ( !(strcmp(svctype,"MIDAS")))
    {
    retval = 1;
    }
  return (retval);
}


//---------------------------------------------------------------------------
void vtkFetchMIDASServerCollection::AddServerByName ( vtkFetchMIDASServer *w, const char* name )
{
  if ( name == NULL )
    {
    vtkErrorMacro ( "AddServerByName got null name.");
    return;
    }
  w->SetName ( name );
  this->vtkCollection::AddItem (w);
}


//---------------------------------------------------------------------------
void vtkFetchMIDASServerCollection::DeleteServerByName ( const char* name )
{
  if ( name == NULL )
    {
    vtkErrorMacro ( "DeleteServerByName got null name.");
    return;
    }

  vtkFetchMIDASServer *w;
  for ( int i=0; i < this->GetNumberOfItems(); i++ )
    {
    w = vtkFetchMIDASServer::SafeDownCast (this->GetItemAsObject(i));
    if ( w != NULL )
      {
      if ( ! strcmp(name,w->GetName()) )
        {
        this->RemoveItem ( i );
        w->Delete();
        break;
        }
      }
    }
}


//---------------------------------------------------------------------------
vtkFetchMIDASServer *vtkFetchMIDASServerCollection::FindServerByName (const char* name )
{
  if ( name == NULL )
    {
    vtkErrorMacro ( "FindServerByName got null name.");
    return NULL;
    }

  vtkFetchMIDASServer *w;
  for ( int i=0; i < this->GetNumberOfItems(); i++ )
    {
    w = vtkFetchMIDASServer::SafeDownCast (this->GetItemAsObject(i));
    if ( w != NULL )
      {
      if ( ! strcmp(name, w->GetName()) )
        {
        return ( w );
        }
      }
    }
  return ( NULL );
}

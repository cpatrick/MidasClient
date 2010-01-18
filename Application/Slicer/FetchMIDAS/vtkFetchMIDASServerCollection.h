#ifndef __vtkFetchMIDASServerCollection_h
#define __vtkFetchMIDASServerCollection_h

#include "vtkMRML.h"
#include "vtkObject.h"
#include "vtkCollection.h"
#include "vtkFetchMIDASServer.h"


class VTK_FETCHMIDAS_EXPORT vtkFetchMIDASServerCollection : public vtkCollection
{
public:
  static vtkFetchMIDASServerCollection *New();
  vtkTypeRevisionMacro(vtkFetchMIDASServerCollection, vtkCollection);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Return a server that corresponds to the specified name.
  vtkFetchMIDASServer * FindServerByName ( const char* name );

  // Description:
  // Add a new server to the collection with the specified name. The server
  // can be retrieved by this name using the FindServerByName (name) method.
  void AddServerByName ( vtkFetchMIDASServer *s, const char* name );

  // Description:
  // Delete a server with the specified name from the collection.
  void DeleteServerByName ( const char* name );

  // Description:
  // Add a Server to the Collection. It is recommended to use the AddServerByName(name)
  // method, so the Server can be retrieved and deleted by name.
  void AddItem ( vtkFetchMIDASServer *s ) {
  this->vtkCollection::AddItem ( (vtkObject *)s); };

  // Description:
  // Get the next Server in the list. Return NULL
  // when at the end of the list.
  vtkFetchMIDASServer *GetNextServer() {
  return static_cast<vtkFetchMIDASServer *>(this->GetNextItemAsObject() ); };

  // Description:
  // Access routine provided for compatibility with previous
  // versions of VTK.  Please use the GetNextServer() variant
  // where possible.
  vtkFetchMIDASServer *GetNextItem() { return this->GetNextServer(); };

  // Description:
  // This method returns 1 if the service type is known, 0 if not.
  // NOTE TO DEVELOPERS: known web service types are enumeraged
  // in this class. Currently supported service types are:
  // vtkFetchMIDASServerCollection::XND, and
  // vtkFetchMIDASServerCollection::HID is being developed.
  // As new services are supported, please add them to the enumeration
  // and the check in this method.
  int IsKnownServiceType ( const char* stype );

  // Description: 
  // Reentrant safe way to get an object in a collection; just pass the
  // same cookie back and forth. 
  vtkFetchMIDASServer *GetNextServer(vtkCollectionSimpleIterator &cookie) {
    return static_cast<vtkFetchMIDASServer *>(this->GetNextItemAsObject(cookie));};

 protected:
  vtkFetchMIDASServerCollection();
  ~vtkFetchMIDASServerCollection();

  // hide the standard AddItem from the user and compiler.
  void AddItem (vtkObject *o ) { this->vtkCollection::AddItem(o); } ;
  
  vtkFetchMIDASServerCollection(const vtkFetchMIDASServerCollection&);
  void operator=(const vtkFetchMIDASServerCollection&); private:
};

#endif

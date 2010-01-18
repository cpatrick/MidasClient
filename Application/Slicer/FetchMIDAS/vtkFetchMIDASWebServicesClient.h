#ifndef __vtkFetchMIDASWebServicesClient_h
#define __vtkFetchMIDASWebServicesClient_h

#include "vtkObject.h"
#include "vtkFetchMIDASWin32Header.h"
#include "vtkURIHandler.h"
#include "vtkTagTable.h"

class VTK_FETCHMIDAS_EXPORT vtkFetchMIDASWebServicesClient : public vtkObject
{
  
public:
  static vtkFetchMIDASWebServicesClient* New();
  vtkTypeRevisionMacro(vtkFetchMIDASWebServicesClient,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get/Set on the Name of this WebServicesClient
  vtkGetStringMacro ( Name );
  vtkSetStringMacro ( Name );

  vtkGetObjectMacro ( URIHandler, vtkURIHandler );
  vtkSetObjectMacro ( URIHandler, vtkURIHandler );
  
  virtual int QueryServerForTags ( const char *responseFileName ) { return 0; };
  virtual int QueryServerForTagValues ( const char *att, const char *responseFilename ) { return 0; };
  virtual int QueryServerForResources ( vtkTagTable *table, const char *responseFileName ) { return 0; };
  virtual int DeleteResourceFromServer ( const char *uri, const char *responseFileName ) { return 0; };

  // Description:
  // Creates a new tag on a server; method used during upload
  virtual int AddTagToServer ( const char *att, const char *val, const char *responseFileName ) {return 0; };
  
  // Description:
  // Posts metadata describing resource; method used during upload
  virtual int CreateNewURIForResource( const char *resourceName,
                               const char *headerFileName,
                               const char *uploadFileName,
                               const char *responseFileName)  {return 0; };

  virtual void Download ( const char *src, const char *dest ) { };
  virtual void Upload ( const char *src, const char *dest ) { };
  
 protected:
  vtkFetchMIDASWebServicesClient();
  virtual ~vtkFetchMIDASWebServicesClient();
  vtkURIHandler *URIHandler;
  char *Name;
  
  vtkFetchMIDASWebServicesClient(const vtkFetchMIDASWebServicesClient&); // Not implemented
  void operator=(const vtkFetchMIDASWebServicesClient&); // Not Implemented
};

#endif


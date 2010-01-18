#ifndef __vtkFetchMIDASWebServicesClientMIDAS_h
#define __vtkFetchMIDASWebServicesClientMIDAS_h

#include "vtkObject.h"
#include "vtkFetchMIDASWin32Header.h"
#include "vtkFetchMIDASWebServicesClient.h"

class VTK_FETCHMIDAS_EXPORT vtkFetchMIDASWebServicesClientMIDAS : public vtkFetchMIDASWebServicesClient
{
  
public:
  static vtkFetchMIDASWebServicesClientMIDAS* New();
  vtkTypeRevisionMacro(vtkFetchMIDASWebServicesClientMIDAS,vtkFetchMIDASWebServicesClient);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual int QueryServerForTags ( const char *responseFileName ){return 0;};
  virtual int QueryServerForTagValues ( const char *att, const char *responseFilename ){return 0;};
  virtual int QueryServerForResources ( vtkTagTable *table, const char *responseFileName ){return 0;};
  virtual int DeleteResourceFromServer ( const char *uri, const char *responseFileName ){return 0;};

  // Description:
  // Creates a new tag on a server; method used during upload
  virtual int AddTagToServer ( const char *att, const char *val, const char *responseFileName){return 0;};
  
  // Description:
  // Posts metadata describing resource; method used during upload
  virtual int CreateNewURIForResource( const char *resourceName,
                               const char *headerFileName,
                               const char *uploadFileName,
                               const char *responseFileName){return 0;};
  
  virtual void Download ( const char *src, const char *dest );
  virtual void Upload ( const char *src, const char *dest ) { };
  
 protected:
  vtkFetchMIDASWebServicesClientMIDAS();
  virtual ~vtkFetchMIDASWebServicesClientMIDAS();

};

#endif


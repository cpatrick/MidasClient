#ifndef __vtkFetchMIDASServer_h
#define __vtkFetchMIDASServer_h

#include "vtkObject.h"
#include "vtkFetchMIDASWin32Header.h"
#include "vtkURIHandler.h"
#include "vtkFetchMIDASParser.h"
#include "vtkFetchMIDASWriter.h"
#include "vtkFetchMIDASWebServicesClient.h"
#include "vtkTagTable.h"
#include <string>

class VTK_FETCHMIDAS_EXPORT vtkFetchMIDASServer : public vtkObject
  {
  public:
    static vtkFetchMIDASServer* New();
    vtkTypeRevisionMacro(vtkFetchMIDASServer, vtkObject);
    void PrintSelf(ostream& os, vtkIndent indent);

    // Description:
    // Get/Set on the name of the host
    vtkGetStringMacro ( Name );
    vtkSetStringMacro ( Name );

    // Description:
    // Get/Set on the Server's parser
    vtkSetObjectMacro ( Parser, vtkFetchMIDASParser );
    vtkGetObjectMacro ( Parser, vtkFetchMIDASParser );

    // Description:
    // Get/Set on the Server's writer
    vtkSetObjectMacro ( Writer, vtkFetchMIDASWriter );
    vtkGetObjectMacro ( Writer, vtkFetchMIDASWriter );

    // Description:
    // Get/Set on the Server's clientcaller
    vtkSetObjectMacro ( WebServicesClient, vtkFetchMIDASWebServicesClient );
    vtkGetObjectMacro ( WebServicesClient, vtkFetchMIDASWebServicesClient );

    // Description:
    // Get/Set on the Server's clientcaller
    vtkSetObjectMacro ( URIHandler, vtkURIHandler );
    vtkGetObjectMacro ( URIHandler, vtkURIHandler );

    // Description:
    // The logic's ServerCollection contains an enumeration of all
    // supported service types; Slicer uses these types to determine
    // what kind of webservice goes with this server.
    vtkGetStringMacro ( ServiceType);
    vtkSetStringMacro ( ServiceType);

    // Description:
    // Get/Set on the name of a URIHandler that goes with this server.
    // The table can retrieved from the MRMLScene's handler collection by name.
    vtkGetStringMacro ( URIHandlerName );
    vtkSetStringMacro ( URIHandlerName );


    // Description:
    // Get/Set on the name of a TagTable that goes with this server.
    // Certain web services may have their own required tags, which
    // should populate the table. The table can be retrieved from the
    // logic's collection of tag tables by name.
    vtkGetStringMacro ( TagTableName );
    vtkSetStringMacro ( TagTableName );
    vtkGetObjectMacro ( TagTable, vtkTagTable );
    vtkSetObjectMacro ( TagTable, vtkTagTable );

  protected:
    virtual ~vtkFetchMIDASServer();
    vtkFetchMIDASServer();

    char *ServiceType;
    char *Name;
    char *URIHandlerName;
    char *TagTableName;

    vtkFetchMIDASParser *Parser;
    vtkFetchMIDASWriter *Writer;
    vtkFetchMIDASWebServicesClient *WebServicesClient;
    vtkURIHandler *URIHandler;
    vtkTagTable *TagTable;
  
  };

#endif


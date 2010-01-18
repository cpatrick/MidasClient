#ifndef __vtkFetchMIDASWriterXND_h
#define __vtkFetchMIDASWriterXND_h

#include "vtkObject.h"
#include "vtkFetchMIDASWriter.h"
#include "vtkFetchMIDASWin32Header.h"

class VTK_FETCHMIDAS_EXPORT vtkFetchMIDASWriterXND : public vtkFetchMIDASWriter
{
  
public:
  static vtkFetchMIDASWriterXND* New();
  vtkTypeRevisionMacro(vtkFetchMIDASWriterXND, vtkFetchMIDASWriter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Method that writes the document declaration for a resource post.
  virtual void WriteDocumentDeclaration();

  // Description:
  // Method that writes the XML header document.
  virtual void WriteXMLHeader ( const char *dataFilename);

  // Description:
  // Method that writes the metadata for a node.
  virtual void WriteMetadataForNode ( const char *nodeID, vtkMRMLScene *scene );

  // Description:
  // Method that writes the metadata for the scene.
  virtual void WriteMetadataForScene ( vtkMRMLScene *scene );
    
 protected:
  vtkFetchMIDASWriterXND();
  virtual ~vtkFetchMIDASWriterXND();

  vtkFetchMIDASWriterXND(const vtkFetchMIDASWriterXND&); // Not implemented
  void operator=(const vtkFetchMIDASWriterXND&); // Not Implemented
};

#endif


#ifndef __vtkFetchMIDASIcons_h
#define __vtkFetchMIDASIcons_h

#include "vtkKWObject.h"
#include "vtkKWResourceUtilities.h"
#include "vtkKWIcon.h"
#include "vtkSlicerIcons.h"
#include "./Resources/vtkFetchMIDAS_ImageData.h"

class vtkFetchMIDASIcons : public vtkSlicerIcons
{
 public:
    static vtkFetchMIDASIcons* New ( );
    vtkTypeRevisionMacro ( vtkFetchMIDASIcons, vtkSlicerIcons);
    void PrintSelf ( ostream& os, vtkIndent indent );

    vtkGetObjectMacro ( DeleteAllIcon, vtkKWIcon );
    vtkGetObjectMacro ( DeleteSelectedIcon, vtkKWIcon );
    vtkGetObjectMacro ( SelectAllIcon, vtkKWIcon );
    vtkGetObjectMacro ( DeselectAllIcon, vtkKWIcon );    
    vtkGetObjectMacro ( AddNewIcon, vtkKWIcon );
    vtkGetObjectMacro ( SearchIcon, vtkKWIcon );
    vtkGetObjectMacro ( DownloadIcon, vtkKWIcon );
    vtkGetObjectMacro (UploadIcon, vtkKWIcon );
    vtkGetObjectMacro (ApplyTagsIcon, vtkKWIcon );
    vtkGetObjectMacro (RemoveTagsIcon, vtkKWIcon );
    vtkGetObjectMacro (ShowDataTagsIcon, vtkKWIcon );
    vtkGetObjectMacro (RefreshServerIcon, vtkKWIcon );
    vtkGetObjectMacro (HelpIcon, vtkKWIcon );
    
    void AssignImageDataToIcons ( );
    
 protected:
    vtkFetchMIDASIcons ( );
    ~vtkFetchMIDASIcons ( );
    vtkKWIcon *HelpIcon;
    vtkKWIcon *AddNewIcon;
    vtkKWIcon *DeleteAllIcon;
    vtkKWIcon *DeleteSelectedIcon;
    vtkKWIcon *SelectAllIcon;
    vtkKWIcon *DeselectAllIcon;
    vtkKWIcon *SearchIcon;
    vtkKWIcon *DownloadIcon;
    vtkKWIcon *UploadIcon;
    vtkKWIcon *ApplyTagsIcon;
    vtkKWIcon *RemoveTagsIcon;
    vtkKWIcon *ShowDataTagsIcon;
    vtkKWIcon *RefreshServerIcon;
    
 private:
    vtkFetchMIDASIcons (const vtkFetchMIDASIcons&); // Not implemented
    void operator = ( const vtkFetchMIDASIcons& ); // Not implemented
    
};

#endif

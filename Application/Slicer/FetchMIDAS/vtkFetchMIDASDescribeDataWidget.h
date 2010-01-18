#ifndef __vtkFetchMIDASDescribeDataWidget_h
#define __vtkFetchMIDASDescribeDataWidget_h


#include "vtkFetchMIDASWin32Header.h"
#include "vtkFetchMIDASMulticolumnWidget.h"

class vtkKWPushButton;
class vtkFetchMIDASIcons;

class VTK_FETCHMIDAS_EXPORT vtkFetchMIDASDescribeDataWidget : public vtkFetchMIDASMulticolumnWidget
{
  
public:
  static vtkFetchMIDASDescribeDataWidget* New();
  vtkTypeRevisionMacro(vtkFetchMIDASDescribeDataWidget,vtkFetchMIDASMulticolumnWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get methods on class members ( no Set methods required. )
  vtkGetObjectMacro ( AddNewButton, vtkKWPushButton);
  vtkGetObjectMacro ( CopyTagsButton, vtkKWPushButton );
  vtkGetObjectMacro ( ShowTagsButton, vtkKWPushButton );
  vtkGetObjectMacro ( SelectAllButton, vtkKWPushButton);
  vtkGetObjectMacro ( DeselectAllButton, vtkKWPushButton);
  vtkGetObjectMacro ( ClearAllButton, vtkKWPushButton);
  vtkGetObjectMacro ( ClearSelectedButton, vtkKWPushButton);
  vtkGetObjectMacro ( FetchMIDASIcons, vtkFetchMIDASIcons );
  
  virtual void AddNewItem ( const char *keyword, const char *value);

  // Description:
  // alternative method to propagate events generated in GUI to logic / mrml
  virtual void ProcessWidgetEvents ( vtkObject *caller, unsigned long event, void *callData );
  
  // Description:
  // alternative method to propagate events generated in GUI to logic / mrml
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );
  
  // Description:
  // removes observers on widgets in the class
  virtual void RemoveWidgetObservers ( );
  // Description:
  // adds observers on widgets in the class
  virtual void AddWidgetObservers ( );

  // Description:
  // add observers on color node
  virtual void AddMRMLObservers ( );

  // Description:
  // remove observers on color node
  virtual void RemoveMRMLObservers ( );

  
 protected:
  vtkFetchMIDASDescribeDataWidget();
  virtual ~vtkFetchMIDASDescribeDataWidget();

  // custom buttons and icons
  vtkKWPushButton *AddNewButton;
  vtkKWPushButton *CopyTagsButton;
  vtkKWPushButton *ShowTagsButton;  
  vtkKWPushButton *SelectAllButton;
  vtkKWPushButton *DeselectAllButton;
  vtkKWPushButton *ClearAllButton;
  vtkKWPushButton *ClearSelectedButton;
  vtkFetchMIDASIcons *FetchMIDASIcons;

  // Description:
  // Create the widget.
  virtual void CreateWidget();

  // Description:
  // Update the widget, used when the color node id changes
  void UpdateWidget();
  
  void UpdateMRML();

  vtkFetchMIDASDescribeDataWidget(const vtkFetchMIDASDescribeDataWidget&); // Not implemented
  void operator=(const vtkFetchMIDASDescribeDataWidget&); // Not Implemented
};

#endif


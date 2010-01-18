#ifndef __vtkFetchMIDASMulticolumnWidget_h
#define __vtkFetchMIDASMulticolumnWidget_h

#include "vtkFetchMIDASWin32Header.h"
#include "vtkSlicerWidget.h"

class vtkKWMultiColumnListWithScrollbars;
class vtkKWFrame;

class VTK_FETCHMIDAS_EXPORT vtkFetchMIDASMulticolumnWidget : public vtkSlicerWidget
{
  
public:
  static vtkFetchMIDASMulticolumnWidget* New();
  vtkTypeRevisionMacro(vtkFetchMIDASMulticolumnWidget,vtkSlicerWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get methods on class members ( no Set methods required. )
  vtkGetObjectMacro ( MultiColumnList, vtkKWMultiColumnListWithScrollbars );
  vtkGetObjectMacro ( ContainerFrame, vtkKWFrame );
  vtkGetMacro (NumberOfColumns, int );
  
  virtual void AddNewItem () { };
  virtual void SelectAllItems ( );
  virtual void DeselectAllItems ( );
  virtual void DeleteAllItems ( );
  virtual void DeleteSelectedItems ( );
  
  // Description:
  // Method that gets all terms in the multicolumn list
  virtual void GetAllItems ( );

  // Description:
  // Method that gets all terms in the multicolumn list
  virtual void GetItemsToUse ( );

  virtual int GetNumberOfItemsToUse( ) {
  return useTerms.size();
  }
  virtual const char *GetNthItemToUse ( int ind ) {
  return useTerms[ind].c_str();
  }
  
  //BTX
  std::vector<std::string> useTerms;
  //ETX

 protected:
  vtkFetchMIDASMulticolumnWidget();
  virtual ~vtkFetchMIDASMulticolumnWidget();

  vtkKWMultiColumnListWithScrollbars *MultiColumnList;
  vtkKWFrame *ContainerFrame;
  
  // Description:
  // Create the widget.
  virtual void CreateWidget();
  
  //BTX
  // Description:
  // The column orders in the list box
  enum
    {
      MulticolumnColumn = 0,
    };
  //ETX

  int NumberOfColumns;

  vtkFetchMIDASMulticolumnWidget(const vtkFetchMIDASMulticolumnWidget&); // Not implemented
  void operator=(const vtkFetchMIDASMulticolumnWidget&); // Not Implemented
};

#endif


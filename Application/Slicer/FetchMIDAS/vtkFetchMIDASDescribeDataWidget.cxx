#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkProperty.h"

#include "vtkFetchMIDASDescribeDataWidget.h"
#include "vtkSlicerApplication.h"
#include "vtkKWFrame.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWMultiColumnListWithScrollbars.h"
#include "vtkKWPushButton.h"
#include "vtkKWIcon.h"
#include "vtkKWCheckButton.h"
#include "vtkFetchMIDASIcons.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkFetchMIDASDescribeDataWidget );
vtkCxxRevisionMacro ( vtkFetchMIDASDescribeDataWidget, "");


//---------------------------------------------------------------------------
vtkFetchMIDASDescribeDataWidget::vtkFetchMIDASDescribeDataWidget ( )
{

    this->AddNewButton = NULL;
    this->ShowTagsButton = NULL;
    this->SelectAllButton = NULL;
    this->DeselectAllButton = NULL;
    this->ClearAllButton = NULL;
    this->ClearSelectedButton = NULL;
    this->FetchMIDASIcons = NULL;
    this->NumberOfColumns = 4;

}


//---------------------------------------------------------------------------
vtkFetchMIDASDescribeDataWidget::~vtkFetchMIDASDescribeDataWidget ( )
{
  this->RemoveMRMLObservers();
  this->RemoveWidgetObservers();

  if ( this->AddNewButton )
    {
    this->AddNewButton->SetParent ( NULL );
    this->AddNewButton->Delete();
    this->AddNewButton = NULL;    
    }
  if ( this->SelectAllButton )
    {
    this->SelectAllButton->SetParent ( NULL );
    this->SelectAllButton->Delete();
    this->SelectAllButton = NULL;    
    }
  if ( this->DeselectAllButton )
    {
    this->DeselectAllButton->SetParent ( NULL );
    this->DeselectAllButton->Delete();
    this->DeselectAllButton = NULL;
    }
  if ( this->ClearAllButton )
    {
    this->ClearAllButton->SetParent ( NULL );
    this->ClearAllButton->Delete();
    this->ClearAllButton = NULL;    
    }
  if ( this->ClearSelectedButton )
    {
    this->ClearSelectedButton->SetParent ( NULL );
    this->ClearSelectedButton->Delete();
    this->ClearSelectedButton = NULL;    
    }
  if ( this->ShowTagsButton)
    {
    this->ShowTagsButton->SetParent ( NULL );    
    this->ShowTagsButton->Delete();
    this->ShowTagsButton = NULL;
    }

  if ( this->FetchMIDASIcons )
    {
    this->FetchMIDASIcons->Delete();
    this->FetchMIDASIcons = NULL;
    }
  this->SetMRMLScene ( NULL );

}




//---------------------------------------------------------------------------
void vtkFetchMIDASDescribeDataWidget::PrintSelf ( ostream& os, vtkIndent indent )
{
    this->vtkObject::PrintSelf ( os, indent );

    os << indent << "vtkFetchMIDASDescribeDataWidget: " << this->GetClassName ( ) << "\n";
    os << indent << "AddNewButton: " << this->GetAddNewButton() << "\n";
    os << indent << "ShowTagsButton: " << this->GetShowTagsButton() << "\n";
    os << indent << "SelectAllButton: " << this->GetSelectAllButton() << "\n";
    os << indent << "ClearSelectedButton: " << this->GetClearSelectedButton() << "\n";
    os << indent << "ClearAllButton: " << this->GetClearAllButton() << "\n";
    os << indent << "DeselectAllButton: " << this->GetDeselectAllButton() << "\n";
}



//---------------------------------------------------------------------------
void vtkFetchMIDASDescribeDataWidget::ProcessWidgetEvents ( vtkObject *caller,
                                                         unsigned long event, void *callData )
{
  vtkKWPushButton *b = vtkKWPushButton::SafeDownCast ( caller );
  
  if ( this->IsCreated() )
    {
    if ( (b == this->GetClearAllButton()) && (event == vtkKWPushButton::InvokedEvent ) )
      {
      this->DeleteAllItems( );
      }
    else if ( (b == this->GetDeselectAllButton()) && (event == vtkKWPushButton::InvokedEvent ) )
      {
      this->DeselectAllItems ( );
      }
    else if ( (b == this->GetAddNewButton()) && (event == vtkKWPushButton::InvokedEvent ) )
      {
      this->AddNewItem( "", "" );
      }
    else if ( (b == this->GetClearSelectedButton()) && (event == vtkKWPushButton::InvokedEvent ) )
      {
      this->DeleteSelectedItems ();
      }
    else if ( (b == this->GetSelectAllButton()) && (event == vtkKWPushButton::InvokedEvent ) )
      {
      this->SelectAllItems ( );
      }
    }
  this->UpdateMRML();
} 



//---------------------------------------------------------------------------
void vtkFetchMIDASDescribeDataWidget::ProcessMRMLEvents ( vtkObject *caller,
                                              unsigned long event, void *callData )
{
  // nothing; handle in parent.
}

//---------------------------------------------------------------------------
void vtkFetchMIDASDescribeDataWidget::AddMRMLObservers ( )
{
}

//---------------------------------------------------------------------------
void vtkFetchMIDASDescribeDataWidget::RemoveMRMLObservers ( )
{
}

//---------------------------------------------------------------------------
void vtkFetchMIDASDescribeDataWidget::UpdateWidget()
{
}


//---------------------------------------------------------------------------
void vtkFetchMIDASDescribeDataWidget::UpdateMRML()
{
  // nothing for now, not allowing editing
}


//---------------------------------------------------------------------------
void vtkFetchMIDASDescribeDataWidget::RemoveWidgetObservers ( ) {

  this->GetClearAllButton()->RemoveObservers(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->GetDeselectAllButton()->RemoveObservers(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->GetAddNewButton()->RemoveObservers(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->GetClearSelectedButton()->RemoveObservers(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );  
  this->GetSelectAllButton()->RemoveObservers(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
}


//---------------------------------------------------------------------------
void vtkFetchMIDASDescribeDataWidget::AddWidgetObservers ( ) {
  this->GetClearAllButton()->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->GetDeselectAllButton()->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );  
  this->GetAddNewButton()->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->GetClearSelectedButton()->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );  
  this->GetSelectAllButton()->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
}



//---------------------------------------------------------------------------
void vtkFetchMIDASDescribeDataWidget::CreateWidget ( )
{
  // Check if already created

  if (this->IsCreated())
    {
    vtkErrorMacro(<< this->GetClassName() << " already created");
    return;
    }
  
  // Call the superclass to create the whole widget
  this->Superclass::CreateWidget();

  // create the icons
  this->FetchMIDASIcons = vtkFetchMIDASIcons::New();

  this->GetMultiColumnList()->GetWidget()->AddColumn ( "use" );
  this->GetMultiColumnList()->GetWidget()->ColumnEditableOn ( 0 );
  this->GetMultiColumnList()->GetWidget()->SetColumnAlignmentToLeft (0 );
  this->GetMultiColumnList()->GetWidget()->ColumnResizableOff ( 0 );
  this->GetMultiColumnList()->GetWidget()->ColumnStretchableOff ( 0 );
  this->GetMultiColumnList()->GetWidget()->SetColumnFormatCommandToEmptyOutput(0);
  this->GetMultiColumnList()->GetWidget()->SetColumnEditWindowToCheckButton(0);

  this->GetMultiColumnList()->GetWidget()->AddColumn ( "show" );
  this->GetMultiColumnList()->GetWidget()->ColumnEditableOff ( 1 );
  this->GetMultiColumnList()->GetWidget()->SetColumnAlignmentToLeft (1 );
  this->GetMultiColumnList()->GetWidget()->ColumnResizableOff ( 1 );
  this->GetMultiColumnList()->GetWidget()->ColumnStretchableOff ( 1 );
  this->GetMultiColumnList()->GetWidget()->SetColumnFormatCommandToEmptyOutput(1);

  this->GetMultiColumnList()->GetWidget()->AddColumn ( "keyword" );
  this->GetMultiColumnList()->GetWidget()->ColumnEditableOn ( 2 );
  this->GetMultiColumnList()->GetWidget()->ColumnResizableOn ( 2 );
  this->GetMultiColumnList()->GetWidget()->SetColumnWidth ( 2, 0 );
  this->GetMultiColumnList()->GetWidget()->SetColumnSortModeToAscii ( 2 );
  this->GetMultiColumnList()->GetWidget()->SetColumnEditWindowToEntry (2);

  this->GetMultiColumnList()->GetWidget()->AddColumn ( "value" );
  this->GetMultiColumnList()->GetWidget()->ColumnEditableOn ( 3 );
  this->GetMultiColumnList()->GetWidget()->ColumnResizableOn ( 3 );
  this->GetMultiColumnList()->GetWidget()->SetColumnWidth ( 3, 0 );
  this->GetMultiColumnList()->GetWidget()->SetColumnSortModeToAscii ( 3 );
  this->GetMultiColumnList()->GetWidget()->SetColumnEditWindowToEntry (3);
  this->Script ( "pack %s -side top -fill x -expand n", this->GetMultiColumnList()->GetWidgetName() );

  // frame for the buttons
  vtkKWFrame *bFrame = vtkKWFrame::New();
  bFrame->SetParent ( this->ContainerFrame );
  bFrame->Create();
  this->Script ("pack %s -side top -fill none -anchor c -expand n -padx 2 -pady 2", bFrame->GetWidgetName() );

  this->AddNewButton = vtkKWPushButton::New();
  this->AddNewButton->SetParent (bFrame);
  this->AddNewButton->Create();
  this->AddNewButton->SetBorderWidth ( 0 );
  this->AddNewButton->SetReliefToFlat();  
  this->AddNewButton->SetImageToIcon ( this->FetchMIDASIcons->GetAddNewIcon() );
  this->AddNewButton->SetBalloonHelpString ( "Add new user tag" );

  this->SelectAllButton = vtkKWPushButton::New();
  this->SelectAllButton->SetParent (bFrame);
  this->SelectAllButton->Create();
  this->SelectAllButton->SetBorderWidth ( 0 );
  this->SelectAllButton->SetReliefToFlat();  
  this->SelectAllButton->SetImageToIcon ( this->FetchMIDASIcons->GetSelectAllIcon() );
  this->SelectAllButton->SetBalloonHelpString ( "Select all tags in list" );

  this->ClearSelectedButton = vtkKWPushButton::New();
  this->ClearSelectedButton->SetParent (bFrame);
  this->ClearSelectedButton->Create();
  this->ClearSelectedButton->SetBorderWidth ( 0 );
  this->ClearSelectedButton->SetReliefToFlat ( );  
  this->ClearSelectedButton->SetImageToIcon ( this->FetchMIDASIcons->GetDeleteSelectedIcon() );
  this->ClearSelectedButton->SetBalloonHelpString ( "Delete selected tags from list" );

  this->ClearAllButton = vtkKWPushButton::New();
  this->ClearAllButton->SetParent (bFrame);
  this->ClearAllButton->Create();
  this->ClearAllButton->SetBorderWidth ( 0 );
  this->ClearAllButton->SetReliefToFlat();  
  this->ClearAllButton->SetImageToIcon ( this->FetchMIDASIcons->GetDeleteAllIcon() );
  this->ClearAllButton->SetBalloonHelpString ( "Delete all tags in list" );

  this->DeselectAllButton = vtkKWPushButton::New();
  this->DeselectAllButton->SetParent (bFrame);
  this->DeselectAllButton->Create();
  this->DeselectAllButton->SetBorderWidth ( 0 );
  this->DeselectAllButton->SetReliefToFlat();  
  this->DeselectAllButton->SetImageToIcon ( this->FetchMIDASIcons->GetDeselectAllIcon() );
  this->DeselectAllButton->SetBalloonHelpString ( "Deselect all tags in list" );

  this->Script ("pack %s %s %s %s %s -side left -anchor c -expand n -padx 3 -pady 2",
               this->AddNewButton->GetWidgetName(),
               this->SelectAllButton->GetWidgetName(),
               this->DeselectAllButton->GetWidgetName(),
               this->ClearSelectedButton->GetWidgetName(),
               this->ClearAllButton->GetWidgetName());
  bFrame->Delete();

}


//---------------------------------------------------------------------------
void vtkFetchMIDASDescribeDataWidget::AddNewItem ( const char *keyword, const char *value )
{
  // default query terms in list
  int i, unique;

  unique = 1;
  // check to see if term is unique before adding it
  int n = this->GetMultiColumnList()->GetWidget()->GetNumberOfRows();
  for ( i=0; i<n; i++ )
    {
    if ( !strcmp (this->GetMultiColumnList()->GetWidget()->GetCellText(i, 2), keyword ) )
      {
      unique = 0;
      }
    }
  if ( !strcmp (keyword, "") )
    {
    keyword = "<new>";
    }
  if ( !strcmp (value, "") )
    {
    value = "<NULL>";
    }

  if ( unique )
    {
    i = this->GetMultiColumnList()->GetWidget()->GetNumberOfRows();
    this->GetMultiColumnList()->GetWidget()->AddRow();
    this->GetMultiColumnList()->GetWidget()->SetCellWindowCommandToCheckButton(i, 0);
    this->GetMultiColumnList()->GetWidget()->SetCellText (i, 2, keyword );
    this->GetMultiColumnList()->GetWidget()->SetCellText (i, 3, value );
    this->GetMultiColumnList()->GetWidget()->SetCellBackgroundColor (i, 0, 1.0, 1.0, 1.0);
    }
}








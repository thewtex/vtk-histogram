#include <vtkAxis.h>
#include <vtkChartXY.h>
#include <vtkCommand.h>
#include <vtkContextView.h>
#include <vtkContextScene.h>
#include <vtkDoubleArray.h>
#include <vtkImageAccumulate.h>
#include <vtkImageData.h>
#include <vtkIntArray.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageReader2.h>
#include <vtkPlot.h>
#include <vtkTextProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTable.h>
#include <vtkSmartPointer.h>

#include <sstream>

#include "histogramArgs.h"

class ResetAxesCommand: public vtkCommand
{
public:
  vtkTypeRevisionMacro(ResetAxesCommand, vtkCommand);

  static ResetAxesCommand *New()
    {
    return new ResetAxesCommand;
    }

  virtual void Execute(vtkObject *vtkNotUsed(caller), unsigned long vtkNotUsed(eventId), 
    void *vtkNotUsed(callData))
    {
      if( this->Chart == NULL )
        {
        std::cerr << "ResetAxesCommand axes have not been set." << std::endl;
        return;
        }

      this->Chart->RecalculateBounds();
    }

  void SetChart( vtkChart *chart )
    {
    this->Chart = chart;
    }

protected:
  ResetAxesCommand():
    Chart( NULL )
  {}

private:
  vtkChart *Chart;
};
vtkCxxRevisionMacro(ResetAxesCommand, "$Revision: 1.0$");

int main( int argc, char *argv[] )
{
  // Handle the arguments
  try
    {
    Args args( argc, argv );

    // Read a jpeg image
    vtkSmartPointer<vtkImageReader2Factory> readerFactory =
      vtkSmartPointer<vtkImageReader2Factory>::New();
    vtkImageReader2 * reader = readerFactory->CreateImageReader2( args.inputImage.c_str() );
    reader->SetFileName( args.inputImage.c_str() );
    reader->Update();
    double* scalarRange = reader->GetOutput()->GetScalarRange();

    double minValue;
    if( args.minValueWasSet )
      {
      minValue = args.minValue;
      }
    else
      {
      minValue = scalarRange[0];
      }

    double maxValue;
    if( args.maxValueWasSet )
      {
      maxValue = args.maxValue;
      }
    else
      {
      maxValue = scalarRange[1];
      }

    vtkSmartPointer<vtkImageAccumulate> histogram =
      vtkSmartPointer<vtkImageAccumulate>::New();
    histogram->SetInputConnection( reader->GetOutputPort() );
    histogram->SetComponentExtent(0,args.numBins - 1,0,0,0,0);
    histogram->SetComponentOrigin( minValue, 0, 0 );
    histogram->SetComponentSpacing(( maxValue - minValue ) / static_cast< double >( args.numBins - 1 ),0,0);
    histogram->Update();

    vtkSmartPointer<vtkDoubleArray> bins =
      vtkSmartPointer<vtkDoubleArray>::New();
    bins->SetNumberOfComponents( 1 );
    bins->SetNumberOfTuples( args.numBins );
    bins->SetName( "Bins" );
    vtkSmartPointer<vtkIntArray> frequencies =
      vtkSmartPointer<vtkIntArray>::New();
    frequencies->SetNumberOfComponents(1);
    frequencies->SetNumberOfTuples( args.numBins );
    frequencies->SetName( "Frequency" );
    int* output = static_cast<int*>(histogram->GetOutput()->GetScalarPointer());
    double spacing = histogram->GetComponentSpacing()[0];
    double bin = histogram->GetComponentOrigin()[0];

    for(unsigned int j = 0; j < args.numBins; ++j)
      {
      bins->SetTuple1(j, bin);
      bin += spacing;
      frequencies->SetTuple1(j, *output++);
      }

    vtkSmartPointer<vtkTable> table =
      vtkSmartPointer<vtkTable>::New();
    table->AddColumn( bins );
    table->AddColumn( frequencies );

    vtkSmartPointer<vtkContextView> view =
      vtkSmartPointer<vtkContextView>::New();
    view->GetRenderer()->SetBackground( 0.2, 0.2, 0.2 );
    view->GetRenderWindow()->SetSize( 640, 480 );

    vtkSmartPointer<vtkChartXY> chart =
      vtkSmartPointer<vtkChartXY>::New();
    view->GetScene()->AddItem( chart );
    std::ostringstream title;
    title << "Minimum value = " << scalarRange[0] << "  Maximum value = " << scalarRange[1];
    chart->SetTitle( title.str().c_str() );

    vtkPlot * line = chart->AddPlot( vtkChart::BAR );
    line->SetInput( table, 0, 1 );
    line->GetXAxis()->SetTitle( "Bin" );
    vtkTextProperty *textProp = line->GetXAxis()->GetLabelProperties();
    textProp->SetColor( 1.0, 1.0, 1.0 );
    textProp = line->GetXAxis()->GetTitleProperties();
    textProp->SetColor( 1.0, 1.0, 1.0 );
    line->GetYAxis()->SetTitle( "Count" );
    line->SetColor( 0.1, 0.3, 0.9 );

    vtkSmartPointer<ResetAxesCommand> resetAxesCommand =
      vtkSmartPointer<ResetAxesCommand>::New();
    resetAxesCommand->SetChart( chart );

    vtkRenderWindowInteractor *interactor = view->GetInteractor();
    interactor->AddObserver( vtkCommand::KeyPressEvent, resetAxesCommand );

    view->GetInteractor()->Initialize();
    view->GetInteractor()->Start();
    }
  catch( const std::exception & e )
    {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
    }

  return  EXIT_SUCCESS;
}

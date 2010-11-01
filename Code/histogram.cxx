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
  if( argc < 2 )
    {
    std::cout << "Required arguments: <input_file>" << std::endl;
    return EXIT_FAILURE;
    }

  // Read a jpeg image
  vtkSmartPointer<vtkImageReader2Factory> readerFactory =
    vtkSmartPointer<vtkImageReader2Factory>::New();
  vtkImageReader2 * reader = readerFactory->CreateImageReader2( argv[1] );
  reader->SetFileName( argv[1] );
  reader->Update();
  double* scalarRange = reader->GetOutput()->GetScalarRange();

  vtkSmartPointer<vtkImageAccumulate> histogram =
    vtkSmartPointer<vtkImageAccumulate>::New();
  histogram->SetInputConnection( reader->GetOutputPort() );
  histogram->SetComponentExtent(0,255,0,0,0,0);
  histogram->SetComponentOrigin(scalarRange[0],0,0);
  histogram->SetComponentSpacing((scalarRange[1] - scalarRange[0]) / 255,0,0);
  histogram->Update();

  vtkSmartPointer<vtkDoubleArray> bins =
    vtkSmartPointer<vtkDoubleArray>::New();
  bins->SetNumberOfComponents( 1 );
  bins->SetNumberOfTuples( 256 );
  bins->SetName( "Bins" );
  vtkSmartPointer<vtkIntArray> frequencies =
    vtkSmartPointer<vtkIntArray>::New();
  frequencies->SetNumberOfComponents(1);
  frequencies->SetNumberOfTuples(256);
  frequencies->SetName( "Frequency" );
  int* output = static_cast<int*>(histogram->GetOutput()->GetScalarPointer());
  double spacing = histogram->GetComponentSpacing()[0];
  double bin = histogram->GetComponentOrigin()[0];

  for(int j = 0; j < 256; ++j)
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

  return  EXIT_SUCCESS;
}

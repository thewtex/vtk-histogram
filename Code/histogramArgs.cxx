#include "histogramArgs.h"

#include "vtkmetaio/metaCommand.h"

using namespace vtkmetaio;

Args::Args( int argc, char* argv[] ):
  minValueWasSet( false ),
  maxValueWasSet( false ),
  numBins( 256 )
{
  MetaCommand command;
  command.SetDescription(
    "Display a histogram of a scalar image." );
  command.SetAuthor( "Matthew McCormick" );

  command.AddField( "inputImage", "Input image.", MetaCommand::STRING, MetaCommand::DATA_IN);

  command.SetOption( "minValue", "p", false, "Minimum value in histogram." );
  command.SetOptionLongTag( "minValue", "minimum" );
  command.AddOptionField( "minValue", "minValue", MetaCommand::FLOAT, true );

  command.SetOption( "maxValue", "q", false, "Maximum value in histogram." );
  command.SetOptionLongTag( "maxValue", "maximum" );
  command.AddOptionField( "maxValue", "maxValue", MetaCommand::FLOAT, true );

  command.SetOption( "numBins", "n", false, "Number of bins." );
  command.SetOptionLongTag( "numBins", "bins" );
  command.AddOptionField( "numBins", "numBins", MetaCommand::INT, true );

  if( !command.Parse( argc, argv ) )
    {
    if( command.GotXMLFlag() )
      throw got_xml_flag_exception( "Passed in --xml" );
    else
      throw std::logic_error( "Could not parse command line arguments." );
    }

  if( !command.GetOptionWasSet( "inputImage" ) )
    throw std::runtime_error( "Input image not specified." );
  this->inputImage = command.GetValueAsString( "inputImage" );

  if( command.GetOptionWasSet( "minValue" ) )
    {
    this->minValueWasSet = true;
    this->minValue       = command.GetValueAsFloat( "minValue", "minValue" );
    }

  if( command.GetOptionWasSet( "maxValue" ) )
    {
    this->maxValueWasSet = true;
    this->maxValue       = command.GetValueAsFloat( "maxValue", "maxValue" );
    }

  if( command.GetOptionWasSet( "numBins" ) )
    this->numBins = command.GetValueAsInt( "numBins", "numBins" );
}

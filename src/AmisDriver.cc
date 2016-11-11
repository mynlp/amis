//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: AmisDriver.cc,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#include <amis/AmisDriver.h>
#include <amis/LogStream.h>
#include <amis/Profile.h>
#include <amis/ErrorBase.h>
#include <amis/EventFormat.h>
//#include <amis/EstimatorState.h>
#include <amis/BzStream.h>
#include <amis/ZStream.h>
#include <amis/GzStream.h>
#include <iostream>
#include <fstream>


AMIS_NAMESPACE_BEGIN

#ifdef AMIS_DEBUG_LEVEL
const int AmisDriver::AMIS_DEBUG_TYPE = AMIS_DEBUG_LEVEL;
#else // AMIS_DEBUG_LEVEL
const int AmisDriver::AMIS_DEBUG_TYPE = 0;
#endif // AMIS_DEBUG_LEVEL

#ifdef AMIS_PROFILE_LEVEL
const int AmisDriver::AMIS_PROFILE_TYPE = AMIS_PROFILE_LEVEL;
#else // AMIS_PROFILE_LEVEL
const int AmisDriver::AMIS_PROFILE_TYPE = 0;
#endif // AMIS_PROFILE_LEVEL

//// obsolete
// #ifdef AMIS_FEATURE_WEIGHT_LAMBDA
// const std::string AmisDriver::AMIS_FEATURE_WEIGHT_TYPE( "lambda" );
// #else // AMIS_FEATURE_WEIGHT_LAMBDA
// const std::string AmisDriver::AMIS_FEATURE_WEIGHT_TYPE( "alpha" );
// #endif // AMIS_FEATURE_WEIGHT_LAMBDA

#ifdef AMIS_PARALLEL
const std::string AmisDriver::AMIS_PARALLEL_TYPE( "on" );
#else // AMIS_PARALLEL
const std::string AmisDriver::AMIS_PARALLEL_TYPE( "off" );
#endif // AMIS_PARALLEL

#ifdef AMIS_USE_LONG_DOUBLE
const std::string AmisDriver::AMIS_REAL_TYPE( "long double" );
#else
const std::string AmisDriver::AMIS_REAL_TYPE( "double" );
#endif

//////////////////////////////////////////////////////////////////////

int AmisDriver::run(const std::vector<std::string>& args) {
  try {
    // Read options
    AMIS_DEBUG_MESSAGE( 1, "Input a property file... " );
    importProperty( args );

    // Help
    if ( property.isShowVersion() ) {
      std::cerr << amisName();
      return 0;
    }
    if ( property.isHelp() || property.isHeavyHelp() ) {
      std::cerr << amisName();
      showHelp();
      return 0;
    }

    // Initialization
    initialize();

  } catch ( ErrorBase& err ) {
    std::cerr << amisName();
    std::cerr << err.message() << std::endl;
    std::cerr << "use -h or --help to see the usage" << std::endl;
    showHelp();
    return 1;
  }

  // Print startup messages
  showStartup();
  if ( property.isShowOptionStatus() ) {
    std::cerr << property.getSpecificationStatus();
  }

  // Main program
  try {
    // Dispatch
    estimatorMain();
    // End...

    showEnding();
    finalize();
  } catch ( ErrorBase& err ) {
    AMIS_PROF_MESSAGE( err.message() << '\n' );
    return 1;
  } catch (...) {
    AMIS_PROF_MESSAGE( "uncaught exception\n" );
    std::cerr << std::flush;
    return 1;
  }

  return 0;
}

//////////////////////////////////////////////////////////////////////

void AmisDriver::estimatorMain() {
  ProfTimer total_timer;
  total_timer.startProf();

  // Construct main objects
  AMIS_DEBUG_MESSAGE( 1, "Construct main objects...\n" );
  ModelFormatPtr model_format = newModelFormat();
  EventReaderPtr event_reader = newEventReader();
  NameTablePtr name_table = newNameTable();
  ModelPtr model = newModel();
  model->setNameTable( *name_table );
  EventSpacePtr event_space = newEventSpace();
  //EstimatorStatePtr estimator_state = newEstimatorState();
  EstimatorPtr estimator = newEstimator();
  
  //StatisticsPtr stat = newStatistics();
  //stat->initialize( *model, *event_space );

  estimator->setModel( &(*model) );
  
  AMIS_PROF_MESSAGE( "  Model:      " << model->modelName() << '\n' );
  AMIS_PROF_MESSAGE( "  EventSpace: " << event_space->eventSpaceName() << '\n' );
  AMIS_PROF_MESSAGE( "  Estimator:  " << estimator->estimatorName() << '\n' );
  AMIS_DEBUG_MESSAGE( 1, "done.\n" );
  
  // Import model & event files
  AMIS_DEBUG_MESSAGE( 1, "Initialize a model...\n" );
  model_format->modelInitialize( *name_table, *model );
  inputModel( *model_format, *name_table, *model );
  AMIS_PROF_MESSAGE( "  # features = " << model->numFeatures() << '\n' );
  AMIS_DEBUG_MESSAGE( 1, "done.\n" );
#if AMIS_DEBUG_MODE( 5 )
  model->debugInfo( std::cerr );
#endif // DEBUG_MODE

  AMIS_DEBUG_MESSAGE( 1, "Initialize events...\n" );
  event_reader->setFeatureNameTable(&*name_table);
  inputEventSpace( *event_reader, *event_space );
  AMIS_PROF_MESSAGE( "  # events = " << event_space->numEvents() );
  AMIS_PROF_MESSAGE( "  # feature lists = " << event_space->numFeatureLists() << '\n' );
  AMIS_PROF_MESSAGE( "  event count = " << event_space->sumEventCount() << '\n' );
  AMIS_DEBUG_MESSAGE( 1, "done.\n" );

  ///////////
  //exit(0);

  // Run the estimator
  AMIS_DEBUG_MESSAGE( 1, "Start estimation.\n" );
  estimation( *model, *event_space, *estimator );
  AMIS_DEBUG_MESSAGE( 1, "Done.\n" );
  
  
  ModelPtr     model_to = newModel();
  NameTablePtr name_table_to = newNameTable();
  //StatisticsPtr stat_to = newStatistics();

  model_to = model; //!!!
  name_table_to = name_table;
 
  // Output the model
  model_format->modelInitialize( *name_table_to, *model_to );
  AMIS_PROF_MESSAGE("Output a model... " );
  outputModel( *model_format, *name_table_to, *model_to );
  AMIS_PROF_MESSAGE( "Done.\n" );
  //AMIS_PROF_MESSAGE( "# of nonzero lambdas = " << model->numNonzeroLambdas() << " (out of " << model->numFeatures() << ")" << '\n' );

  total_timer.stopProf();

  AMIS_PROF_MESSAGE( "Total time: " << total_timer.realTimeString() << '\n' );
  AMIS_PROF_MESSAGE( "CPU Usage USER: " << total_timer.userTimeString() << " SYSTEM: " << total_timer.systemTimeString() << std::endl; );

}

//////////////////////////////////////////////////////////////////////

std::string AmisDriver::amisName() const {
  OStringStream os;
  os << AMIS_PACKAGE << " " << AMIS_VERSION << " - A maximum entropy estimator by Yusuke Miyao\n";
  os << "[Debug=" << AMIS_DEBUG_TYPE << "] [Profile=" << AMIS_PROFILE_TYPE << "] [Parallel=" << AMIS_PARALLEL_TYPE << "]" << '\n';
  return os.str();
}

//////////////////////////////////////////////////////////////////////

void AmisDriver::importProperty( const std::vector<std::string>& args )
{
  AMIS_DEBUG_MESSAGE( 1, "Input a property file... " );
  property.parseArguments( args );
  if ( property.isHelp() || property.isHeavyHelp() || property.isShowVersion() ) {
    // don't read property file
    return;
  }

  property.importProperty();
  property.resetToUnspecified();
  property.parseArguments(args);
}

//////////////////////////////////////////////////////////////////////

void AmisDriver::showHelp() {
  if ( property.isHeavyHelp() ) {
    std::cerr << property.getLongHelp();
  } else {
    std::cerr << property.getShortHelp();
  }
  if ( property.isShowOptionStatus() ) {
    std::cerr << std::endl;
    std::cerr << property.getSpecificationStatus();
  }
}

//////////////////////////////////////////////////////////////////////

void AmisDriver::initialize()
{
  // Set up logging
  LogStream::DefaultLogStream.open( property.getLogFile() );
  if ( ! LogStream::DefaultLogStream ) {
    throw AmisDriverError( "Cannot open a log file" );
  }
  LogStream::DefaultLogStream.setSuppressed( property.getSuppressMessage() );
}

//////////////////////////////////////////////////////////////////////


void AmisDriver::showStartup() {
  AMIS_PROF_MESSAGE( amisName() );
  AMIS_DEBUG_MESSAGE( 1, "[Real=" << AMIS_REAL_TYPE << "] (" << sizeof( Real ) << " bytes)" << '\n' ); 
  AMIS_DEBUG_MESSAGE( 1, "[REAL_MAX=" << REAL_MAX << "]" << '\n' );
  AMIS_DEBUG_MESSAGE( 1, "[REAL_MIN=" << REAL_MIN << "]" << '\n' );
  AMIS_DEBUG_MESSAGE( 1, "[REAL_EPSILON=" << REAL_EPSILON << "]" << '\n' );
  AMIS_DEBUG_MESSAGE( 1, "[REAL_INF=" << REAL_INF << "]" << '\n' );
  AMIS_DEBUG_MESSAGE( 1, "[(REAL_INF > 0.0)=" << (REAL_INF > 0.0) << "]\n" );
  AMIS_DEBUG_MESSAGE( 1, "[exp(-REAL_INF) =" << exp( -REAL_INF ) << "]\n" );
  AMIS_PROF_MESSAGE( "  Model files:" );
  std::vector<std::string> model_list = property.getModelFileList();
  std::vector<std::string> event_list = property.getEventFileList();
  for ( std::vector< std::string >::const_iterator m = model_list.begin(); m != model_list.end(); ++m ) {
    AMIS_PROF_MESSAGE( " " << *m );
  }
  AMIS_PROF_MESSAGE( "\n" );
  AMIS_PROF_MESSAGE( "  Event files:" );
  for ( std::vector< std::string >::const_iterator e = event_list.begin(); e != event_list.end(); ++e ) {
    AMIS_PROF_MESSAGE( " " << *e );
  }
  AMIS_PROF_MESSAGE( "\n" );
  AMIS_PROF_MESSAGE( "  Output file: " << property.getOutputFile() << '\n' );
  AMIS_PROF_MESSAGE( std::flush );
  AMIS_DEBUG_MESSAGE( 1, "done.\n" );
}



void AmisDriver::inputModel( ModelFormat& format,
                             NameTable& name_table,
                             ModelBase& model ) {
  std::vector<std::string> model_files = property.getModelFileList();
  for (std::vector<std::string>::iterator file_it = model_files.begin();
       file_it != model_files.end();
       ++file_it) {
    std::auto_ptr< std::istream > model_file = Property::openInputStream( *file_it, property.getModelFileCompression() );
    if ( ! *model_file ) {
      throw( AmisDriverError( "Cannot open input file " + *file_it ) );
    }

    format.inputModel( *model_file, name_table, model );
  }
}

void AmisDriver::inputEventSpace( EventReader& reader,
                                  EventSpace& event_space )
{
  reader.initialize();
  EventFormat format(&reader);

  std::vector<std::string> event_files = property.getEventFileList();
  if ( property.getReferenceDistribution() ) {
    // with reference distribution
    std::vector<std::string> reference_files = property.getReferenceFileList();
    if ( event_files.size() != reference_files.size() ) {
      throw ( AmisDriverError( "The numbers of event and references files are different" ) );
    }
    
    std::vector<std::string>::iterator event_it = event_files.begin();
    std::vector<std::string>::iterator reference_it = reference_files.begin();
    for ( ;
	  event_it != event_files.end();
	  ++event_it, ++reference_it ) {
      std::auto_ptr< std::istream > event_file = Property::openInputStream( *event_it, property.getEventFileCompression() );
      if ( ! *event_file ) {
	throw( AmisDriverError( "Cannot open input file " + *event_it ) );
      }
      std::auto_ptr< std::istream > reference_file = Property::openInputStream( *reference_it, "raw" );
      if ( ! *reference_file ) {
	throw( AmisDriverError( "Cannot open input file " + *reference_it ) );
      }

      format.inputEventSpaceWithProb( *event_file, *reference_file, event_space );
    }
  }
  else {
    // without reference distribution
    for (std::vector<std::string>::iterator file_it = event_files.begin();
	 file_it != event_files.end();
	 ++file_it) {
      std::auto_ptr< std::istream > event_file = Property::openInputStream( *file_it, property.getEventFileCompression() );
      if ( ! *event_file ) {
	throw( AmisDriverError( "Cannot open input file " + *file_it ) );
      }
      format.inputEventSpace( *event_file, event_space );
    }
  }
}


void AmisDriver::estimation( ModelBase& model, EventSpace& event_space, Estimator& estimator )
{
  estimator.setModel( &model );
  estimator.setEventSpace( &event_space );
  //estimator.setEstimatorState( &estimator_state );
  //estimator.setStatistics( model_stat );
  estimator.estimate( property.getNumIterations(), property.getPrecision(), property.getReportInterval() );
}

void AmisDriver::outputModel( const ModelFormat& format,
                              const NameTable& name_table,
                              const ModelBase& model ) {
  std::auto_ptr< std::ostream > output_file = Property::openOutputStream( property.getOutputFile(), property.getOutputFileCompression() );
  if ( ! *output_file ) throw( AmisDriverError( "Cannot open an output file: " + property.getOutputFile() ) );
  format.outputModel( name_table, model, *output_file, property.getPrecision() , property.getFilterInactiveFeatures() );
}

void AmisDriver::finalize() {
  LogStream::DefaultLogStream.close();
}

void AmisDriver::showEnding() {
  AMIS_PROF_MESSAGE( Profile::profileResult() );
}

AMIS_NAMESPACE_END

// end of AmisDriver.cc

//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: Property.cc,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#include <amis/Property.h>
#include <amis/Tokenizer.h>
#include <amis/ArgumentParser.h>
#include <GzStream.h>
#include <BzStream.h>
#include <ZStream.h>

AMIS_NAMESPACE_BEGIN

// Constants
const std::string Property::DEFAULT_PROPERTY_FILE( "amis.conf" );

// Static members
Initializer< OptionManager* >* PropertyItemBase::OPTION_QUEUE = NULL;
OptionManager Property::option_manager( PropertyItemBase::OPTION_QUEUE );
bool Property::initialized = false;

//////////////////////////////////////////////////////////////////////

// Launchers

DataFormatLauncher Property::data_format_launcher;
//NameTableLauncher Property::name_table_launcher;
ModelLauncher Property::model_launcher;
EventSpaceLauncher Property::event_space_launcher;
ModelFormatLauncher Property::model_format_launcher;
EventReaderLauncher Property::event_reader_launcher;
EstimatorLauncher Property::estimator_launcher;
//EstimatorStateLauncher Property::estimator_state_launcher;
GaussianPriorLauncher Property::gaussian_prior_launcher;
//StatisticsLauncher Property::statistics_launcher;


//////////////////////////////////////////////////////////////////////

// Options

PropertyItemToggle Property::help( "", "", "-h", false, "Print help messages", true );
PropertyItemToggle Property::heavy_help( "", "--help", "-hh", false, "Print detailed help messages", true );
PropertyItemToggle Property::show_option_status( "", "--show-option-status", "-sos", false, "Show option status", true );
PropertyItemToggle Property::show_version( "", "--version", "-v", false, "Show version", true );
PropertyItemToggle Property::make_init_model( "MAKE_INIT_MODEL", "--make-init-model", "", false, "Make init model", true );
PropertyItem< bool > Property::make_init_model_make_all( "MAKE_INIT_MODEL_MAKE_ALL", "--make-init-model-make-all", "", true, "Make all features possible from given events (including zero-count features)\n", true );


PropertyItem< std::string > Property::feature_type( "FEATURE_TYPE", "--feature-type", "-f", "binary", "The type of features" );

class FeatureWeightHash : public StringHash< FeatureWeightType > {
public:
  FeatureWeightHash( void ) {
    (*this)[ "alpha" ] = ALPHA;
    (*this)[ "lambda" ] = LAMBDA;
  }
};
namespace { FeatureWeightHash feature_weight_hash; };
PropertyItemNamedInt< FeatureWeightType > Property::feature_weight_type( "FEATURE_WEIGHT_TYPE", "--feature-weight-type", "-w", feature_weight_hash, ALPHA, "The type of weights of features" );

PropertyItem< std::string > Property::root_dir( "", "--root-dir", "-R", ".", "Root directory" );
PropertyItem< std::string > Property::data_dir( "DATA_DIR", "--data-dir", "-D", ".", "Data directory" );
PropertyItemVector< std::string > Property::model_file_list( "MODEL_FILE", "--model-file", "-m", std::vector< std::string >( 1, "amis.model" ), "Model file name(s)" );
PropertyItemVector< std::string > Property::event_file_list( "EVENT_FILE", "--event-file", "-e", std::vector< std::string >( 1, "amis.event" ), "Event file name(s)" );
PropertyItemVector< std::string > Property::fixmap_file_list( "FIXMAP_FILE", "--fixmap-file", "-x", std::vector< std::string >( 1, "amis.fixmap" ), "Mapper for AmisFix format" );
PropertyItemVector< std::string > Property::transition_file_list( "TRANSITION_FILE", "--transition-file", "-t", std::vector< std::string >( 1, "amis.trans" ), "Transition Table for AmisCRF Format" );
//PropertyItem< bool > Property::event_file_bz2( "EVENT_FILE_BZ2", "--event-file-bz2", "", false, "Event files are provided as compressed (bzip2) files" );
PropertyItem< std::string > Property::event_file_compression( "EVENT_FILE_COMPRESSION", "--event-file-compression", "", "none", "Compression format of event file [raw, gz, z, bz2]" );
PropertyItem< std::string > Property::model_file_compression( "MODEL_FILE_COMPRESSION", "--model-file-compression", "", "none", "Compression format of model file [raw, gz, z, bz2]" );
PropertyItem< std::string > Property::output_file_compression( "OUTPUT_FILE_COMPRESSION", "--output-file-compression", "", "none", "Compression format of output file [raw, gz, z, bz2]" );
PropertyItem< std::string > Property::fixmap_file_compression( "FIXMAP_FILE_COMPRESSION", "--fixmap-file-compression", "", "none", "Compression format of fixmap file [raw, gz, z, bz2]" );
PropertyItem< std::string > Property::transition_file_compression( "TRANSITION_FILE_COMPRESSION", "--transition-file-compression", "", "none", "Compression format of transition file [raw, gz, z, bz2]" );
PropertyItemVector< std::string > Property::reference_file_list( "REFERENCE_FILE", "--reference-file", "", std::vector< std::string >( 1, "amis.ref" ), "Reference distribution file name(s)" );
PropertyItem< std::string > Property::output_file( "OUTPUT_FILE", "--output-file", "-o", "amis.output", "The name of the output file" );
PropertyItem< std::string > Property::prob_file( "PROB_FILE", "--prob-file", "-b", "amis.prob", "The name of the file of probability output" );
PropertyItem< std::string > Property::log_file( "LOG_FILE", "--log-file", "-l", "amis.log", "The name of the log file" );
PropertyItem< std::string > Property::statistics_file( "STATISTICS_FILE", "--statistics-file", "", "amis.stat", "The name of the file of the statistics of features" );
PropertyItem< std::string > Property::data_format( "DATA_FORMAT", "--data-format", "-d", "Amis", "Data format type (Amis, AmisTree, AmisFix)" );
PropertyItem< std::string > Property::estimation_algorithm( "ESTIMATION_ALGORITHM", "--estimation-algorithm", "-a", "GIS", "Estimation algorithm (GIS, GISMAP, BFGS, BFGSMAP, BLMVMBC, BLMVMBCMAP)" );
PropertyItem< std::string > Property::parameter_type( "PARAMETER_TYPE", "--parameter-type", "", "alpha", "Internal parameter type (alpha, lambda)" );
PropertyItem< int > Property::num_iterations( "NUM_ITERATIONS", "--num-iterations", "-i", 200, "The number of iterations" );
PropertyItem< int > Property::report_interval( "REPORT_INTERVAL", "--report-interval", "-r", 1, "The interval of interation report" );
PropertyItem< int > Property::prec( "PRECISION", "--precision", "-p", 6, "The precision of the estimation" );
PropertyItem< bool > Property::event_on_file( "EVENT_ON_FILE", "--event-on-file", "", false, "Store events on file during the estimation" );
PropertyItem< std::string > Property::event_on_file_name( "EVENT_ON_FILE_NAME", "--event-on-file-name", "", "amis.event.tmp", "The name of temp event file used by EVENT_ON_FILE" );
PropertyItem< int > Property::num_threads( "NUM_THREADS", "--num-threads", "-j", 1, "The level of parallelism of estimation" );
PropertyItem< bool > Property::till_convergence( "TILL_CONVERGENCE", "--till-convergence", "-tc", false, "Run estimation till convergence" );
PropertyItem< bool > Property::suppress_message( "SUPPRESS_MESSAGE", "--suppress-message", "", false, "Suppress profiling messages" );
PropertyItem< std::string > Property::variance_type( "VARIANCE_TYPE", "--variance-type", "", "single", "Type of the variances used for MAP estimation" );
PropertyItem< bool > Property::write_statistics( "WRITE_STATISTICS", "--write-statistics", "", false, "Output various kinds of statistics" );
PropertyItem< bool > Property::write_heavy_statistics( "WRITE_HEAVY_STATISTICS", "--write-heavy-statistics", "", false, "Output various kind of \"heavy\" statistics, which typically involve the value for each feature." );
PropertyItem< bool > Property::reference_distribution( "REFERENCE_DISTRIBUTION", "--reference", "", false, "Use reference probability distribution" );
PropertyItem< bool > Property::filter_inactive_features( "FILTER_INACTIVE_FEATURES", "--filter-inactive-features", "", false, "Discard inactive features" );

//PropertyItemToggle Property::do_reestimation( "DO_REESTIMATION", "--do-reestimation", "", false, "Perform estimation once more (intended to be used with --do-model-transform. That is, after the first estimation is completed, the model is transformed, for example by cut-offing features, and then the model is reestimatited)" );

//////////////////////////////////////////////////////////////////////

void Property::parseArguments(const std::vector<std::string>& args) throw( IllegalPropertyError ) {
  try {
    ArgumentParser arguments(args);
    bool property_file_initialized = false;
    while ( ! arguments.empty() ) {
      std::string arg = arguments.nextArgument( "Too few arguments" );
      if ( arg[0] != '-' ) {
        if ( property_file_initialized ) {
          throw IllegalPropertyError( "More than one arguments specified: " + arg );
        }
        property_file = arg;
        property_file_initialized = true;
      } else {
        OptionBase* option = option_manager.findOptionLongName( arg );
        if ( option == NULL ) {
          option = option_manager.findOptionShortName( arg );
          if ( option == NULL ) {
            throw IllegalPropertyError( "Unknown command-line option found: " + arg );
          }
        }
        int num_args = option->getNumArguments();
        if ( num_args < 0 ) {
          if ( arguments.empty() ) {
            throw IllegalPropertyError( "Arguments of the option not found: " + arg );
          }
          std::string val = arguments.nextArgument( "Too few arguments for the command-line option: " + arg );
          option->setValueFromString( val );
        } else if ( num_args == 0 ) {
          option->setValueFromString( "" );
        } else {
          for ( int i = 0; i < num_args; i++ ) {
            if ( arguments.empty() ) {
              OStringStream os;
              os << option->getNameDescription() << " requires " << num_args << " arguments";
              throw IllegalPropertyError( os.str() );
            }
            arg = arguments.nextArgument( "Too few arguments for the command-line option: " + arg );
            option->setValueFromString( arg );
          }
        }
      }
    }
  } catch ( IllegalArgumentError& err ) {
    throw IllegalPropertyError( err.message() );
  } catch ( IllegalOptionError& err ) {
    throw IllegalPropertyError( err.message() );
  }
}

void Property::importProperty( void ) throw( IllegalPropertyError ) {
  try {
    //std::ifstream pf( property_file.c_str() );
    std::string pfname = getPropertyFile();
    std::ifstream pf( pfname.c_str() );
    if ( ! pf ) {
      throw IllegalPropertyError( "Can't open property file: " + pfname );
    }
    option_manager.resetToInit();

    Tokenizer t( pf );
    std::string attr, value, dummy;
    while ( ! t.endOfStream() ) {
      if ( ! t.nextToken( attr ) ) continue;  // empty line
      OptionBase* option = option_manager.findOption( attr );
      if ( option == NULL ) {
        throw IllegalPropertyError( "Unknown property: " + attr );
      }
      int num_args = option->getNumArguments();
      if ( num_args < 0 ) {
        while( t.nextToken( value ) ) {
          option->setValueFromString( value );
        }
      } else if ( num_args == 0 ) {
        // toggle-type property
        // Allow the following way
        // PROP1   ==> !default_val
        // PROP1 true  ==> true
        // PROP2 false ==> false
        option->setValueFromString( "" );
        while ( t.nextToken( value ) ) {
          option->setValueFromString( value );
        }
      } else {
        for ( int i = 0; i < num_args; i++ ) {
          if ( ! t.nextToken( value ) ) {
            OStringStream os;
            os << "Property: " << attr << " requires " << num_args << " arguments.";
            throw IllegalPropertyError( os.str() );
          }
          option->setValueFromString( value );
        }
      }
    }
  } catch ( IllegalOptionError err ) {
    throw IllegalPropertyError( err.message() );
  }
}

std::auto_ptr< std::istream > Property::openInputStream( const std::string& file_name, const std::string& compress = "none" )
{
  std::auto_ptr< std::istream > ret;
  
  if (file_name == "-") {
    if ( compress == "bz2" ) {
      ret = std::auto_ptr< std::istream >( new IBzStream( std::cin ) );
    }
    else if ( compress == "z" ) {
      ret = std::auto_ptr< std::istream >( new IZStream( std::cin ) );
    }
    else if ( compress == "gz" ) {
      ret = std::auto_ptr< std::istream >( new IGzStream( std::cin ) );
    }
    else if ( compress == "none" || compress == "raw" ) {
      // must be fixed!!
      ret = std::auto_ptr< std::istream >( new std::istream( std::cin.rdbuf() ) );
    }
    else {
      throw( IllegalPropertyError( "Unknown compression format: " + compress ) );
    }
  }
  else {
    if ( compress == "bz2" ) {
      ret = std::auto_ptr< std::istream >( new IBzFStream( file_name ) );
    }
    else if ( compress == "z" ) {
      ret = std::auto_ptr< std::istream >( new IZFStream( file_name ) );
    }
    else if ( compress == "gz" ) {
      ret = std::auto_ptr< std::istream >( new IGzFStream( file_name ) );
    }
    else if ( compress == "none" || compress == "raw" ) {
      ret = std::auto_ptr< std::istream >( new std::ifstream( file_name.c_str() ) );
    }
    else {
      throw( IllegalPropertyError( "Unknown compression format: " + compress ) );
    }
  }

  return ret;
}

std::auto_ptr< std::ostream > Property::openOutputStream( const std::string& file_name, const std::string& compress )
{
  if ( compress == "bz2" ) {
    return std::auto_ptr< std::ostream >( new OBzFStream( file_name ) );
  }
  else if ( compress == "z" ) {
    return std::auto_ptr< std::ostream >( new OZFStream( file_name ) );
  }
  else if ( compress == "gz" ) {
    return std::auto_ptr< std::ostream >( new OGzFStream( file_name ) );
  }
  else if ( compress == "none" || compress == "raw" ) {
    return std::auto_ptr< std::ostream >( new std::ofstream( file_name.c_str() ) );
  }
  else {
    throw( IllegalPropertyError( "Unknown compression format: " + compress ) );
  }
}


AMIS_NAMESPACE_END

// end of Property.cc

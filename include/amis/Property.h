//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: Property.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_Property_h_

#define Amis_Property_h_

#include <amis/configure.h>
#include <amis/PropertyItem.h>
//#include <amis/NameTableLauncher.h>
#include <amis/ModelLauncher.h>
#include <amis/EventSpaceLauncher.h>
#include <amis/ModelFormatLauncher.h>
#include <amis/EventReaderLauncher.h>
#include <amis/EstimatorLauncher.h>
#include <amis/GaussianPriorLauncher.h>
//#include <amis/StatisticsLauncher.h>
//#include <amis/EstimatorStateLauncher.h>
#include <amis/DataFormat.h>
#include <amis/ErrorBase.h>
#include <amis/objstream.h>
#include <iostream>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/// <classdef>
/// <name>IllegalPropertyError</name>
/// <overview>Exception for illegal properties</overview>
/// <desc>
/// The class signals illegal property specifications in
/// command-line arguments and in a property file
/// </desc>
/// <body>

class IllegalPropertyError : public ErrorBase {
 public:
  IllegalPropertyError( const std::string& m ) : ErrorBase( m ) {}
  /// Initialize with an error message
  IllegalPropertyError( const char* m ) : ErrorBase( m ) {}
  /// Initialize with an error message
};

/// </body>
/// </classdef>

//////////////////////////////////////////////////////////////////////

/// <classdef>
/// <name>Property</name>
/// <overview>Program property manager</overview>
/// <desc>
/// The class manages program properties
/// specified with program arguments and property files
/// </desc>
/// <body>

class Property {
public:
  static const std::string DEFAULT_PROPERTY_FILE;
  
  static PropertyItem< bool> make_init_model_make_all;
  
private:
  static OptionManager option_manager;
  static bool initialized;

  static PropertyItemToggle help;
  static PropertyItemToggle heavy_help;
  static PropertyItemToggle show_option_status;
  static PropertyItemToggle show_version;
  static PropertyItemToggle  make_init_model;
  
  
  static PropertyItem< std::string > feature_type;
  static PropertyItemNamedInt< FeatureWeightType > feature_weight_type;
  static PropertyItem< std::string > root_dir;
  static PropertyItem< std::string > data_dir;
  static PropertyItemVector< std::string > model_file_list;
  static PropertyItemVector< std::string > event_file_list;
  static PropertyItemVector< std::string > fixmap_file_list;
  static PropertyItemVector< std::string > transition_file_list;
  
  //static PropertyItem< bool > event_file_bz2;
  static PropertyItem< std::string > event_file_compression;
  static PropertyItem< std::string > model_file_compression;
  static PropertyItem< std::string > output_file_compression;
  static PropertyItem< std::string > fixmap_file_compression;
  static PropertyItem< std::string > transition_file_compression;

  static PropertyItemVector< std::string > reference_file_list;
  static PropertyItem< std::string > output_file;
  static PropertyItem< std::string > prob_file;
  static PropertyItem< std::string > log_file;
  static PropertyItem< std::string > statistics_file;
  static PropertyItem< std::string > data_format;
  static PropertyItem< std::string > estimation_algorithm;
  static PropertyItem< std::string > parameter_type;
  static PropertyItem< int > num_iterations;
  static PropertyItem< int > report_interval;
  static PropertyItem< int > prec;
  static PropertyItem< bool > event_on_file;
  static PropertyItem< std::string > event_on_file_name;
  static PropertyItem< int > num_threads;
  static PropertyItem< bool > till_convergence;
  static PropertyItem< bool > suppress_message;
  static PropertyItem< std::string > variance_type;
  static PropertyItem< bool > write_statistics;
  static PropertyItem< bool > write_heavy_statistics;
  static PropertyItem< bool > reference_distribution; 
  static PropertyItem< bool > filter_inactive_features; 
  static PropertyItemToggle   do_reestimation; // do re-estimation
  
  static DataFormatLauncher    data_format_launcher;
  static ModelLauncher         model_launcher;
  static EventSpaceLauncher    event_space_launcher;
  static ModelFormatLauncher   model_format_launcher;
  //static EstimatorStateLauncher   estimator_state_launcher;
  static EventReaderLauncher   event_reader_launcher;
  static EstimatorLauncher     estimator_launcher;
  static GaussianPriorLauncher gaussian_prior_launcher;
  //static StatisticsLauncher    statistics_launcher;
  
  std::string property_file;
  objstream* event_on_file_stream;
  
  public:
  Property() {
    property_file = DEFAULT_PROPERTY_FILE;
    event_on_file_stream = NULL;
    if ( ! initialized ) {
      option_manager.initAllOptions();
      data_format_launcher.initLauncherItems();
      model_launcher.initLauncherItems();
      event_space_launcher.initLauncherItems();
      model_format_launcher.initLauncherItems();
      event_reader_launcher.initLauncherItems();
      estimator_launcher.initLauncherItems();
      //estimator_state_launcher.initLauncherItems();
      gaussian_prior_launcher.initLauncherItems();
      //statistics_launcher.initLauncherItems();
      initialized = true;
    }
  }
  virtual ~Property() {
    if ( event_on_file_stream != NULL ) {
      delete event_on_file_stream;
    }
    event_on_file_stream = NULL;
  }

  const OptionManager& getOptionManager() const {
    return option_manager;
  }

public:
  // Import properties
  void parseArguments(const std::vector<std::string>& args) throw( IllegalPropertyError );
  void importProperty() throw( IllegalPropertyError );
  void resetToUnspecified() {
    option_manager.resetToUnspecified();
  }

public:
  // Help messages
  std::string getShortHelp() const {
    OStringStream os;
    os << "Usage: amis [list of options] [property file]\n";
    os << option_manager.getShortDescription();
    return os.str();
  }
  std::string getLongHelp() const {
    OStringStream os;
    os << "Usage: amis [list of options] [property file]\n";
    os << option_manager.getFullDescription();
    os << " [registered data formats]\n";
    data_format_launcher.showLauncherItems( os );
    os << "\n";
    os << " [registered models]\n";
    model_launcher.showLauncherItems( os );
    os << "\n";
    os << " [registered event spaces]\n";
    event_space_launcher.showLauncherItems( os );
    os << "\n";
    os << " [registered model formats]\n";
    model_format_launcher.showLauncherItems( os );
    os << "\n";
    os << " [registered event formats]\n";
    event_reader_launcher.showLauncherItems( os );
    os << "\n";
    os << " [registered estimators]\n";
    estimator_launcher.showLauncherItems( os );
    os << "\n";
    /*os << " [registered statistics]\n";
      statistics_launcher.showLauncherItems( os );
      os << "\n";*/
    return os.str();
  }
  std::string getSpecificationStatus() const {
    OStringStream os;
    os << "Specified values of the options\n";
    os << option_manager.getSpecificationStatus();
    return os.str();
  }

protected:
  std::string prefixByDir( const std::string& dir, const std::string& s ) const {
    if ( dir == "." || dir == "" ) {
      // current directory
      return s;
    }
    if ( s[ 0 ] == '/' ) {
      // absolute path
      return s;
    }
    if ( s == "stdin" || s == "stdout" || s == "cin" || s == "-" ) {
      // standard input/output
      return s;
    }
    return dir + "/" + s;
  }
  std::string prefixByDataDir( const std::string& s ) const {
    return prefixByDir( getDataDir(), s );
  }
  std::string prefixByRootDir( const std::string& s ) const {
    return prefixByDir( getRootDir(), prefixByDataDir( s ) );
  }
  
  std::vector< std::string > prefixByRootDir( const std::vector< std::string >& v ) const {
    std::vector< std::string > ret;
    for ( std::vector< std::string >::const_iterator itr = v.begin(); itr != v.end(); ++itr ) {
      ret.push_back( prefixByRootDir( *itr ) );
    }
    return ret;
  }

public:
  // Accessors of options
  bool isHelp() const {
    return help.getValue();
  }
  bool isHeavyHelp() const {
    return heavy_help.getValue();
  }
  bool isShowOptionStatus() const {
    return show_option_status.getValue();
  }
  bool isShowVersion() const {
    return show_version.getValue();
  }
  bool isMakeInitModel() {
	  return make_init_model.getValue();
  }
  
  bool doReestimation() {
    return do_reestimation.getValue();  
  }
  
  const std::string& getFeatureType() const {
    return feature_type.getValue();
  }
  FeatureWeightType getFeatureWeightType() const {
    return feature_weight_type.getValue();
  }
  std::string getRootDir() const {
    return root_dir.getValue();
  }
  std::string getDataDir() const {
    return data_dir.getValue();
  }
  
  std::string getPropertyFile() const {
    // prefix by only root directory
    return prefixByDir( getRootDir(), property_file );
  }	  
  
  std::vector< std::string > getModelFileList() const {
    return prefixByRootDir( model_file_list.getValue() );
  }
  std::vector< std::string > getEventFileList() const {
    return prefixByRootDir( event_file_list.getValue() );
  }

  std::vector< std::string > getFixMapFileList() const {
    return prefixByRootDir( fixmap_file_list.getValue() );
  }

  std::vector< std::string > getTransitionFileList() const {
    return prefixByRootDir( transition_file_list.getValue() );
  }

//   bool getEventFileBz2() const {
//     return event_file_bz2.getValue();
//   }
  std::string getEventFileCompression() const {
    return event_file_compression.getValue();
  }
  std::string getModelFileCompression() const {
    return model_file_compression.getValue();
  }
  std::string getOutputFileCompression() const {
    return output_file_compression.getValue();
  }
  std::string getFixMapFileCompression() const {
    return fixmap_file_compression.getValue();
  }
  std::string getTransitionFileCompression() const {
    return transition_file_compression.getValue();
  }




  std::vector< std::string > getReferenceFileList() const {
    return prefixByRootDir( reference_file_list.getValue() );
  }
  
  std::string getOutputFile() const {
    return prefixByRootDir( output_file.getValue() );
  }
  std::string getProbFile() const {
    return prefixByRootDir( prob_file.getValue() );
  }
  std::string getLogFile() const {
    return prefixByRootDir( log_file.getValue() );
  }
  
  static std::auto_ptr< std::istream > openInputStream( const std::string& file_name, const std::string& compress );
  /// Open output stream
  static std::auto_ptr< std::ostream > openOutputStream( const std::string& file_name, const std::string& compress );


  /*std::string getStatisticsFile() const {
    return prefixByRootDir( statistics_file.getValue() );
    }*/
  
  
  const std::string& getDataFormat() const {
    return data_format.getValue();
  }
  
  const std::string getNameTableType() const throw( IllegalPropertyError ) {
    try {
      return data_format_launcher.launch( getDataFormat(), this ).nameTableType();
    } catch ( IllegalLauncherItemError& ) {
      throw IllegalPropertyError( "Unknown data format: " + getDataFormat() );
    }
  }

  const std::string getModelType() const throw( IllegalPropertyError ) {
    try {
      return data_format_launcher.launch( getDataFormat(), this ).modelType();
    } catch ( IllegalLauncherItemError& ) {
      throw IllegalPropertyError( "Unknown data format: " + getDataFormat() );
    }
  }
  
  const std::string getEventType() const throw( IllegalPropertyError ) {
    try {
      return data_format_launcher.launch( getDataFormat(), this ).eventType();
    } catch ( IllegalLauncherItemError& ) {
      throw IllegalPropertyError( "Unknown data format: " + getDataFormat() );
    }
  }

   
  const std::string& getParameterType() const throw( IllegalPropertyError ) {
    return parameter_type.getValue();
  }
  const std::string& getEstimationAlgorithm() const {
    return estimation_algorithm.getValue();
  }
  int getNumIterations() const {
    return num_iterations.getValue();
  }
  int getReportInterval() const {
    return report_interval.getValue();
  }
  int getPrecision() const {
    return prec.getValue();
  }
  bool isEventOnFile() const {
    return event_on_file.getValue();
  }
  objstream* eventOnFileStream() throw( IllegalPropertyError ) {
    if ( event_on_file_stream != NULL ) delete event_on_file_stream;
    std::string tmp_file = prefixByRootDir( event_on_file_name.getValue() );
    event_on_file_stream = new objstream( tmp_file );
    if ( ! *event_on_file_stream ) {
      throw IllegalPropertyError( "Cannot open temporary file: " + tmp_file );
    }
    return event_on_file_stream;
  }
  int getNumThreads() const {
    return num_threads.getValue();
  }
  bool getTillConvergence() const {
    return till_convergence.getValue();
  }
  bool getSuppressMessage() const {
    return suppress_message.getValue();
  }
  const std::string& getVarianceType() const {
    return variance_type.getValue();
  }
  /*bool getWriteStatistics() const {
    return write_statistics.getValue();
  }
  bool getWriteHeavyStatistics() const {
    return write_heavy_statistics.getValue(); 
    }*/
  
  bool getReferenceDistribution() {
    return reference_distribution.getValue();
  }

  bool getFilterInactiveFeatures() {
    return filter_inactive_features.getValue();
  }
  
public:
  virtual NameTablePtr newNameTable() throw( IllegalPropertyError ) {
    try {
      return NameTablePtr(new NameTable());
      //return name_table_launcher.launch( getNameTableType(), this );
    }
    catch ( IllegalLauncherItemError& ) {
      throw IllegalPropertyError( "Unknown data format: " + getNameTableType() );
    }
  }
  virtual ModelPtr newModel() throw( IllegalPropertyError ) {
    try {
      return model_launcher.launch( ModelIdentifier(getModelType(), getParameterType()), this );
    } catch ( IllegalLauncherItemError& ) {
      throw IllegalPropertyError( "Unknown model: " + getModelType() );
    }
  }
  virtual EventSpacePtr newEventSpace() throw( IllegalPropertyError ) {
    try {
      if ( isEventOnFile() ) {
	objstream* os = eventOnFileStream();
	return EventSpacePtr( new EventSpaceOnFile( os ) );
      }
      else {
	return EventSpacePtr( new EventSpaceOnMemory() );
      }
    }
    catch ( IllegalLauncherItemError& ) {
      throw IllegalPropertyError( "Unknown event space: " + getEventType() + ", " + getFeatureType() );
    }
  }
  virtual ModelFormatPtr newModelFormat() throw( IllegalPropertyError ) {
    try {
      return model_format_launcher.launch( getModelType(), this );
    } catch ( IllegalLauncherItemError& ) {
      throw IllegalPropertyError( "Unknown model format: " + getModelType() );
    }
  }
  virtual EventReaderPtr newEventReader() throw( IllegalPropertyError ) {
    try {
      return event_reader_launcher.launch( EventReaderIdentifier( getDataFormat(),
								  getFeatureType(),
								  getParameterType(),
								  getReferenceDistribution() ),
					   this );
    }
    catch ( IllegalLauncherItemError& ) {
      throw IllegalPropertyError( "Unknown event format: " + getEventType() + ", " + getFeatureType() );
    }
  }
  /*virtual EstimatorStatePtr newEstimatorState() throw( IllegalPropertyError ) {
    try {
      return estimator_state_launcher.launch( EstimatorStateIdentifier(getModelType(), 
								       getEventType(),
								       getFeatureType(), 
								       getParameterType(), 
								       getReferenceDistribution() ),
						     this );
    } catch ( IllegalLauncherItemError& ) {
      throw IllegalPropertyError( "Unknown event format: " + getEventType() + ", " + getFeatureType() );
    }
    }*/
  virtual EstimatorPtr newEstimator() throw( IllegalPropertyError ) {
    try {
      EstimatorPtr estimator =
      estimator_launcher.launch( EstimatorIdentifier( getEstimationAlgorithm() ),
				 this );
              estimator->setSuppressMessage( getSuppressMessage() );
              estimator->setTillConvergence( getTillConvergence() );
              return estimator;
    } catch ( IllegalLauncherItemError& ) {
      std::string args( getFeatureType() + ',' + getModelType() + ',' + getEventType() );
      if ( getReferenceDistribution() ) args += ",Reference";
      throw IllegalPropertyError( "Unknown estimator: " + getEstimationAlgorithm() + '<' + args + '>' );
    }
  }
  virtual GaussianPriorPtr newGaussianPrior() throw( IllegalPropertyError ) {
    try {
      return gaussian_prior_launcher.launch( getVarianceType(), this );
    } catch ( IllegalLauncherItemError& ) {
      throw IllegalPropertyError( "Unknown Gaussian prior: " + getVarianceType() );
    }
  }
  
  /*virtual StatisticsPtr newStatistics() throw( IllegalPropertyError ) {
    try {
      return statistics_launcher.launch( StringTriple( getModelType(), getEventType(), getFeatureType() ), this );
    } catch ( IllegalLauncherItemError& ) {
      throw IllegalPropertyError( "Unknown statistics format: " + getModelType() + ',' + getEventType() + ',' + getFeatureType() );
    }
    }*/
};

/// </body>
/// </classdef>

AMIS_NAMESPACE_END

#endif // Amis_Property_h_
// end of Property.h

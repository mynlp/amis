//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: AmisDriver.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_AmisDriver_h_
#define Amis_AmisDriver_h_

#include <amis/configure.h>
#include <amis/Property.h>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/**
 * Notification of an error in AmisDriver
 */

class AmisDriverError : public ErrorBase {
public:
  /// Initialize with a message
  AmisDriverError( const std::string& m ) : ErrorBase( m ) {}
  /// Initialize with a error message
  AmisDriverError( const char* m ) : ErrorBase( m ) {}
  /// Destructor
  virtual ~AmisDriverError() {}
};

//////////////////////////////////////////////////////////////////////

/**
 * User interface for running amis.
 * Users can easily run estimator by using this class.
 */

class AmisDriver {
public:
  /// Debug level of this amis
  static const int AMIS_DEBUG_TYPE;
  /// Profile level of this amis
  static const int AMIS_PROFILE_TYPE;
  /// Feature weights (alpha/lambda) of this amis
  //static const std::string AMIS_FEATURE_WEIGHT_TYPE;
  /// Parallel or not
  static const std::string AMIS_PARALLEL_TYPE;
  /// "Real" is double or long double
  static const std::string AMIS_REAL_TYPE;
  /// Use TAO or not
  //static const std::string AMIS_USE_TAO_TYPE;

  //////////////////////////////////////////////////////////////////////

private:
  Property property;

public:
  /// Constructor
  AmisDriver() {}
  /// Destructor
  virtual ~AmisDriver() {}

public:
  /// Get the name, version and status of amis
  std::string amisName() const;
  /// Get the property used by this AmisDriver
  const Property& getProperty() { return property; }

  //////////////////////////////////////////////////////////////////////

public:
  /// Main routine to run amis (using the following functions)
  virtual int run(const std::vector<std::string>& args);

  /// Main routine for model transformation
  void modelTransform( ModelBase& model_from,  NameTable& name_table_from, StatisticsBase& model_stat,
                       ModelBase& model_to, NameTable& name_table_to );

  /// Main routine for estimator
  virtual void estimatorMain();

  /// Parse command-line arguments and import a property file
  virtual void importProperty(const std::vector<std::string>& args);
  /// Initialize amis
  virtual void initialize();
  /// Show startup messages
  virtual void showStartup();
  /// Show help messages
  virtual void showHelp();

  /// Make a new name table specified by the property
  virtual NameTablePtr newNameTable() {
    NameTablePtr name_table = property.newNameTable();
    return name_table;
  }

  /// Make a new model specified by the property
  virtual ModelPtr newModel() {
    ModelPtr model = property.newModel();
    return model;
  }
  /// Make a new event space specified by the property
  virtual EventSpacePtr newEventSpace() {
    EventSpacePtr event_space = property.newEventSpace();
    return event_space;
  }
  /// Make a new model format specified by the property
  virtual ModelFormatPtr newModelFormat() {
    ModelFormatPtr model_format = property.newModelFormat();
    return model_format;
  }
  /// Make a new event format specified by the property
  virtual EventReaderPtr newEventReader() {
    EventReaderPtr event_reader = property.newEventReader();
    return event_reader;
  }
  /// Make a new estimator state specified by the property
  /*virtual EstimatorStatePtr newEstimatorState() {
    EstimatorStatePtr estimator_state = property.newEstimatorState();
    return estimator_state;
    }*/
  /// Make a new estimator specified by the property
  virtual EstimatorPtr newEstimator() {
    EstimatorPtr estimator = property.newEstimator();
    return estimator;
  }

  /// Make a new statistics specified by the property
  /*virtual StatisticsPtr newStatistics() {
    StatisticsPtr model_stat = property.newStatistics();
    return model_stat;
    }*/
  /// Open input stream

  /// Input a model by using the model format
  virtual void inputModel( ModelFormat& format, NameTable& name_table, ModelBase& model );
  /// Input events by using the event format
  virtual void inputEventSpace( EventReader& format, EventSpace& event_space );
  
  /// Input events by using the event format while making a model

  /// Start estimation
  virtual void estimation( ModelBase& model, EventSpace& event_space, Estimator& estimator );

  /// Output the estimated model
  virtual void outputModel( const ModelFormat& format, const NameTable& name_table, const ModelBase& model );
  /// Output the statistics of the model and event space (not implemented yet)
  //  virtual void outputStatistics( const ModelBase& model, const StatisticsBase& model_stat,
  //const ProfTimer& total_timer, const ProfTimer& estimation_timer );

  /// Finalize the data
  virtual void finalize();
  /// Show ending messages
  virtual void showEnding();
};

AMIS_NAMESPACE_END

#endif // Amis_AmisDriver_h_
// end of AmisDriver.h

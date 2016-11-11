//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: Estimator.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_Estimator_h_

#define Amis_Estimator_h_

#include <amis/configure.h>

#include <amis/Real.h>
#include <amis/ModelBase.h>
#include <amis/EventSpace.h>

#include <amis/Profile.h>
#include <amis/LogStream.h>

#include <iomanip>
#include <iostream>
#include <memory>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/**
 * Error for failing initialization of an estimator.
 * The error is thrown when an initialization of an estimator failed.
 */

class EstimatorInitError : public ErrorBase {
 public:
  /// Initialize with an error message
  explicit EstimatorInitError( const std::string& s ) : ErrorBase( s ) {}
  /// Initialize with an error message
  explicit EstimatorInitError( const char* s ) : ErrorBase( s ) {}
  /// Destructor
  virtual ~EstimatorInitError() {}
};

//////////////////////////////////////////////////////////////////////

/**
 * This class is a base class for maximum entropy estimators
 * such as GIS and IIS.
 */

class Estimator {
protected:
  /// Iteration counter
  int iteration_count;
  /// Number of report interval
  int report_interval;
  /// Minimum significant number
  Real machine_epsilon;
  /// Precision of the estimation
  int  estimation_precision;

  /// Log-likelihood of the event space with the current model
  Real log_likelihood;
  /// Log-likelihood of the event space with the current model (per event average)
  Real log_likelihood_per_event;
  
  /// The current value of the objective function of the estimation algorithm (as maximization)
  Real objective_function_value;
  /// The value of the objective function at the previous iteration
  Real prev_objective_function_value;
  
  /// Minimum value of model update
  Real min_update;
  /// Maximum value of model update
  Real max_update;
  /// Whether to iterate until convergence
  bool till_convergence;
  /// Whether to show messages
  bool suppress_message;

  /// Timer to measure the estimation time
  ProfTimer profile_timer;

public:
  /// Default number of iterations
  static const int DEFAULT_NUM_ITERATIONS = 100;
  /// Default precision
  static const int DEFAULT_PRECISION = 6;
  /// Default number of report interval
  static const int DEFAULT_REPORT_INTERVAL = 1;
  /// Maximum number of iterations
  static const int MAX_NUM_ITERATIONS = 5000;
  
  /// Convergence by absolute criterion
  static const int CONVERGED_ATOL = 0;
  /// Convergence by relative criterion
  static const int CONVERGED_RTOL = 1;
  /// Since the minimization returned increase of the objective function
  static const int BAD_INCREASE   = 2;


  Real ATOL;
  Real RTOL;
  
protected:
  /// Initialize min/max update values
  void initMinMaxUpdate() {
    min_update = REAL_MAX;
    max_update = -REAL_MAX;
  }

  /// Update min/max update values
  void setMinMaxUpdate( Real m ) {
    if ( min_update > m ) min_update = m;
    if ( max_update < m ) max_update = m;
  }

public:
  /// Get the iteration counter
  int iterationCount() const { return iteration_count; }
  /// Get the report interval
  int reportInterval() const { return report_interval; }
  /// Whether this iteration is a report iteration
  bool isReportIteration() const { return ( iteration_count % report_interval == 0 ); }
  /// Get the machine-epsilon value
  Real machineEpsilon() const { return machine_epsilon; }
  /// Get the precision of the estimation
  int estimationPrecision() const { return estimation_precision; }
  
  /// Get the log-likelihood of the event space with the current model
  Real logLikelihood() const { return log_likelihood; }
  /// Get the log-likelihood of the event space with the current model (per event average)
  Real logLikelihoodPerEvent() const { return log_likelihood_per_event; }

  /// Get the value of the objective function
  Real objectiveFunctionValue() const { return objective_function_value; }
  /// Get the previous value of the objective function
  Real previousObjectiveFunctionValue() const { return prev_objective_function_value; }

  /// Get the minimum value of the model
  Real minUpdate() const { return min_update; }
  /// Get the maximum value of the model
  Real maxUpdate() const { return max_update; }

  /// Whether an estimation continues until convergence
  bool tillConvergence() { return till_convergence; }
  /// Let the estimation to continue until convergence
  void setTillConvergence( bool t = true ) { till_convergence = t; }
  /// Whether to suppress profiling messages
  bool suppressMessage() { return suppress_message; }
  /// Let the estimator to suppress profiling messages
  void setSuppressMessage( bool t = true ) { suppress_message = t; }

  /// Get the timer measuring estimation time
  const ProfTimer& getEstimationTimer() { return profile_timer; }

  /// Set the log-likelihood
  void setLogLikelihood( Real ll_pe, Real sum_event_count ) {
    log_likelihood_per_event = ll_pe;
    log_likelihood = ll_pe * sum_event_count;
  }

  /// Set the value of the objective function
  void setObjectiveFunctionValue( Real fv ) {
	  objective_function_value = fv;
  }

  /// Set the previous value of the objective function
  void setPreviousObjectiveFunctionValue( Real fv ) {
	  prev_objective_function_value = fv;
  }
  
  /// Start a profile timer
  void startProfTimer() {
    profile_timer.startProf();
  }

  /// Stop a profile timer
  void stopProfTimer() {
    profile_timer.stopProf();
  }

  /// Write statistics to a stream
  virtual void writeStatToStream( std::ostream& os, const StatisticsBase& stat ) const {
    os << "Estimator:LOG_LIKELIHOOD " << log_likelihood << "\n";
    os << "Estimator:LOG_LIKELIHOOD_NORM " << log_likelihood_per_event << "\n";
    os << "Estimator:OBJECTIVE_FUNCTION_VALUE " << objective_function_value << "\n";
    os << "Estimator:ESTIMATION_REAL_SEC " << profile_timer.realSecP() << "\n";
    os << "Estimator:ESTIMATION_USER_SEC " << profile_timer.userSecP() << "\n";
    os << "Estimator:ESTIMATION_SYS_SEC " << profile_timer.systemSecP() << "\n";
    os << "Estimator:NUM_ITERATION " << iteration_count << "\n";
    
  }

  /// Write statistics to a stream
  virtual void writeHeavyStatToStream( std::ostream& os, const StatisticsBase& stat ) const {
    os << "Estimator:LOG_LIKELIHOOD_HISTORY (not implemented)\n";
    os << "Estimator:LOG_LIKELIHOOD_NORM_HISTORY (not implemented)\n";
    os << "Estimator:OBJECTIVE_FUNCTION_VALUE_HISTORY (not implemented)\n";
  }
  
protected:
  /// Initialization of the internal data before estimation
  virtual void initialize() = 0;
  /// Finalization of the internal data after estimation
  virtual void finalize() = 0;
  /// Each iteration for estimation
  virtual void iteration() = 0;
  /// After one iteration
  virtual void postIteration() {}
  /// Print start-up message
  virtual void startupMessage();
  /// Print a message for each report iteration
  virtual void reportMessage();
  /// Print ending message
  virtual void endingMessage();

public:
  /// Check whether the model is converged
  virtual bool isConverged() = 0;

  /// Name of the estimator
  virtual const std::string estimatorName() const = 0;

  /// Estimation of parameters of a maximum entropy model
  virtual void estimate( int num_iterations = DEFAULT_NUM_ITERATIONS,
                         int precision = DEFAULT_PRECISION,
                         int r = DEFAULT_REPORT_INTERVAL );

  /// Check whether the estimation converged
  virtual bool checkConvergence( double pf, double f, int* reason ) {
	  return checkConvergenceAmisStyle( pf, f, reason );
  }

  virtual bool checkConvergenceTaoStyle( double pf, double f, int* reason ) {
    AMIS_DEBUG_MESSAGE( 3, "checkConvergenceTaoStyle\n" );
	 
    if( pf - f <= ATOL ) {
      AMIS_DEBUG_MESSAGE( 3, "ABS_CONV=true\n" );
      *reason = CONVERGED_ATOL;
      return true;
    }
	  
    if( fabs( pf - f ) / ( fabs( pf ) + 1 ) <= RTOL ) {
      AMIS_DEBUG_MESSAGE( 3, "REL_CONV=true\n" );
      *reason = CONVERGED_RTOL;
      return true;
    }
	  
    return false;
  }
  
  virtual bool checkConvergenceAmisStyle( double pf, double f, int* reason ) {
	  /// From the implementation of BFGSSolver
    AMIS_DEBUG_MESSAGE( 3, "checkConvergenceAmisStyle\n" );
	 
	  
    if( pf - f <= ATOL ) {
      
      if( pf - f <= 0.0 ) {
	*reason = BAD_INCREASE;
      }
      else{
	*reason = CONVERGED_ATOL;
      }
      return true;
    }
	  
	  
    if( fabs( 1.0 - fabs( f/ pf )  ) <= RTOL ) {
      *reason = CONVERGED_RTOL;
      return true;
    }
    return false;
  }
 
protected:
  /// Maximum entropy model
  ModelBase* model;
  /// Event space, i.e., Set of events
  EventSpace* event_space;

  virtual void finalizeModel()
  {
    //normalizeModel();
  }

protected:
  /// Number of threads
  int num_threads;

  size_t numFeatures()
  {
    return model->numFeatures();
  }

  Real sumEventCount()
  {
    return event_space->sumEventCount();
  }

  int numEvents()
  {
    return event_space->numEvents();
  }

  void incLambda( FeatureID id, Real a ) {
    model->incLambda(id, a);
  }
  Real getLambda( FeatureID id ) {
    return model->getLambda( id );
  }
  void setLambda( FeatureID id , Real r) {
    return model->setLambda(id, r);
  }


  Real maxFeatureCount()
  {
    return event_space->maxFeatureCount();
  }

  std::string featureNameString( FeatureID id )
  {
    return model->featureNameString(id);
  }

  Real lambdaMax()
  {
    return Limits::LAMBDA_MAX;
  }
  Real lambdaMin()
  {
    return Limits::LAMBDA_MIN;
  }

public:
  Estimator();
  Estimator( ModelBase* init_model, EventSpace* init_event );
  /// Initialize with the initial model and the events
  virtual ~Estimator()
  {
    //delete model_expectation;
    //delete empirical_expectation;
    //delete empirical_variance;
  }

  //////////////////////////////////////////////////////////////////////

public:
  void setModel( ModelBase* m ) {
    model = m;
  }
  void setEventSpace( EventSpace* e ) {
    event_space = e;
  }
  /// Set the model
  const ModelBase& getModel() const {
    return *model;
  }
  /// Set the event space
  const EventSpace& getEventSpace() const {
    return *event_space;
  }
  /// Get the event space

  virtual int getNumThreads() {
    return num_threads;
  }
  virtual void setNumThreads( int n ) {
    num_threads = n;
  }
  /// Set the number of threads

};


typedef std::auto_ptr< Estimator > EstimatorPtr;


AMIS_NAMESPACE_END

#endif // Estimator_h_
// end of Estimator.h

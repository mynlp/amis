//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: Estimator.cc,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#include <amis/Estimator.h>
#include <amis/EstimatorLauncher.h>
//#include <amis/EmpiricalVarianceBase.h>

AMIS_NAMESPACE_BEGIN

Initializer< Launcher< EstimatorPtr, Property*, EstimatorIdentifier >* >* EstimatorLauncher::queue = NULL;

//PropertyItem<Real> EmpiricalVarianceBase::sdscale_features( "SDSCALE_FEATURES", "--sdscale-features", "", 0.0, "", true);
//PropertyItem<Real> EmpiricalVarianceBase::sdscale_priors( "SDSCALE_PRIORS", "--sdscale-priors", "", 0.0, "", true);

Estimator::Estimator()
{
  iteration_count = 0;
  report_interval = 0;
  machine_epsilon = 0.0;
  estimation_precision = 0;
  log_likelihood = 0.0;
  log_likelihood_per_event      = 0.0;
  objective_function_value      = 0.0;
  prev_objective_function_value = 0.0;
  min_update = 0.0;
  max_update = 0.0;
  till_convergence = false;
  suppress_message = false;

  model = NULL;
  event_space = NULL;
  num_threads = 1;
}

Estimator::Estimator( ModelBase* init_model, EventSpace* init_event ) 
{
  iteration_count = 0;
  report_interval = 0;
  machine_epsilon = 0.0;
  estimation_precision = 0;
  log_likelihood = 0.0;
  log_likelihood_per_event      = 0.0;
  objective_function_value      = 0.0;
  prev_objective_function_value = 0.0;
  min_update = 0.0;
  max_update = 0.0;
  till_convergence = false;
  suppress_message = false;

  setModel( init_model );
  setEventSpace( init_event );
  num_threads = 1;
}

void Estimator::startupMessage() {
  if( till_convergence ) {
    AMIS_PROF_MESSAGE( "(iterate till convergence)\n" );
  }
  AMIS_PROF_MESSAGE( "------------------------------------------------------------------------------\n" );
  AMIS_PROF_MESSAGE( "  Iter.  |  LL(log10)  |  LL(norm)   |     OBJ     | Min update  | Max update \n" );
  AMIS_PROF_MESSAGE( "------------------------------------------------------------------------------\n" );
  AMIS_PROF_MESSAGE( std::flush );
}
void Estimator::reportMessage() {
  AMIS_PROF_MESSAGE( std::resetiosflags( std::ios::adjustfield ) << std::setiosflags( std::ios::right )
		     << std::setw( 7 ) << iteration_count << "  " );
  AMIS_PROF_MESSAGE( std::setiosflags( std::ios::showpoint ) << std::setprecision( 6 )
		     << std::resetiosflags( std::ios::adjustfield ) << std::setiosflags( std::ios::left ) );
  AMIS_PROF_MESSAGE( "  " << std::setw( 12 ) << log_likelihood / log( 10.0 ) );
  AMIS_PROF_MESSAGE( "  " << std::setw( 12 ) << log_likelihood_per_event / log( 10.0 ) );
  AMIS_PROF_MESSAGE( "  " << std::setw( 12 ) << objective_function_value );
  AMIS_PROF_MESSAGE( "  " << std::setw( 12 ) << min_update );
  AMIS_PROF_MESSAGE( "  " << std::setw( 12 ) << max_update << '\n' );
  AMIS_PROF_MESSAGE( std::flush );
}
void Estimator::endingMessage() {
  AMIS_PROF_MESSAGE( "------------------------------------------------------------------------------\n" );
  AMIS_PROF_MESSAGE( "Estimation took " << profile_timer.realTimeString() << '\n' );
  AMIS_PROF_MESSAGE( "CPU Usage USER: " << profile_timer.userTimeString() << " SYSTEM: " << profile_timer.systemTimeString() << '\n' );
  //AMIS_PROF_MESSAGE( "Memory Usage: " << profile_timer.memorySize() << " bytes\n" );
  AMIS_PROF_MESSAGE( std::flush );
}

void Estimator::estimate( int num_iterations, int precision, int r ) {
  AMIS_DEBUG_MESSAGE( 3, "Initialization...\n" );

  machine_epsilon = pow( 0.1, precision + 1 );
  estimation_precision = precision;
	
  ATOL = REAL_TOL;
  RTOL = pow( 0.1, precision + 1);
	
  report_interval = r;
  initialize();
  profile_timer.initProf();
  AMIS_DEBUG_MESSAGE( 3, "Start.\n" );
  if ( ! suppress_message ) startupMessage();
  profile_timer.startProf();
  for ( iteration_count = 1; (till_convergence && iteration_count < MAX_NUM_ITERATIONS) || iteration_count <= num_iterations; ++iteration_count ) {
    //log_likelihood = 0.0;
    initMinMaxUpdate();
    iteration();
    if ( ! suppress_message && isReportIteration() ) reportMessage();
    postIteration();
    if ( isConverged() ) {
      if ( ! suppress_message ) AMIS_PROF_MESSAGE( "Model converged at iteration " << iteration_count << '\n' );
      break;
    }
  }
  finalizeModel();
  profile_timer.stopProf();
  if ( ! suppress_message ) endingMessage();
  finalize();
}

AMIS_NAMESPACE_END

// end of Estimator.cc

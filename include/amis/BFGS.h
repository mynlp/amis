//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: BFGS.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_BFGS_h_

#define Amis_BFGS_h_

#include <amis/configure.h>
#include <amis/EventSpace.h>
#include <amis/BatchEstimator.h>
#include <amis/BFGSSolver.h>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/**
 * Base class for ME estimator by limited-memory BFGS method.
 */

class ModelBase;

class BFGS : public BatchEstimator {
public:
  /// Default size of BFGS memory
  static const size_t DEFAULT_MEMORY_SIZE = 5;

  /// Number of vectors to memorize
  size_t memory_size;
  /// Optimizer based on limited-memory BFGS
  BFGSSolver< BFGS >* solver;
  ParameterSpaceBase* back_up;

  //////////////////////////////////////////////////////////////////////

public:
  /// Constructor
  explicit BFGS( size_t m = DEFAULT_MEMORY_SIZE )
  {
    memory_size = m;
    solver = 0;
    back_up = 0;
  }
  /// Constructor with model and event space
  BFGS( ModelBase* init_model, EventSpace* init_event, 
	size_t m = DEFAULT_MEMORY_SIZE )
    : BatchEstimator( init_model, init_event ) {
    memory_size = m;
    solver = 0;
    back_up = 0;
  }
  /// Destructor
  virtual ~BFGS() {
    //if ( solver != 0 ) delete solver;
    delete solver;
  }

  /// Get the size of memory
  virtual size_t getMemorySize() {
    return memory_size;
  }
  /// Set the size of memory
  virtual void setMemorySize( size_t m ) {
    memory_size = m;
  }

  //////////////////////////////////////////////////////////////////////

public:
  /// Initialize model/empirical expectations, and BFGS solver
  void initialize() {
    AMIS_DEBUG_MESSAGE( 3, "Set internal data...\n" );
    this->initNormalizers();
    AMIS_DEBUG_MESSAGE( 3, "Make a BFGS solver...\n" );
    delete solver;
    solver = new BFGSSolver< BFGS >( this, this->numFeatures(), memory_size, this->machineEpsilon() );
    solver->initialize();
  }
  /// Delete the BFGS solver
  void finalize() {
    if ( solver != 0 ) delete solver;
    solver = 0;
  }

  /// Check whether the model is converged
  bool isConverged() {
    return solver->isConverged();
  }

  /// An iteration of estimation - invoked by class Estimator
  void iteration() {
    //if ( isReportIteration() ) {
    //  log_likelihood = -solver->functionValue() * event_space->sumEventCount();
    //}
    solver->iteration();
  }

public:
  /// Compute the function value and the derivative - accessed by BFGSSolver
  virtual Real computeFunctionDerivative( std::valarray< Real >& grad )
  {
    Real ret = this->setModelExpectation();
	// now model_expectation does "* event_empirical_expectation"
    setLogLikelihood( ret, this->sumEventCount() );
    this->setObjectiveFunctionValue( ret );
	
    for ( size_t i = 0; i < this->numFeatures(); ++i ) {
      grad[ i ] = this->modelExpectation(i) - this->empiricalExpectation(i);
    }
	
    return -ret;
  }

  void backUpVector()
  {
    delete back_up;
    back_up = this->getModel().getParameterSpace()->clone();
  }

  /// Update the vector - accessed by BFGSSolver
  void testVector( const std::valarray< Real >& direction, Real a ) {
    this->initMinMaxUpdate();
    AMIS_DEBUG_MESSAGE( 5, "\t\t------------------------------\n" );
    for ( size_t i = 0; i < this->numFeatures(); ++i ) {
      //if ( empirical_expectation[ i ] != 0.0 ) {
      Real lambda_update = a * direction[ i ];
      this->setMinMaxUpdate( lambda_update );
      this->setLambda(i, back_up->getLambda(i) + lambda_update);
      AMIS_DEBUG_MESSAGE( 5, "\t\t" << i << "  " << getLambda( i ) << std::endl );
      //}
    }
    AMIS_DEBUG_MESSAGE( 5, "\t\t------------------------------\n" );
  }

  const std::string estimatorName() const {
    return "BFGS";
  }

};

AMIS_NAMESPACE_END

#endif // BFGS_h_
// end of BFGS.h

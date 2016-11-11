//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: OWBFGSBC.h,v 1.2 2008-07-29 17:05:14 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_OWBFGSBC_h_

#define Amis_OWBFGSBC_h_

#include <amis/configure.h>
#include <amis/EventSpace.h>
#include <amis/BatchEstimator.h>
#include <amis/BFGSSolver.h>

#include <amis/BLMVMBCLauncherItem.h>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/**
 * Base class for ME estimator by limited-memory BFGS method.
 */

class ModelBase;

class OWBFGSBC : public BatchEstimator {
public:
  /// Default size of BFGS memory
  static const size_t DEFAULT_MEMORY_SIZE = 5;

  /// Number of vectors to memorize
  size_t memory_size;
  /// Optimizer based on limited-memory BFGS
  BFGSSolver< OWBFGSBC, true >* solver;
  ParameterSpaceBase* back_up;

  //////////////////////////////////////////////////////////////////////

  Real al_bc_lower;
  Real al_bc_upper;

public:
  /// Constructor
  explicit OWBFGSBC( size_t m = DEFAULT_MEMORY_SIZE )
  {
    memory_size = m;
    solver = 0;
    back_up = 0;

    al_bc_lower = 1.0 / BLMVMBCLauncherItemBase::bc_lower.getValue();
    al_bc_upper = 1.0 / BLMVMBCLauncherItemBase::bc_upper.getValue();
  }
  /// Constructor with model and event space
  OWBFGSBC( ModelBase* init_model, EventSpace* init_event, 
	    size_t m = DEFAULT_MEMORY_SIZE )
    : BatchEstimator( init_model, init_event ) {
    memory_size = m;
    solver = 0;
    back_up = 0;
  }
  /// Destructor
  virtual ~OWBFGSBC() {
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
    solver = new BFGSSolver< OWBFGSBC, true >( this, this->numFeatures(), memory_size, this->machineEpsilon() );
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
  virtual Real computeFunctionDerivative( std::valarray< Real >& grad, std::valarray< Real >& p_grad )
  {
    Real ret = this->setModelExpectation();
	// now model_expectation does "* event_empirical_expectation"
    setLogLikelihood( ret, this->sumEventCount() );
    ret = -ret;

    for ( size_t i = 0; i < this->numFeatures(); ++i ) {
      Real x = this->getLambda(i);
      Real v = this->modelExpectation(i) - this->empiricalExpectation(i);
      grad[i] = v;
      if ( x > 0.0 ) {
	v += al_bc_upper;
	ret += x * al_bc_upper;
      }
      else if ( x < 0.0 ) {
	v -= al_bc_lower;
	ret -= x * al_bc_lower;
      }
      else {
	if ( v + al_bc_upper < 0.0 ) {
	  v += al_bc_upper;
	}
	else if ( v - al_bc_lower > 0.0 ) {
	  v -= al_bc_lower;
	}
	else {
	  v = 0.0;
	}
      }
      p_grad[i] = v;
    }
    this->setObjectiveFunctionValue( -ret );
    //std::cerr << "obj = " << -ret << std::endl;
	
    return ret;
  }

  void backUpVector()
  {
    delete back_up;
    back_up = this->getModel().getParameterSpace()->clone();
  }

  /// Update the vector - accessed by BFGSSolver
  void testVector( const std::valarray< Real >& direction, Real a)
  {
    this->initMinMaxUpdate();
    AMIS_DEBUG_MESSAGE( 5, "\t\t------------------------------\n" );

    //bool truncation = false;
    for ( size_t i = 0; i < this->numFeatures(); ++i ) {
      //if ( empirical_expectation[ i ] != 0.0 ) {
      Real lambda_update = a * direction[ i ];
      Real old_lambda = back_up->getLambda(i);
      if ( old_lambda > 0.0 ) {
	if ( old_lambda + lambda_update < 0.0 ) {
	  lambda_update = - old_lambda;
	  //truncation = true;
	}
      }
      else if ( old_lambda < 0.0 ) {
	if ( old_lambda + lambda_update > 0.0 ) {
	  lambda_update = - old_lambda;
	  //truncation = true;
	}
      }

      this->setMinMaxUpdate( lambda_update );
      this->setLambda(i, old_lambda + lambda_update);
    }
    //std::cerr << "truncation = " << truncation << std::endl;
    AMIS_DEBUG_MESSAGE( 5, "\t\t------------------------------\n" );
  }

  const std::string estimatorName() const {
    return "OWBFGSBC";
  }

};

AMIS_NAMESPACE_END

#endif // OWBFGSBC_h_
// end of OWBFGSBC.h

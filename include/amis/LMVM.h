//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: LMVM.h,v 1.2 2008-07-29 17:05:14 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_LMVM_h_

#define Amis_LMVM_h_

#include <amis/configure.h>
#include <amis/EventSpace.h>

#include <Property.h>
#include <amis/BatchEstimator.h>
#include <amis/BLMVMSolver.h>

#include <amis/LMVMLauncherItem.h>


AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/**
 * Base class for ME estimator by limited-memory variable metric method.
 */

class ModelBase;

class LMVM : public BatchEstimator {
public:
  /// Default size of LMVM memory
  static const size_t DEFAULT_MEMORY_SIZE = 5;

  /// Number of vectors to memorize
  size_t memory_size;

  /// Optimizer based on limited-memory LMVM
  class LMVMSolver : public BLMVMSolver {
    LMVM* estimator;
    Real machine_epsilon;
  public:
    LMVMSolver(LMVM* est,
	       int dim, int num_mem,
	       Real e)
      : BLMVMSolver(dim, num_mem), estimator(est),
	machine_epsilon(e)
    { }
  protected:
    double lowerBound(int i) { return -REAL_INF; }
    double upperBound(int i) { return REAL_INF; }

    /// Compute the function value and the derivative - accessed by LMVMSolver
    double computeFunctionGradient(BLMVMSolver::Vector& params,
				   BLMVMSolver::Vector* ret)
    {
      return estimator->computeFunctionGradient(params, ret);
    }

    bool checkConvergence(double , double ,
			  const BLMVMSolver::Vector& grad)
    {
      if ( fabs( 1.0 - fabs( fun_val / prev_val ) ) <= machine_epsilon ) {
	AMIS_DEBUG_MESSAGE( 3, "\tFunction value converged  " << prev_val << "  " << fun_val << "\n" );
	return true;
      }
      for ( size_t i = 0; i < grad.size(); ++i ) {
	if ( fabs( grad[ i ] ) > machine_epsilon ) return false;
      }
      AMIS_DEBUG_MESSAGE( 3, "\tGradient converged!\n" );
      return true;
    }
  };

  LMVMSolver* solver;

public:
  /// Constructor
  explicit LMVM( size_t m = DEFAULT_MEMORY_SIZE )
  {
    memory_size = m;
    solver = NULL;
  }

  /// Constructor with model and event space
  LMVM( ModelBase* init_model, EventSpace* init_event, 
	    size_t m = DEFAULT_MEMORY_SIZE )
    : BatchEstimator( init_model, init_event ) {
    memory_size = m;
    solver = NULL;
  }
  /// Destructor
  virtual ~LMVM() {
    //if ( solver != NULL ) delete solver;
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
  /// Initialize model/empirical expectations, and LMVM solver
  void initialize() {
    AMIS_DEBUG_MESSAGE( 3, "Set internal data...\n" );
    this->initNormalizers();
    AMIS_DEBUG_MESSAGE( 3, "Make a LMVM solver...\n" );
    delete solver;
    solver =
    new LMVMSolver(this, this->numFeatures(), memory_size,
		   this->machineEpsilon());

    for (unsigned i = 0; i < this->numFeatures(); ++i) {
      solver->setX(i, this->getLambda(i));
    }
    solver->bound();
  }
  /// Delete the LMVM solver
  void finalize() {
    delete solver;
    solver = NULL;
  }

  /// Check whether the model is converged
  bool isConverged() {
    //AMIS_ERROR_MESSAGE("converged: " << solver->converged() << '\n');
    return solver->converged();
  }

  /// An iteration of estimation - invoked by class Estimator
  void iteration()
  {
    solver->iter();
  }

  virtual Real computeFunctionGradient(BLMVMSolver::Vector& params,
			       BLMVMSolver::Vector* ret)
  {
    this->initMinMaxUpdate();
    AMIS_DEBUG_MESSAGE( 5, "\t\t------------------------------\n" );
    for ( size_t i = 0; i < this->numFeatures(); ++i ) {
      Real lambda_update = (params[i]) - this->getLambda(i);
      this->setMinMaxUpdate( lambda_update );
      this->incLambda( i, lambda_update );
      AMIS_DEBUG_MESSAGE( 5, "\t\t" << i << "  " << getLambda( i ) << std::endl );
    }
    AMIS_DEBUG_MESSAGE( 5, "\t\t------------------------------\n" );

    std::cerr << "calculating objective" << std::endl;
    Real ll = this->setModelExpectation();
    std::cerr << "obj = " << ll << std::endl;
    // now model_expectation does "* event_empirical_expectation"
    setLogLikelihood(ll, this->sumEventCount());
	
    for ( size_t i = 0; i < this->numFeatures(); ++i ) {
      (*ret)[i] = - this->empiricalExpectation(i) + this->modelExpectation(i);
    }
    this->setObjectiveFunctionValue(ll);
    return -ll;
  }

  const std::string estimatorName() const {
    return "LMVM";
  }
};

AMIS_NAMESPACE_END

#endif // LMVM_h_
// end of LMVM.h

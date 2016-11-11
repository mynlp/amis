//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: LMVMMAP.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_LMVMMAP_h_

#define Amis_LMVMMAP_h_

#include <amis/configure.h>
#include <amis/EventSpace.h>
#include <amis/LMVM.h>
#include <amis/GaussianPrior.h>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/**
 * Base class for ME estimator by limited-memory LMVM method.
 */

class LMVMMAP : public LMVM {
protected:
  /// Gaussian prior used for MAP estimation
  GaussianPriorPtr gaussian_prior;

public:
  typedef LMVM BaseClass;
  /// Constructor
  explicit LMVMMAP( size_t m = BaseClass::DEFAULT_MEMORY_SIZE )
    : BaseClass( m ) {
  }
  /// Constructor with a prior distribution
  explicit LMVMMAP( GaussianPriorPtr p, size_t m = BaseClass::DEFAULT_MEMORY_SIZE )
    : BaseClass( m ), gaussian_prior( p ) {
  }
  /// Constructor with a prior distribution, model, and event space
  LMVMMAP( ModelBase* init_model, EventSpace* init_event, GaussianPriorPtr p, size_t m = BaseClass::DEFAULT_MEMORY_SIZE )
    : BaseClass( init_model, init_event, m ), gaussian_prior( p ) {
  }
  /// Destructor
  virtual ~LMVMMAP() {}

  //////////////////////////////////////////////////////////////////////

public:
  /// Set a gaussian prior
  virtual void setGaussianPrior( GaussianPriorPtr p ) {
    gaussian_prior = p;
  }
  /// Get the currenct gaussian prior
  virtual const GaussianPrior& getGaussianPrior() const {
    return *gaussian_prior;
  }

public:
  /// Initialize the gaussian prior
  void initialize()
  {
    LMVM::initialize();
    gaussian_prior->initialize();
  }
  /// Compute the function value and the derivative - accessed by LMVMSolver

  Real computeFunctionGradient(BLMVMSolver::Vector& params,
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

    Real ll = this->setModelExpectation();
    // now model_expectation does "* event_empirical_expectation"
    setLogLikelihood(ll, this->sumEventCount());
	
    for ( size_t i = 0; i < this->numFeatures(); ++i ) {
      Real lambda_i = params[i];
      ll +=
	- lambda_i * lambda_i * (0.5 * (*gaussian_prior)[i]);

      (*ret)[i] =
	- this->empiricalExpectation(i) + this->modelExpectation(i)
	+ lambda_i * (*gaussian_prior)[i];
    }
    this->setObjectiveFunctionValue(ll);
    return -ll;
  }

  const std::string estimatorName() const {
    return "LMVMMAP(" + gaussian_prior->getName() + ")";
  }
};

AMIS_NAMESPACE_END

#endif // LMVMMAP_h_
// end of LMVMMAP.h

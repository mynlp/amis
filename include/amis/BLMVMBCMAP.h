//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: BLMVMBCMAP.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_BLMVMBCMAP_h_

#define Amis_BLMVMBCMAP_h_

#include <amis/configure.h>
#include <amis/EventSpace.h>
#include <amis/BLMVMBC.h>
#include <amis/GaussianPrior.h>

AMIS_NAMESPACE_BEGIN

/**
 * Base class for ME estimator by limited-memory BLMVMBC method.
 */

class BLMVMBCMAP : public BLMVMBC {
protected:
  /// Gaussian prior used for MAP estimation
  GaussianPriorPtr gaussian_prior;

public:
  typedef BLMVMBC BaseClass;
  /// Constructor
  explicit BLMVMBCMAP( size_t m = BaseClass::DEFAULT_MEMORY_SIZE )
    : BaseClass( m ) {
  }
  /// Constructor with a prior distribution
  explicit BLMVMBCMAP( GaussianPriorPtr p, size_t m = BaseClass::DEFAULT_MEMORY_SIZE )
    : BaseClass( m ), gaussian_prior( p ) {
  }
  /// Constructor with a prior distribution, model, and event space
  BLMVMBCMAP( ModelBase* init_model, EventSpace* init_event, GaussianPriorPtr p, size_t m = BaseClass::DEFAULT_MEMORY_SIZE )
    : BaseClass( init_model, init_event, m ), gaussian_prior( p ) {
  }
  /// Destructor
  virtual ~BLMVMBCMAP() {}

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
    BLMVMBC::initialize();
    gaussian_prior->initialize();
  }
  /// Compute the function value and the derivative - accessed by BLMVMBCSolver

  Real computeFunctionGradient(BLMVMSolver::Vector& params,
			       BLMVMSolver::Vector* ret)
  {
    this->initMinMaxUpdate();
    AMIS_DEBUG_MESSAGE( 5, "\t\t------------------------------\n" );
    for ( size_t i = 0; i < this->numFeatures(); ++i ) {
      Real lambda_update = (params[i*2+1] - params[i*2]) - this->getLambda(i);
      this->setMinMaxUpdate( lambda_update );
      this->incLambda( i, lambda_update );
      AMIS_DEBUG_MESSAGE( 5, "\t\t" << i << "  " << getLambda( i ) << std::endl );
    }
    AMIS_DEBUG_MESSAGE( 5, "\t\t------------------------------\n" );

    Real ll = this->setModelExpectation();
    // now model_expectation does "* event_empirical_expectation"
    setLogLikelihood(ll, this->sumEventCount());
	
    for ( size_t i = 0; i < this->numFeatures(); ++i ) {
      Real alpha_i = params[i*2+1];
      Real beta_i = params[i*2];
      ll +=
	- beta_i * this->al_bc_lower
	- beta_i * beta_i * (0.5 * (*gaussian_prior)[i])
	- alpha_i * this->al_bc_upper
	- alpha_i * alpha_i * (0.5 * (*gaussian_prior)[i]);

      (*ret)[i*2] =
	- this->modelExpectation(i) + this->empiricalExpectation(i)
	+ this->al_bc_lower
	+ beta_i * (*gaussian_prior)[i];
      (*ret)[i*2+1] =
	- this->empiricalExpectation(i) + this->modelExpectation(i)
	+ this->al_bc_upper
	+ alpha_i * (*gaussian_prior)[i];
    }
    this->setObjectiveFunctionValue(ll);
    return -ll;
  }


  const std::string estimatorName() const {
    return "BLMVMBCMAP(" + gaussian_prior->getName() + ")";
  }
};


AMIS_NAMESPACE_END

#endif // BLMVMBCMAP_h_
// end of BLMVMBCMAP.h

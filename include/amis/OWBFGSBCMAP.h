//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: OWBFGSBCMAP.h,v 1.2 2008-07-29 17:05:14 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_OWBFGSBCMAP_h_

#define Amis_OWBFGSBCMAP_h_

#include <amis/configure.h>
#include <amis/EventSpace.h>
#include <amis/OWBFGSBC.h>
#include <amis/GaussianPrior.h>

AMIS_NAMESPACE_BEGIN

/**
 * Base class for ME estimator by limited-memory OWBFGSBC method.
 */

class OWBFGSBCMAP : public OWBFGSBC {
protected:
  /// Gaussian prior used for MAP estimation
  GaussianPriorPtr gaussian_prior;

public:
  typedef OWBFGSBC BaseClass;
  /// Constructor
  explicit OWBFGSBCMAP( size_t m = BaseClass::DEFAULT_MEMORY_SIZE )
    : BaseClass( m ) {
  }
  /// Constructor with a prior distribution
  explicit OWBFGSBCMAP( GaussianPriorPtr p, size_t m = BaseClass::DEFAULT_MEMORY_SIZE )
    : BaseClass( m ), gaussian_prior( p ) {
  }
  /// Constructor with a prior distribution, model, and event space
  OWBFGSBCMAP( ModelBase* init_model, EventSpace* init_event, GaussianPriorPtr p, size_t m = BaseClass::DEFAULT_MEMORY_SIZE )
    : BaseClass( init_model, init_event, m ), gaussian_prior( p ) {
  }
  /// Destructor
  virtual ~OWBFGSBCMAP() {}

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
    OWBFGSBC::initialize();
    gaussian_prior->initialize();
  }
  /// Compute the function value and the derivative - accessed by OWBFGSBCSolver

  virtual Real computeFunctionDerivative( std::valarray< Real >& grad, std::valarray< Real >& p_grad )
  {
    Real ret = this->setModelExpectation();
	// now model_expectation does "* event_empirical_expectation"
    setLogLikelihood( ret, this->sumEventCount() );
    ret = -ret;
	
    for ( size_t i = 0; i < this->numFeatures(); ++i ) {
      Real x = this->getLambda(i);
      Real v =
	this->modelExpectation(i) - this->empiricalExpectation(i)
	+ x * (*gaussian_prior)[i];
      ret += x * x * (0.5 * (*gaussian_prior)[i]);
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
	
    return ret;
  }

  const std::string estimatorName() const {
    return "OWBFGSBCMAP(" + gaussian_prior->getName() + ")";
  }
};


AMIS_NAMESPACE_END

#endif // OWBFGSBCMAP_h_
// end of OWBFGSBCMAP.h

//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: BFGSMAP.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_BFGSMAP_h_

#define Amis_BFGSMAP_h_

#include <amis/configure.h>
#include <amis/EventSpace.h>
#include <amis/BFGS.h>
#include <amis/GaussianPrior.h>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/**
 * Base class for ME estimator by limited-memory BFGS method.
 */

class BFGSMAP : public BFGS {
protected:
  /// Gaussian prior used for MAP estimation
  GaussianPriorPtr gaussian_prior;

public:
  typedef BFGS BaseClass;
  /// Constructor
  explicit BFGSMAP( size_t m = BaseClass::DEFAULT_MEMORY_SIZE )
    : BaseClass( m ) {
  }
  /// Constructor with a prior distribution
  explicit BFGSMAP( GaussianPriorPtr p, size_t m = BaseClass::DEFAULT_MEMORY_SIZE )
    : BaseClass( m ), gaussian_prior( p ) {
  }
  /// Constructor with a prior distribution, model, and event space
  BFGSMAP( ModelBase* init_model, EventSpace* init_event, GaussianPriorPtr p, size_t m = BaseClass::DEFAULT_MEMORY_SIZE )
    : BaseClass( init_model, init_event, m ), gaussian_prior( p ) {
  }
  /// Destructor
  virtual ~BFGSMAP() {}

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
  void initialize() {
    BFGS::initialize();
    gaussian_prior->initialize();
  }
  /// Compute the function value and the derivative - accessed by BFGSSolver
  Real computeFunctionDerivative( std::valarray< Real >& grad )
  {
    AMIS_DEBUG_MESSAGE( 3, "begin BFGSMAP::computeFunctionDerivative" );
    Real ret = BFGS::computeFunctionDerivative( grad );
    for ( size_t i = 0; i < BaseClass::numFeatures(); ++i ) {
      Real lambda = BaseClass::getLambda( i );
      ret += lambda * lambda
	* ( 0.5 * (*gaussian_prior)[ i ]);
      grad[ i ] += lambda * (*gaussian_prior)[ i ];
    }
    BaseClass::setObjectiveFunctionValue( -ret ); // likelihood != objective function
    AMIS_DEBUG_MESSAGE( 3, "end BFGSMAP::computeFunctionDerivative" );
    return ret;
  }

  const std::string estimatorName() const {
    return "BFGSMAP(" + gaussian_prior->getName() + ")";
  }
};

AMIS_NAMESPACE_END

#endif // BFGSMAP_h_
// end of BFGSMAP.h

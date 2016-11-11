//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: GISMAP.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_GISMAP_h_

#define Amis_GISMAP_h_

#include <amis/configure.h>
#include <amis/EventSpace.h>
#include <amis/GIS.h>
#include <amis/GaussianPrior.h>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/**
 * Base class for ME estimator by limited-memory BLMVMBC method.
 */

class GISMAP : public GIS {
protected:
  /// Gaussian prior used for MAP estimation
  GaussianPriorPtr gaussian_prior;
  static const size_t max_newton_iteration = 100;

public:
  typedef GIS BaseClass;
  /// Constructor
  explicit GISMAP() { }
  /// Constructor with a prior distribution
  explicit GISMAP( GaussianPriorPtr p )
    : gaussian_prior( p ) { }
  /// Constructor with a prior distribution, model, and event space
  GISMAP( ModelBase* init_model, EventSpace* init_event, GaussianPriorPtr p )
    : BaseClass( init_model, init_event ), gaussian_prior( p ) {
  }
  /// Destructor
  virtual ~GISMAP() {}

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


private:
  Real newtonGISMAP(FeatureID i)
  {
    Real x = 0.0;
    
    Real ee = BaseClass::empiricalExpectation(i);
    Real me = BaseClass::modelExpectation(i);
    Real l = BaseClass::getLambda(i);
    Real sigma2 = (*gaussian_prior)[ i ];

    for (size_t i = 0; i < max_newton_iteration; ++i) {
      Real f =
	me * exp(BaseClass::count*x) +
	(l+x) * sigma2 - ee;
      Real fd =
	BaseClass::count * me * exp(BaseClass::count*x) +
	sigma2;
      Real update = f / fd;
      x -= update;
      if ( fabs(update / x) < this->machine_epsilon ) {
	break;
      }
    }
    return x;
  }


public:
  /// Initialize the gaussian prior
  void initialize()
  {
    GIS::initialize();
    gaussian_prior->initialize();
  }

  Real solveEquation( FeatureID i )
  {
    Real ret = newtonGISMAP(i);
    return ret;
  }

  const std::string estimatorName() const {
    return "GISMAP(" + gaussian_prior->getName() + ")";
  }
};


AMIS_NAMESPACE_END

#endif // Amis_GISMAP_h_
// end of GIS.h

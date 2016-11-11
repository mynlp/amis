//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: BatchEstimator.h,v 1.2 2008-07-29 17:05:14 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_BatchEstimator_h_

#define Amis_BatchEstimator_h_

#include <amis/configure.h>

#include <amis/Estimator.h>
#include <amis/EmpiricalExpect.h>
#include <amis/ModelExpect.h>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/**
 * This class is a base class for maximum entropy estimators
 * such as GIS and IIS.
 */

class BatchEstimator : public Estimator {
private:
  /// Empirical expectation of features
  EmpiricalExpect empirical_expectation;
  ModelExpect model_expectation;
protected:
  void initNormalizers()
  {
    model_expectation.initialize( model, event_space );
    empirical_expectation.initialize( model, event_space );
    empirical_expectation.setEmpiricalExpectation();
  }

  Real setModelExpectation(bool report_iteration = true,
			   int begin_id = 0,
			   int end_id = -1)
  {
    Real ret =
      model_expectation.setModelExpectation();
    //std::cerr << "ll = " << ret << std::endl;
    return ret;
  }

  Real modelExpectation(int i)
  {
    return model_expectation[i];// / empirical_variance->getFeatureScale(i);
  }
  Real empiricalExpectation(int i)
  {
    return empirical_expectation[i];// / empirical_variance->getFeatureScale(i);
  }

public:
  BatchEstimator()
  { }
  BatchEstimator( ModelBase* init_model, EventSpace* init_event )
    : Estimator(init_model, init_event)
  { }
  /// Initialize with the initial model and the events
  virtual ~BatchEstimator()
  { }

  //////////////////////////////////////////////////////////////////////

public:
  const EmpiricalExpect& getEmpiricalExpectation()
  {
    return empirical_expectation;
  }

};



AMIS_NAMESPACE_END

#endif // Estimator_h_
// end of Estimator.h

//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: APEstimator.h,v 1.2 2008-07-29 17:05:14 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_APEstimator_h_

#define Amis_APEstimator_h_

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

class APEstimator : public Estimator {
  Real step_size;
  int online_iteration;
  std::vector<Real> history;
public:
  APEstimator()
  {
    online_iteration = 0;
    step_size = 1.0;
  }
  APEstimator( ModelBase* init_model, EventSpace* init_event )
    : Estimator(init_model, init_event)
  {
    online_iteration = 0;
    step_size = 1.0;
  }
  /// Initialize with the initial model and the events
  virtual ~APEstimator()
  { }

  void setStepSize(Real ss)
  {
    step_size = ss;
  }

  Real getStepSize()
  {
    return step_size;
  }

  void initialize()
  {
    history.resize(model->numFeatures(), 0.0);
  }
  /// Delete the BFGS solver
  void finalizeModel()
  {
    //std::cerr << "finalizing ... " << online_iteration << std::endl;
    for ( size_t index = 0; index < model->numFeatures(); ++index ) {
      model->incLambda(index,
		       - history[index] / static_cast<double>(online_iteration));
    }
  }
  void finalize()
  { }


  /// Check whether the model is converged
  bool isConverged()
  {
    return false;
  }

  /// An iteration of estimation - invoked by class Estimator
  void iteration()
  {
    //std::cerr << "step_size = " << step_size << std::endl;
    event_space->getAccumulator()->clearIgnored();
    for ( size_t index = 0; index < event_space->numEvents(); ++ index ) {
      ++online_iteration;
      const EventBase& event = (*event_space)[index];
      event_space->getAccumulator()->perceptronUpdate(event,
						      step_size,
						      online_iteration,
						      *model,
						      history);
    }
    if ( event_space->getAccumulator()->numIgnored() > 0 ) {
      AMIS_WARNING_MESSAGE(event_space->getAccumulator()->numIgnored() << " events are ignored because of infinite weight sum\n");
    }
  }

  const std::string estimatorName() const
  {
    return "AP";
  }
  //////////////////////////////////////////////////////////////////////

};



AMIS_NAMESPACE_END

#endif // Estimator_h_
// end of Estimator.h

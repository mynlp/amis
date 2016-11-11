//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: AmisWeightAccumulator.h,v 1.2 2008-07-29 17:05:14 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_AmisWeightAccumulator_h_

#define Amis_AmisWeightAccumulator_h_

#include <amis/AmisWeightAccumulator.h>
#include <amis/Event.h>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/**
 * The class imports event data from Amis-style data files.
 * The Amis format enables non-binary features and packed events.
 * For details, see README.
 * @see Event, EventSpace, AmisWeightAccumulator
 */


template<class Feature, class ParameterValue, bool ReferenceProbability>
class AmisWeightAccumulator : public WeightAccumulator {
public:
  typedef Event<Feature, ReferenceProbability> EventType;
  AmisWeightAccumulator()
  { }

  void empiricalCount(std::vector<Real>& ee,
		      const EventBase& event_base,
		      Real event_empirical_expectation)
  {
    const EventType& event =
      dynamic_cast<const EventType&>(event_base);
    
    const FeatureList< Feature >& fl = event.observedEvent();
    for ( typename FeatureList< Feature >::const_iterator feature = fl.begin();
	  feature != fl.end();
	  ++feature ) {
      AMIS_DEBUG_MESSAGE( 5, "feature=" << feature->id() << '\n' );
      ee[feature->id()] += feature->freq() * event.eventFrequency() * event_empirical_expectation;
    }
  }

  Real modelCount(std::vector<Real>& me,
		  const EventBase& event_base,
		  Real event_empirical_expectation,
		  const ModelBase& model)
  {
    const EventType& event =
      dynamic_cast<const EventType&>(event_base);
    const ParameterSpace<ParameterValue>* parameterspace =
      dynamic_cast<const ParameterSpace<ParameterValue>*>(model.getParameterSpace());

    int n = event.numFeatureList();
    std::vector<ParameterValue> prod(n);
    typename std::vector<ParameterValue>::iterator pt;
    Real sum = 0.0;

    pt = prod.begin();
    for ( typename EventType::const_iterator fl = event.begin(); fl != event.end(); ++pt, ++fl ) {
      pt->initValue(ParameterValue::exponent( parameterspace->getParameters(), *fl ));
    }
    ParameterValue::scale_prob(prod.begin(), prod.end());

    pt = prod.begin();
    for ( typename EventType::const_iterator fl = event.begin(); fl != event.end(); ++pt, ++fl ) {
      if ( ReferenceProbability ) {
	*pt *= fl->referenceProbability();
	//ref_sum += fl->referenceProbability();
      }
      sum += pt->getAlpha();
    }

    if ( ! finite( sum ) ) {
      this->incIgnored();
      //OStringStream oss;
      //oss << "IN: " << __PRETTY_FUNCTION__ << "sum is not finite (" << sum << "). this should not occur";
      //AMIS_WARNING_MESSAGE( oss.str() << '\n' );
      return 0.0;
      //throw IllegalModelExpectationError( oss.str() );
    }
    if ( sum != 0.0 ) {
      Real inv_sum = 1.0 / sum;
      Real denom = event.eventFrequency() * inv_sum * event_empirical_expectation;
      for ( size_t i = 0; i < event.size(); ++i ) {
	const FeatureList< Feature >& fl = event[i];
	for ( typename FeatureList<Feature>::const_iterator feature = fl.begin();
	      feature != fl.end();
	      ++feature ) {
	  me[feature->id()] +=
	    feature->freq() * prod[i].getAlpha() * denom;
	}
      }
      Real ret =
	event.eventProbability() *
	event.eventFrequency() * event_empirical_expectation *
        ModelBase::safe_log( prod[event.observedEventID()].getAlpha() * inv_sum * event.eventProbability() );
      return ret;
    }
    else {
      return 0.0;
    }
  }



  void perceptronUpdate(const EventBase& event_base,
			Real step_size,
			int iteration,
			ModelBase& model,
			std::vector<Real>& history)
  {
    const EventType& event =
      dynamic_cast<const EventType&>(event_base);
    const ParameterSpace<ParameterValue>* parameterspace =
      dynamic_cast<const ParameterSpace<ParameterValue>*>(model.getParameterSpace());

    size_t n = event.numFeatureList();
    std::vector<Real> prod(n);

    for ( size_t i = 0; i < n; ++i ) {
      prod[i] = ParameterValue::exponent(parameterspace->getParameters(), event[i]);
    }

    Real max = 0.0;
    size_t max_target = 0;
    for ( size_t i = 0; i < n; ++i ) {
      if ( ReferenceProbability ) {
	prod[i] *= event[i].referenceProbability();
	//ref_sum += fl->referenceProbability();
      }
      if ( prod[i] > max ) {
	max = prod[i];
	max_target = i;
      }
    }

    if ( ! finite(max) ) {
      this->incIgnored();
      //OStringStream oss;
      //oss << "IN: " << __PRETTY_FUNCTION__ << "sum is not finite (" << max << "). this should not occur";
      //AMIS_WARNING_MESSAGE( oss.str() << '\n' );
      return;
    }

    const FeatureList< Feature >& best = event[max_target];
    for ( typename FeatureList< Feature >::const_iterator feature = best.begin();
	  feature != best.end();
	  ++feature ) {
      Real incr = - feature->freq() * event.eventFrequency() * step_size;
      model.incLambda(feature->id(), incr);
      history[feature->id()] += static_cast<double>(iteration) * incr;
    }

    const FeatureList< Feature >& observed = event.observedEvent();
    for ( typename FeatureList< Feature >::const_iterator feature = observed.begin();
	  feature != observed.end();
	  ++feature ) {
      Real incr = feature->freq() * event.eventFrequency() * step_size;
      model.incLambda(feature->id(), incr);
      history[feature->id()] += static_cast<double>(iteration) * incr;
    }
  }

};



AMIS_NAMESPACE_END

#endif // Amis_AmisWeightAccumulator_h_
// end of AmisWeightAccumulator.h

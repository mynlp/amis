//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: FixWeightAccumulator.h,v 1.2 2008-07-29 17:05:14 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_FixWeightAccumulator_h_

#define Amis_FixWeightAccumulator_h_

#include <amis/FixWeightAccumulator.h>
#include <amis/Event.h>


AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/**
 * The class imports event data from Amis-style data files.
 * The Amis format enables non-binary features and packed events.
 * For details, see README.
 * @see Event, EventSpace, FixWeightAccumulator
 */


template<class Feature, class ParameterValue, bool ReferenceProbability>
class FixWeightAccumulator : public WeightAccumulator {
  const FixedTarget& fixed_target;
public:
  typedef EventFix<Feature, ReferenceProbability> EventType;
  FixWeightAccumulator(const FixedTarget& ft)
    : fixed_target(ft)
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

    size_t n = fixed_target.numTargets();

    std::vector<ParameterValue> prod(n);
    Real sum = 0.0;
    Real ref_sum = 0.0;
    const FeatureList<Feature>& hist_list  = event.getHistoryList();

    for( typename FeatureList<Feature>::const_iterator itr = hist_list.begin();
         itr != hist_list.end();
         ++itr ) {
      FeatureID hid = itr->id();
      typename Feature::FeatureFreq freq = itr->freq();
      const std::vector< std::pair<FeatureID, FeatureID> >& fv =
	fixed_target.getFeatures( hid );

      ParameterValue::exponent_fix( parameterspace->getParameters(), fv, freq, prod);
    }
    ParameterValue::scale_prob( prod.begin(), prod.end());
    
    for ( size_t i = 0; i < n; ++i ) {
      if ( ReferenceProbability ) {
        prod[ i ] *= event.referenceProbability( i );
        ref_sum += event.referenceProbability( i );
      }
      sum += prod[i].getAlpha();
    }

    if( !finite(sum) ){
      this->incIgnored();
      //OStringStream oss;
      //oss << "IN: " << __PRETTY_FUNCTION__ << "sum is not finite (" << sum << "). this should not occur";
      //AMIS_WARNING_MESSAGE( oss.str() << '\n' );
      return 0.0;
    }

    if ( sum != 0.0 ) {
      Real inv_sum = 1.0 / sum;
      Real denom = event.eventFrequency() * inv_sum * event_empirical_expectation;
      for( typename FeatureList<Feature>::const_iterator itr = hist_list.begin();
	   itr != hist_list.end();
	   ++itr ) {
	FeatureID hid = itr->id();
	typename Feature::FeatureFreq freq = itr->freq();
	const std::vector< std::pair<FeatureID, FeatureID> >& fv =
	  fixed_target.getFeatures( hid );
	for ( std::vector< std::pair<FeatureID, FeatureID> >::const_iterator it = fv.begin();
	      it != fv.end();
	      ++it ) {
	  me[it->second] +=
	    freq * prod[it->first].getAlpha() * denom;
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

    size_t n = fixed_target.numTargets();

    std::vector<ParameterValue> prod(n);
    Real sum = 0.0;
    Real ref_sum = 0.0;
    const FeatureList<Feature>& hist_list  = event.getHistoryList();

    for( typename FeatureList<Feature>::const_iterator itr = hist_list.begin();
         itr != hist_list.end();
         ++itr ) {
      FeatureID hid = itr->id();
      typename Feature::FeatureFreq freq = itr->freq();
      const std::vector< std::pair<FeatureID, FeatureID> >& fv =
	fixed_target.getFeatures( hid );

      ParameterValue::exponent_fix( parameterspace->getParameters(), fv, freq, prod);
    }
    
    for ( size_t i = 0; i < n; ++i ) {
      if ( ReferenceProbability ) {
        prod[ i ] *= event.referenceProbability( i );
        ref_sum += event.referenceProbability( i );
      }
      sum += prod[i].getAlpha();
    }

    Real max = 0.0;
    size_t max_target = 0;
    for ( size_t i = 0; i < n; ++i ) {
      if ( ReferenceProbability ) {
	prod[i] *= event.referenceProbability(i);
	//ref_sum += fl->referenceProbability();
      }
      if ( prod[i].getAlpha() > max ) {
	max = prod[i].getAlpha();
	max_target = i;
      }
    }

    if( !finite(max) ){
      this->incIgnored();
      //OStringStream oss;
      //oss << "IN: " << __PRETTY_FUNCTION__ << "sum is not finite (" << max << "). this should not occur";
      //AMIS_WARNING_MESSAGE( oss.str() << '\n' );
      return;
    }

    for( typename FeatureList<Feature>::const_iterator itr = hist_list.begin();
	 itr != hist_list.end();
	 ++itr ) {
      FeatureID hid = itr->id();
      typename Feature::FeatureFreq freq = itr->freq();
      const std::vector< std::pair<FeatureID, FeatureID> >& fv =
	fixed_target.getFeatures( hid );
      for ( std::vector< std::pair<FeatureID, FeatureID> >::const_iterator it = fv.begin();
	    it != fv.end();
	    ++it ) {
	if ( it->first == max_target ) {
	  Real incr = - freq * event.eventFrequency() * step_size;
	  model.incLambda(it->second, incr);
	  history[it->second] += static_cast<double>(iteration) * incr;
	}
      }
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

#endif // Amis_FixWeightAccumulator_h_
// end of FixWeightAccumulator.h

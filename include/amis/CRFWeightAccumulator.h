//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: CRFWeightAccumulator.h,v 1.2 2008-07-29 17:05:14 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_CRFWeightAccumulator_h_

#define Amis_CRFWeightAccumulator_h_

#include <amis/CRFWeightAccumulator.h>
#include <amis/Event.h>

#include <amis/CRFTable.h>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/**
 * The class imports event data from Amis-style data files.
 * The Amis format enables non-binary features and packed events.
 * For details, see README.
 * @see Event, EventSpace, CRFWeightAccumulator
 */


template<class Feature, class ParameterValue, bool ReferenceProbability >
class CRFWeightAccumulator : public WeightAccumulator {
  const FixedTarget& fixed_target;
  const CRFTransition& transition;
  CRFTable<Feature, ParameterValue, ReferenceProbability> crf_table;
  CRFViterbi<Feature, ParameterValue, ReferenceProbability> crf_viterbi;
public:
  typedef EventCRF<Feature, ReferenceProbability> EventType;
  CRFWeightAccumulator(const FixedTarget& ft, const CRFTransition& crf)
    : fixed_target(ft), transition(crf), crf_table(ft, crf), crf_viterbi(ft, crf)
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
    const EventType& crf =
      dynamic_cast<const EventType&>(event_base);
    const ParameterSpace<ParameterValue>* parameterspace =
      dynamic_cast<const ParameterSpace<ParameterValue>*>(model.getParameterSpace());
    
    Real log_scale = crf_table.setProduct( *parameterspace, crf );
    ParameterValue inv_sum = crf_table.sumProduct();

    if ( !finite( inv_sum.getValue() ) ) {
      this->incIgnored();
      //OStringStream oss;
      //oss << "IN: " << __PRETTY_FUNCTION__ << "sum is not finite (" << inv_sum.getValue() << "). this should not occur";
      //AMIS_WARNING_MESSAGE( oss.str() << '\n' );
      return 0.0;
    }

    inv_sum.reciprocal();

    Real denom = crf.eventFrequency() * event_empirical_expectation;

    AMIS_DEBUG_MESSAGE( 5, "\t\tSet model expectations for features\n" );
    for ( size_t i = 0; i < crf.length(); ++i ) {
      const std::vector<ParameterValue>& weights = crf_table.targetWeight(i);
      const std::vector< Feature >& history_list = crf.getHistoryList()[i];
      for ( typename std::vector< Feature >::const_iterator h_it = history_list.begin();
	    h_it != history_list.end();
	    ++h_it ) {
	Real fb_product = denom * h_it->freq();
	const std::vector< std::pair<FeatureID, FeatureID> >& fv = fixed_target.getFeatures(h_it->id());
	for ( std::vector< std::pair<FeatureID, FeatureID> >::const_iterator tf_it = fv.begin();
	      tf_it != fv.end();
	      ++tf_it ) {
	  me[tf_it->second] += weights[tf_it->first].getAlpha() * fb_product;
	}
      }
    }

    Real lp = 0.0;
    for ( typename FeatureList<Feature>::const_iterator fit = crf.observedEvent().begin();
	  fit != crf.observedEvent().end();
	  ++fit ) {
      lp += parameterspace->getParameter( fit->id() ).getLambda() * fit->freq();
    }
    lp += (inv_sum.getLambda()-log_scale);
    if ( ReferenceProbability ) {
      assert(false);
      //lp *= crf.observedEvent().referenceProbability();
    }
    return event_empirical_expectation * crf.frequency() * lp;
  }

  void perceptronUpdate(const EventBase& event_base,
			Real step_size,
			int iteration,
			ModelBase& model,
			std::vector<Real>& history)
  {
    const EventType& crf =
      dynamic_cast<const EventType&>(event_base);
    const ParameterSpace<ParameterValue>* parameterspace =
      dynamic_cast<const ParameterSpace<ParameterValue>*>(model.getParameterSpace());
    
    crf_viterbi.setPath( *parameterspace, crf );
    ParameterValue max = crf_viterbi.maxProduct();

    //std::cerr << "max = " << max.getValue() << std::endl;
    if ( !finite( max.getValue() ) ) {
      this->incIgnored();
      //OStringStream oss;
      //oss << "IN: " << __PRETTY_FUNCTION__ << "sum is not finite (" << max.getValue() << "). this should not occur";
      //AMIS_WARNING_MESSAGE( oss.str() << '\n' );
      return;
    }
    //std::cerr << "update" << std::endl;

    //std::cerr << "max path = ";
    for ( std::pair<size_t, size_t> it = crf_viterbi.rbegin();
	  true;
	  crf_viterbi.next(it) ) {
      //std::cerr << " <" << it.first << "," << it.second << "/" << crf.observedEventIDs()[it.first] << ">";
      const std::vector< Feature >& history_list = crf.getHistoryList()[it.first];
      for ( typename std::vector< Feature >::const_iterator h_it = history_list.begin();
	    h_it != history_list.end();
	    ++h_it ) {
	const std::vector< std::pair<FeatureID, FeatureID> >& fv = fixed_target.getFeatures(h_it->id());
	for ( std::vector< std::pair<FeatureID, FeatureID> >::const_iterator tf_it = fv.begin();
	      tf_it != fv.end();
	      ++tf_it ) {
	  const std::vector<FeatureID>& targets = transition.out(it.second);
	  if ( std::find(targets.begin(), targets.end(), tf_it->first) != targets.end() ) {
	    Real incr = -h_it->freq() * crf.eventFrequency() * step_size;
	    model.incLambda(tf_it->second, incr);
	    history[tf_it->second] += static_cast<double>(iteration) * incr;
	  }
	}
      }
      if ( it == crf_viterbi.rend() ) break;
    }
    //std::cerr << std::endl;

    const FeatureList< Feature >& observed = crf.observedEvent();
    for ( typename FeatureList< Feature >::const_iterator feature = observed.begin();
	  feature != observed.end();
	  ++feature ) {
      Real incr = feature->freq() * crf.eventFrequency() * step_size;
      model.incLambda(feature->id(), incr);
      history[feature->id()] += static_cast<double>(iteration) * incr;
    }
  }

};



AMIS_NAMESPACE_END

#endif // Amis_CRFWeightAccumulator_h_
// end of CRFWeightAccumulator.h

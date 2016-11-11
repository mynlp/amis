//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: CRFTable.h,v 1.2 2008-07-28 20:30:56 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_CRFTable_h_

#define Amis_CRFTable_h_

#include <amis/configure.h>
#include <amis/EventCRF.h>
#include <vector>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/**
 * Partially calculated alpha product values are stored in a tree
 * structure according to the tree structure of an event.
 */

template < class Feature, class ParameterValue, bool ReferenceProbability = false >
class CRFTable {
public:
  /// Feature frequency
  typedef typename Feature::FeatureFreq FeatureFreq;

private:
  const FixedTarget& fixed_target;
  const CRFTransition& transition;

  size_t max_length;
  size_t num_targets;
  size_t num_states;
  std::vector< std::vector< ParameterValue > > state_weight;
  std::vector< std::vector< ParameterValue > > forward_prod;
  std::vector< std::vector< ParameterValue > > backward_prod;
  //std::vector< std::vector< ParameterValue > > tmp_prod;
  ParameterValue sum_forward;
  //std::vector< Real > sum_reference;

public:
  /// Constructor
  CRFTable(const FixedTarget& ft, const CRFTransition& crf)
    : fixed_target(ft), transition(crf)
  {
    num_targets = fixed_target.numTargets();
    num_states = transition.numStates();
    state_weight.resize(200, std::vector<ParameterValue>(num_targets));
    forward_prod.resize(200, std::vector<ParameterValue>(num_states));
    backward_prod.resize(200, std::vector<ParameterValue>(num_states));
  }
  /// Destructor
  ~CRFTable() {}

public:
  /// Initialize the tree of the size _id_ with alpha sets of the size _freq_
  /// Set the products of alpha values to the tree
  Real setProduct( const ParameterSpace<ParameterValue>& parameterspace,
		   const EventCRF< Feature, ReferenceProbability >& event_crf )
  {
    Real log_scale = 0.0;
    //std::cerr << "start setProduct" << std::endl;
    //event_crf.getTransition().debugInfo(std::cerr);

    size_t crf_length = event_crf.length();
    if ( crf_length == 0 || num_states == 0 ||
	 num_targets == 0 || parameterspace.getParameters().size() == 0 ) return 0.0;
    if ( crf_length >= forward_prod.size() ) {
      state_weight.resize(crf_length, std::vector<ParameterValue>(num_targets));
      forward_prod.resize(crf_length, std::vector<ParameterValue>(num_states));
      backward_prod.resize(crf_length, std::vector<ParameterValue>(num_states));
      //tmp_prod.resize(crf_length, std::vector<ParameterValue>(num_states));
    }
    
    FeatureID start_state = event_crf.observedEventIDs().front();
    FeatureID end_state = event_crf.observedEventIDs().back();

    for ( size_t index = 0; index < crf_length; ++index ) {
      const std::vector<Feature>& hist_list  = event_crf.getHistoryList()[index];
      std::vector<ParameterValue>& state_weight_index(state_weight[index]);
      for ( size_t target = 0; target < num_targets; ++target ) {
	state_weight_index[target].initAlpha(1.0);
      }
      for( typename std::vector<Feature>::const_iterator itr = hist_list.begin();
	   itr != hist_list.end();
	   ++itr ) {
	typename Feature::FeatureFreq freq = itr->freq();
	ParameterValue::exponent_fix(parameterspace.getParameters(),
				     fixed_target.getFeatures(itr->id()),
				     freq,
				     state_weight_index);
      }
    }

    for ( size_t index = 0; index < crf_length; ++index ) {
      std::vector<ParameterValue>& state_weight_index(state_weight[index]);
      std::vector<ParameterValue>& backward_prod_index(backward_prod[index]);
      for ( size_t state = 0; state < num_states; ++state ) {
	const std::vector<FeatureID>& targets(transition.out(state));
	ParameterValue tmp; tmp.initAlpha(1.0);
	for ( std::vector<FeatureID>::const_iterator tt = targets.begin();
	      tt != targets.end();
	      ++tt ) {
	  tmp *= state_weight_index[*tt];
	}
	backward_prod_index[state] = tmp;
      }
      for ( size_t target = 0; target < num_targets; ++ target ) {
	state_weight_index[target].initAlpha(0.0);
      }
    }

    for ( size_t state = 0; state < num_states; ++state ) {
      forward_prod[0][state].initAlpha(0);
    }
    forward_prod[0][start_state] = backward_prod[0][start_state];

    log_scale += log(ParameterValue::scale_weight(forward_prod[0], backward_prod[0]));
    for ( size_t index = 1; index < crf_length; ++index ) {
      std::vector<ParameterValue>& forward_prod_index(forward_prod[index]);
      std::vector<ParameterValue>& forward_prod_index_1(forward_prod[index-1]);
      std::vector<ParameterValue>& backward_prod_index(backward_prod[index]);
      for ( size_t state = 0; state < num_states; ++state ) {
	forward_prod_index[state].accumulate(forward_prod_index_1, transition.prevStates(state));
	forward_prod_index[state] *= backward_prod_index[state];
      }
      log_scale += log(ParameterValue::scale_weight(forward_prod[index], backward_prod_index));
    }
    sum_forward = forward_prod[crf_length-1][end_state];


    for ( size_t state = 0; state < num_states; ++state ) {
      if ( state != end_state ) {
	backward_prod[crf_length-1][state].initAlpha(0);
      }
    }
    ParameterValue tmp;
    for ( int s_index = crf_length-2; s_index >= 0; --s_index ) {
      size_t index = s_index;
      std::vector<ParameterValue>& forward_prod_index(forward_prod[index]);
      std::vector<ParameterValue>& backward_prod_index(backward_prod[index]);
      std::vector<ParameterValue>& backward_prod_index_1(backward_prod[index+1]);
      std::vector<ParameterValue>& state_weight_index(state_weight[index]);
      for ( size_t state = 0; state < num_states; ++state ) {
	tmp.accumulate(backward_prod_index_1, transition.nextStates(state));
	backward_prod_index[state] *= tmp;

	tmp = quotient(product(forward_prod_index[state], tmp),
		       sum_forward);
	const std::vector<FeatureID>& emission(transition.out(state));
	for ( std::vector<FeatureID>::const_iterator it = emission.begin();
	      it != emission.end();
	      ++ it ) {
	  state_weight_index[*it] += tmp;
	}
      }
    }

    for ( size_t state = 0; state < num_states; ++state ) {
      tmp = quotient(product(forward_prod[crf_length-1][state], backward_prod[crf_length-1][state]),
		     sum_forward);
      const std::vector<FeatureID>& emission(transition.out(state));
      for ( std::vector<FeatureID>::const_iterator it = emission.begin();
	    it != emission.end();
	    ++ it ) {
	state_weight[crf_length-1][*it] += tmp;
      }
    }
    //ParameterValue sum_backward = backward_prod[0][start_state];
    //std::cerr << "forward/backward = " << sum_forward.getAlpha() << "/" << sum_backward.getAlpha() << std::endl;
    return log_scale;
  }

  /// Sum of all inside products
  const ParameterValue& sumProduct() const {
    return sum_forward;
  }

  const std::vector<ParameterValue>& targetWeight(size_t index) const
  {
    return state_weight[index];
  }
};



template < class Feature, class ParameterValue, bool ReferenceProbability = false >
class CRFViterbi {
public:
  /// Feature frequency
  typedef typename Feature::FeatureFreq FeatureFreq;

private:
  const FixedTarget& fixed_target;
  const CRFTransition& transition;

  size_t max_length;
  size_t num_targets;
  size_t num_states;
  std::vector< std::vector< ParameterValue > > state_weight;
  std::vector< std::vector< ParameterValue > > forward_max;
  std::vector< std::vector< size_t > > backward_pointer;
  ParameterValue max_product;
  size_t crf_length;
  std::pair<size_t, size_t> start_state;
  std::pair<size_t, size_t> end_state;
  //std::vector< Real > sum_reference;

public:
  /// Constructor
  CRFViterbi(const FixedTarget& ft, const CRFTransition& crf)
    : fixed_target(ft), transition(crf)
  {
    num_targets = fixed_target.numTargets();
    num_states = transition.numStates();
  }
  /// Destructor
  ~CRFViterbi() {}

public:
  /// Initialize the tree of the size _id_ with alpha sets of the size _freq_
  /// Set the products of alpha values to the tree
  void setPath( const ParameterSpace<ParameterValue>& parameterspace,
		const EventCRF< Feature, ReferenceProbability >& event_crf )
  {
    //std::cerr << "start setProduct" << std::endl;
    //event_crf.getTransition().debugInfo(std::cerr);

    crf_length = event_crf.length();
    if ( crf_length == 0 ) return;
    if ( crf_length >= forward_max.size() ) {
      state_weight.resize(crf_length, std::vector<ParameterValue>(num_targets));
      forward_max.resize(crf_length, std::vector<ParameterValue>(num_states));
      backward_pointer.resize(crf_length, std::vector<size_t>(num_states));
    }
    
    start_state = std::pair<size_t, size_t>(0, event_crf.observedEventIDs().front());
    end_state = std::pair<size_t, size_t>(crf_length-1, event_crf.observedEventIDs().back());

    for ( size_t index = 0; index < crf_length; ++index ) {
      for ( size_t state = 0; state < num_states; ++state ) {
	forward_max[index][state].initAlpha(0.0);
	backward_pointer[index][state] = 0;
      }
      for ( size_t target = 0; target < num_targets; ++target ) {
	state_weight[index][target].initAlpha(1.0);
      }
    }


    for ( size_t index = 0; index < crf_length; ++index ) {
      const std::vector<Feature>& hist_list  = event_crf.getHistoryList()[index];
      for( typename std::vector<Feature>::const_iterator itr = hist_list.begin();
	   itr != hist_list.end();
	   ++itr ) {
	FeatureID hid = itr->id();
	typename Feature::FeatureFreq freq = itr->freq();
	const std::vector< std::pair<FeatureID, FeatureID> >& fv =
	  fixed_target.getFeatures( hid );
	ParameterValue::exponent_fix( parameterspace.getParameters(), fv, freq, state_weight[index]);
      }
    }

    forward_max[0][start_state.second].initAlpha(1.0);
    const std::vector<FeatureID>& initial_targets(transition.out(start_state.second));
    for ( std::vector<FeatureID>::const_iterator tt = initial_targets.begin();
	  tt != initial_targets.end();
	  ++tt ) {
      forward_max[0][start_state.second] *= state_weight[0][*tt];
    }
    
    for ( size_t index = 1; index < crf_length; ++index ) {
      for ( size_t state = 0; state < num_states; ++state ) {
	const std::vector<FeatureID>& prev(transition.prevStates(state));
	size_t max_id = 0;
	for ( std::vector<FeatureID>::const_iterator it = prev.begin();
	      it != prev.end();
	      ++it ) {
	  if ( forward_max[index-1][*it].getValue() > forward_max[index][state].getValue() ) {
	    max_id = *it;
	    forward_max[index][state] = forward_max[index-1][*it];
	  }
	}
	const std::vector<FeatureID>& targets(transition.out(state));
	for ( std::vector<FeatureID>::const_iterator tt = targets.begin();
	      tt != targets.end();
	      ++tt ) {
	  forward_max[index][state] *= state_weight[index][*tt];
	}
	backward_pointer[index][state] = max_id;
      }
    }
    max_product = forward_max[crf_length-1][end_state.second];
    //std::cerr << "max = " << max_product.getValue() << std::endl;
  }

  const ParameterValue& maxProduct() const {
    return max_product;
  }

  const std::pair<size_t, size_t>& rend()
  {
    return start_state;
  }

  const std::pair<size_t, size_t>& rbegin()
  {
    return end_state;
  }

  void next(std::pair<size_t, size_t>& ret)
  {
    if ( ret.first == 0 ) {
      throw WeightAccumulatorError("should not happen, CRFViterbi::next");
    }
    //    std::cerr << "ret " << ret.first << ", " << ret.second << std::endl;

    ret.second = backward_pointer[ret.first][ret.second];
    --ret.first;
  }
};

AMIS_NAMESPACE_END

#endif // Amis_CRFTable_h_
// end of CRFTable.h

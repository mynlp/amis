//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: TreeWeightAccumulator.h,v 1.2 2008-07-29 17:05:14 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_TreeWeightAccumulator_h_

#define Amis_TreeWeightAccumulator_h_

#include <amis/TreeWeightAccumulator.h>
#include <amis/Event.h>

#include <amis/AlphaTree.h>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/**
 * The class imports event data from Amis-style data files.
 * The Amis format enables non-binary features and packed events.
 * For details, see README.
 * @see Event, EventSpace, TreeWeightAccumulator
 */


template<class Feature, class ParameterValue, bool ReferenceProbability>
class TreeWeightAccumulator : public WeightAccumulator {
  AlphaTree< Feature, ParameterValue, ReferenceProbability > alpha_tree;
  ViterbiTree< Feature, ParameterValue, ReferenceProbability > viterbi_tree;
public:
  typedef EventTree<Feature, ReferenceProbability> EventType;
  TreeWeightAccumulator()
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
    Real log_likelihood = 0.0;
    const EventType& tree =
      dynamic_cast<const EventType&>(event_base);
    const ParameterSpace<ParameterValue>* parameterspace =
      dynamic_cast<const ParameterSpace<ParameterValue>*>(model.getParameterSpace());
    
    AMIS_DEBUG_MESSAGE( 5, "\t\tMake AlphaProdTree\n" );
    alpha_tree.setProduct( *parameterspace, tree );
    ParameterValue inv_sum = alpha_tree.sumProduct();
    AMIS_DEBUG_MESSAGE( 5, "\t\t  Sum = " << inv_sum.getAlpha() << '\n' );

    if ( ! finite( inv_sum.getValue() ) ) {
      this->incIgnored();
      //OStringStream oss;
      //oss << "IN: " << __PRETTY_FUNCTION__ << "sum is not finite (" << inv_sum.getValue() << "). this should not occur";
      //AMIS_WARNING_MESSAGE( oss.str() << '\n' );
      return 0.0;
    }

    inv_sum.reciprocal();
    ParameterValue denom = inv_sum;
    denom *= tree.eventFrequency() * event_empirical_expectation;

    //if ( EnableJointProb ) denom *= tree.eventProbability();
    //cerr << "denom=" << denom << endl;

    AMIS_DEBUG_MESSAGE( 5, "\t\tSet model expectations for features\n" );
    ParameterValue alpha_product;
    for ( EventTreeNodeID i = 0; i < tree.numEventTreeNodes(); ++i ) {
      const EventTreeNode< Feature >& event_node = tree[ i ];
      if ( ! event_node.isDisjunctiveNode() ) {
	alpha_product = product( alpha_tree.insideProd( i ),
				 alpha_tree.outsideProd( i ) );
	AMIS_DEBUG_MESSAGE( 5, alpha_product << std::endl );
	alpha_product *= denom;
	for ( typename FeatureList< Feature >::const_iterator feature = event_node.featureList().begin();
	      feature != event_node.featureList().end();
	      ++feature ) {
	  me[feature->id()] += alpha_product.getAlpha() * feature->freq();
	}
      }
    }
    ParameterValue lp;
    lp.initValue(ParameterValue::exponent( parameterspace->getParameters(), tree.observedEvent() ));
    lp *= inv_sum;
    if ( ReferenceProbability ) {
      lp *= tree.observedEvent().referenceProbability();
    }
    log_likelihood += event_empirical_expectation * tree.eventFrequency() * lp.safeGetLambda();
    return log_likelihood;
  }





  void perceptronUpdate(const EventBase& event_base,
			Real step_size,
			int iteration,
			ModelBase& model,
			std::vector<Real>& history)
  {
    const EventType& tree =
      dynamic_cast<const EventType&>(event_base);
    const ParameterSpace<ParameterValue>* parameterspace =
      dynamic_cast<const ParameterSpace<ParameterValue>*>(model.getParameterSpace());
    
    viterbi_tree.setPath( *parameterspace, tree );
    ParameterValue max = viterbi_tree.maxProduct();
    //std::cerr << "max = " << max.getAlpha() << std::endl;

    if ( ! finite( max.getValue() ) ) {
      this->incIgnored();
      //OStringStream oss;
      //oss << "IN: " << __PRETTY_FUNCTION__ << "sum is not finite (" << max.getValue() << "). this should not occur";
      //AMIS_WARNING_MESSAGE( oss.str() << '\n' );
      return;
    }

    for ( EventTreeNodeID i = 0; i < tree.numEventTreeNodes(); ++i ) {
      const EventTreeNode< Feature >& event_node = tree[ i ];
      if ( ! event_node.isDisjunctiveNode() &&
	   viterbi_tree.isMaximizer(i) ) {
	for ( typename FeatureList< Feature >::const_iterator feature = event_node.featureList().begin();
	      feature != event_node.featureList().end();
	      ++feature ) {
	  Real incr = -feature->freq() * tree.eventFrequency() * step_size;
	  model.incLambda(feature->id(), incr);
	  history[feature->id()] += static_cast<double>(iteration) * incr;
	}
      }
    }
    const FeatureList< Feature >& observed = tree.observedEvent();
    for ( typename FeatureList< Feature >::const_iterator feature = observed.begin();
	  feature != observed.end();
	  ++feature ) {
      Real incr = feature->freq() * tree.eventFrequency() * step_size;
      model.incLambda(feature->id(), incr);
      history[feature->id()] += static_cast<double>(iteration) * incr;
    }
  }

};



AMIS_NAMESPACE_END

#endif // Amis_TreeWeightAccumulator_h_
// end of TreeWeightAccumulator.h

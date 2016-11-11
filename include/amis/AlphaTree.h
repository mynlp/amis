//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: AlphaTree.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_AlphaTree_h_

#define Amis_AlphaTree_h_

#include <amis/configure.h>
#include <amis/EventTree.h>
#include <vector>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/**
 * Partially calculated alpha product values are stored in a tree
 * structure according to the tree structure of an event.
 */

template < class Feature, class ParameterValue, bool ReferenceProbability = false >
class AlphaTree {
public:
  /// Feature frequency
  typedef typename Feature::FeatureFreq FeatureFreq;

private:
  std::vector< ParameterValue > inside_prod;
  std::vector< ParameterValue > outside_prod;
  ParameterValue sum_product;
  //std::vector< Real > sum_reference;

public:
  /// Constructor
  AlphaTree()
  {
    sum_product.clear();
  }
  /// Destructor
  ~AlphaTree() {}

public:
  /// Initialize the tree of the size _id_ with alpha sets of the size _freq_
  /*  void initialize( EventTreeNodeID id) {
    max_id = id;
    inside_prod.resize( max_id );
    outside_prod.resize( max_id );
    sum_product.clear();
    }*/

  /// Set the products of alpha values to the tree
  void setProduct( const ParameterSpace<ParameterValue>& parameterspace,
		   const EventTree< Feature, ReferenceProbability >& event_tree )
  {
    size_t max_id = event_tree.numEventTreeNodes() + 1;
    if ( max_id >= inside_prod.size() ) {
      inside_prod.resize(max_id);
      outside_prod.resize(max_id);
    }
    // Initialize & inside product
    //cerr << "inside prod" << std::endl;
    typename std::vector< ParameterValue >::iterator inside_it = inside_prod.begin();
    typename std::vector< ParameterValue >::iterator outside_it = outside_prod.begin();
    //typename std::vector< Real >::iterator sum_ref_it = sum_reference.begin();
    for ( typename EventTree< Feature, ReferenceProbability >::const_iterator event_node = event_tree.begin();
          event_node != event_tree.end();
          ++event_node, ++inside_it, ++outside_it ) {
      outside_it->clear(); // initialize outside products
      if ( event_node->isDisjunctiveNode() ) {
        AMIS_PROF3( "AlphaTree::inside_prod(disj)" );
	inside_it->accumulate( inside_prod, event_node->daughterList().begin(), event_node->daughterList().end() );
      }
      else {
        AMIS_PROF3( "AlphaTree:inside_prod(conj)" );
        // conjunctive node
        inside_it->initValue( ParameterValue::exponent( parameterspace.getParameters(), event_node->featureList() ) );
        if ( ReferenceProbability ) {
          *inside_it *= event_node->featureList().referenceProbability();
        }
        for ( EventTreeNodeIDList::const_iterator dtr = event_node->daughterList().begin();
              dtr != event_node->daughterList().end();
              ++dtr ) {
          *inside_it *= inside_prod[ *dtr ];
        }
      }
    }
    // summation of products
    --inside_it;
    sum_product = *inside_it;

    --outside_it;// --alpha_it;
    outside_it->initAlpha( 1.0 );
    for ( typename EventTree< Feature, ReferenceProbability >::const_reverse_iterator event_node = event_tree.rbegin();
          event_node != event_tree.rend();
          ++event_node, --outside_it, --inside_it ) {
      if ( event_node->isDisjunctiveNode() ) {
        AMIS_PROF3( "AlphaTree::outside_prod(disj)" );
        // disjunctive node
        for ( EventTreeNodeIDList::const_iterator dtr = event_node->daughterList().begin();
              dtr != event_node->daughterList().end();
              ++dtr ) {
          outside_prod[ *dtr ] += *outside_it;
        }
      }
      else {
        AMIS_PROF3( "AlphaTree::outside_prod(conj)" );
        // conjunctive node
        //outside_it->accumulate( outside_prod, event_node->motherList(), max_freq );
        // optimized version
        const EventTreeNodeIDList& daughters = event_node->daughterList();
        if ( ! daughters.empty() ) {
	  ParameterValue inside_outside;
	  //inside_outside.product(*outside_it, *inside_it);
	  inside_outside = product(*outside_it, *inside_it);
          for ( size_t id = 0; id < daughters.size(); ++id ) {
	    if (!inside_prod[ daughters[ id ] ].isZero() ) {
	      //std::cerr << outside_prod[daughters[id]].getValue() << ", ";
	      outside_prod[ daughters[ id ] ].incQuotient( inside_outside,
							   inside_prod[ daughters[ id ] ]);
	      //std::cerr << inside_outside.getValue() << ", "
	      //<< inside_prod[daughters[ id ]].getValue() << ", "
	      //<< outside_prod[ daughters[ id ] ].getValue() << std::endl;
	    }
          }
        }
      }
    }
    //cerr << "done." << std::endl;
  }

  /// Get the inside product of id-th node
  const ParameterValue& insideProd( EventTreeNodeID id ) const {
    return inside_prod[ id ];
  }
  /// Get the outside product of id-th node
  const ParameterValue& outsideProd( EventTreeNodeID id ) const {
    return outside_prod[ id ];
  }
  /// Sum of all inside products
  const ParameterValue& sumProduct() const {
    return sum_product;
  }
};





template < class Feature, class ParameterValue, bool ReferenceProbability = false >
class ViterbiTree {
public:
  /// Feature frequency
  typedef EventTree<Feature, ReferenceProbability> EventType;
  typedef typename Feature::FeatureFreq FeatureFreq;

private:
  std::vector< ParameterValue > inside_prod;
  std::vector< EventTreeNodeID > max_dtr;
  std::vector< bool > maximizers;
  ParameterValue max_product;
  //std::vector< Real > sum_reference;

public:
  /// Constructor
  ViterbiTree()
  {
    max_product.clear();
  }
  /// Destructor
  ~ViterbiTree() {}

public:
  /// Set the products of alpha values to the tree
  void setPath( const ParameterSpace<ParameterValue>& parameterspace,
		const EventType& event_tree )
  {
    size_t max_id = event_tree.numEventTreeNodes() + 1;
    if ( max_id >= inside_prod.size() ) {
      inside_prod.resize(max_id);
      max_dtr.resize(max_id);
      maximizers.resize(max_id);
    }
    // Initialize & inside product
    //cerr << "inside prod" << std::endl;
    typename std::vector< ParameterValue >::iterator inside_it = inside_prod.begin();
    typename std::vector< EventTreeNodeID >::iterator max_it = max_dtr.begin();
    std::vector< bool >::iterator check_it = maximizers.begin();
    //typename std::vector< Real >::iterator sum_ref_it = sum_reference.begin();
    for ( typename EventTree< Feature, ReferenceProbability >::const_iterator event_node = event_tree.begin();
          event_node != event_tree.end();
          ++event_node, ++inside_it, ++max_it, ++check_it ) {
      *max_it = 0;
      *check_it = false;
      if ( event_node->isDisjunctiveNode() ) {
        AMIS_PROF3( "AlphaTree::inside_prod(disj)" );
	const EventTreeNodeIDList& dtrs = event_node->daughterList();
	inside_it->clear();
	
	for ( EventTreeNodeIDList::const_iterator it = dtrs.begin();
	      it != dtrs.end();
	      ++it ) {
	  if ( inside_prod[*it].getValue() > inside_it->getValue() ) {
	    *inside_it = inside_prod[*it];
	    *max_it = *it;
	  }
	}
      }
      else {
        AMIS_PROF3( "AlphaTree:inside_prod(conj)" );
        // conjunctive node
        inside_it->initValue( ParameterValue::exponent( parameterspace.getParameters(), event_node->featureList() ) );
        if ( ReferenceProbability ) {
          *inside_it *= event_node->featureList().referenceProbability();
        }
        for ( EventTreeNodeIDList::const_iterator dtr = event_node->daughterList().begin();
              dtr != event_node->daughterList().end();
              ++dtr ) {
          *inside_it *= inside_prod[*dtr];
        }
      }
    }
    // summation of products
    --inside_it;
    max_product = *inside_it;

    --max_it;
    --check_it;// --alpha_it;
    *check_it = true;
    for ( typename EventTree< Feature, ReferenceProbability >::const_reverse_iterator event_node = event_tree.rbegin();
          event_node != event_tree.rend();
          ++event_node, --max_it, --check_it ) {
      if ( *check_it ) {
	if ( event_node->isDisjunctiveNode() ) {
	  AMIS_PROF3( "AlphaTree::outside_prod(disj)" );
	  // disjunctive node
	  maximizers[*max_it] = true;
	}
	else {
	  AMIS_PROF3( "AlphaTree::outside_prod(conj)" );
	  const EventTreeNodeIDList& daughters = event_node->daughterList();
	  for ( EventTreeNodeIDList::const_iterator it = daughters.begin();
		it != daughters.end();
		++it ) {
	    maximizers[*it] = true;
	  }
	}
      }
    }
    //cerr << "done." << std::endl;
  }

  /// Get the inside product of id-th node
  bool isMaximizer( EventTreeNodeID id ) const {
    return maximizers[ id ];
  }

  /// Sum of all inside products
  const ParameterValue& maxProduct() const {
    return max_product;
  }
};



AMIS_NAMESPACE_END

#endif // Amis_AlphaTree_h_
// end of AlphaTree.h

//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: EventTree.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_EventTree_h_

#define Amis_EventTree_h_

#include <amis/configure.h>
#include <amis/EventBase.h>
#include <amis/Event.h>
#include <amis/FeatureList.h>
#include <amis/objstream.h>
#include <amis/Array.h>

#include <vector>

AMIS_NAMESPACE_BEGIN

typedef unsigned int EventTreeNodeID;
typedef Array< EventTreeNodeID > EventTreeNodeIDList;

//////////////////////////////////////////////////////////////////////

/// <classdef>
/// <name>EventTreeNode</name>
/// <overview>A node of an event tree</overview>
/// <desc>
/// A node in an event tree (conjunctive/disjunctive).
/// if 'feature_list' is NULL, it is a disjunctive node.
/// </desc>
/// <body>

template < class Feature >
class EventTreeNode {
private:
  bool disjunctive;
  FeatureList< Feature > feature_list;
  EventTreeNodeIDList daughter_list;

  public:
  typedef typename FeatureList<Feature>::const_iterator const_iterator;
  
  const_iterator begin() const {
    return feature_list->begin();
  }
  const_iterator end() const {
    return feature_list->end();
  }
  
public:
  EventTreeNode() {
    disjunctive = true;
  }
  EventTreeNode( const EventTreeNode< Feature >& n )
    : daughter_list( n.daughter_list ) {
    if ( n.disjunctive ) {
      disjunctive = true;
    } else {
      disjunctive = false;
      feature_list = FeatureList< Feature >( n.feature_list );
    }
  }
  EventTreeNode< Feature >& operator=( const EventTreeNode< Feature >& n ) {
    daughter_list = n.daughter_list;
    //if ( !disjunctive ) delete feature_list;
    if ( n.disjunctive ) {
      disjunctive = true;
    } else {
      disjunctive = false;
      feature_list = FeatureList< Feature >( n.feature_list );
    }
    return (*this);
  }

  EventTreeNode( const std::vector< Feature >& fl, const std::vector< EventTreeNodeID >& dl )
    : daughter_list( dl ) {
    // conjunctive node
    disjunctive = false;
    feature_list = FeatureList< Feature >( fl, 1 );
  }
  EventTreeNode( const std::vector< EventTreeNodeID >& dl )
    : daughter_list( dl ) {
    // disjunctive node
    disjunctive = true;
  }
  ~EventTreeNode() {
    //if ( feature_list != NULL ) {
    //delete feature_list;
    //}
  }

public:
  bool isDisjunctiveNode() const {
    return disjunctive;
  }
  const FeatureList< Feature >& featureList() const {
    return feature_list;
  }
  FeatureList< Feature >& featureList() {
    return feature_list;
  }
  const EventTreeNodeIDList& daughterList() const {
    return daughter_list;
  }

public:
  void writeObject( objstream& os ) const {
    if ( disjunctive ) {
      // Disjunctive node
      os << false;
    } else {
      // Conjunctive node
      os << true;
      feature_list.writeObject( os );
    }
    os << static_cast< EventTreeNodeID >( daughter_list.size() );
    for ( EventTreeNodeIDList::const_iterator it = daughter_list.begin();
          it != daughter_list.end();
          ++it ) {
      os << *it;
    }
  }
  void readObject( objstream& is ) {
    bool is_conj;
    is >> is_conj;
    if ( is_conj ) {
      // Conjunctive node
      //if ( feature_list.empty() ) feature_list = FeatureList< Feature >();
      disjunctive = false;
      feature_list.readObject( is );
    } else {
      disjunctive = true;
      //if ( !feature_list.empty() ) {
      //feature_list = FeatureList< Feature >();
      //}
    }
    EventTreeNodeID max, id;
    is >> max;
    daughter_list.resize( max );
    for ( EventTreeNodeIDList::iterator it = daughter_list.begin();
          it != daughter_list.end();
          ++it ) {
      is >> id;
      *it = id;
    }
  }
};

/// </body>
/// </classdef>

//////////////////////////////////////////////////////////////////////

/// <classdef>
/// <name>EventTree</name>
/// <overview>An event with tree structure</overview>
/// <desc>
/// An event (list of list of features) is described with packed tree structure
/// (a feature forest).
/// A forest is represented with a set of nodes, and relations from a node
/// to daughters.
/// Since 'newEventTreeNode' is called with DaughterList (a relation from a node
/// to daughters), a new ID is assigned after all daughters are registered.
/// This means that a node list is sorted in a bottom-up manner.
/// When you traverse a forest in a bottom-up manner, you should just 
/// scan the node list from the beginning to the end.
/// At the same time, the node list is stored in the order of "last visit",
/// so the node list is 'topological-sort'ed.
/// When you traverse a forest in a top-down manner, you should just
/// scan the node list from the end to the beginning.
/// </desc>
/// <body>

template < class Feature, bool ReferenceProbability >
class EventTree : public EventBase {
public:
  typedef Feature FeatureType;
  typedef typename Feature::FeatureFreq FeatureFreq;
  typedef typename std::vector< EventTreeNode< Feature > >::const_iterator const_iterator;
  typedef typename std::vector< EventTreeNode< Feature > >::const_reverse_iterator const_reverse_iterator;

private:
  FeatureList< Feature > observed_feature_list;
  std::vector< EventTreeNode< Feature > > node_list;

public:
  const_iterator begin() const {
    return node_list.begin();
  }
  const_iterator end() const {
    return node_list.end();
  }
  const_reverse_iterator rbegin() const {
    return node_list.rbegin();
  }
  const_reverse_iterator rend() const {
    return node_list.rend();
  }

public:
  EventTree() {
    freq_count = 0;
  }

  void swap( EventTree< Feature, ReferenceProbability >& event ) {
    std::swap( freq_count, event.freq_count );
    observed_feature_list.swap( event.observed_feature_list );
    node_list.swap( event.node_list );
  }

  Real eventProbability() const {
    return 1.0;
  }
  void setEventProbability( Real p ) { }

  ////////////////////////////////////////////////////////////

  void addObservedEvent( int f, const std::vector< Feature >& fl ) {
    if ( freq_count != 0 ) throw IllegalEventError( "Observed feature list is already added" );
    freq_count = f;
    observed_feature_list = FeatureList< Feature >( fl, 1 );
  }

  EventTreeNodeID newConjunctiveNode( const std::vector< Feature >& fl, const std::vector< EventTreeNodeID >& dl ) {
    EventTreeNodeID new_id = node_list.size();
    node_list.push_back( EventTreeNode< Feature >( fl, dl ) );
    return new_id;
  }
  EventTreeNodeID newDisjunctiveNode( const std::vector< EventTreeNodeID >& dl ) {
    EventTreeNodeID new_id = node_list.size();
    node_list.push_back( EventTreeNode< Feature >( dl ) );
    return new_id;
  }

  void clear() {
    freq_count = 0;
    node_list.resize(0);
  }

  const FeatureList< Feature >& observedEvent() const {
    return observed_feature_list;
  }
  FeatureList< Feature >& observedEvent() {
    return observed_feature_list;
  }

  EventTreeNodeID numEventTreeNodes() const {
    return node_list.size();
  }
  size_t numFeatureList() const
  {
    // Since # feature lists is meaningless in this model,
    // so return # tree nodes.
    return numEventTreeNodes();
  }
  const EventTreeNode< Feature >& operator[]( EventTreeNodeID id ) const {
    return node_list[ id ];
  }
  EventTreeNode< Feature >& operator[]( EventTreeNodeID id ) {
    return node_list[ id ];
  }

  ////////////////////////////////////////////////////////////

  Real maxFeatureCount() const
  {
    std::vector< FeatureFreq > max_count_list( numEventTreeNodes() );
    typename std::vector< EventTreeNode< Feature > >::const_iterator node = begin();
    typename std::vector< FeatureFreq >::iterator max_count = max_count_list.begin();
    for ( ; node != end(); ++node, ++max_count ) {
      if ( node->isDisjunctiveNode() ) {
        // disjunctive node
        FeatureFreq count = Feature::MIN_FEATURE_FREQ;
        for ( EventTreeNodeIDList::const_iterator dtr = node->daughterList().begin();
              dtr != node->daughterList().end();
              ++dtr ) {
          count = std::max( count, max_count_list[ *dtr ] );
        }
        *max_count = count;
      } else {
        // conjunctive node
        FeatureFreq count = node->featureList().featureCount();
        for ( EventTreeNodeIDList::const_iterator dtr = node->daughterList().begin();
              dtr != node->daughterList().end();
              ++dtr ) {
          count += max_count_list[ *dtr ];
        }
        *max_count = count;
      }
    }
    return max_count_list.back();
  }

  //////////////////////////////////////////////////////////////////////

  void setEmpiricalExpectation( const ModelBase& model, std::vector< Real >& empirical_expectation ) const {
    AMIS_DEBUG_MESSAGE( 5, "Feature loop\n" );
    const FeatureList< Feature >& fl = observedEvent();
    Real freq = eventFrequency();
    //if ( EnableJointProb ) freq *= eventProbability();
    for ( typename FeatureList< Feature >::const_iterator feature = fl.begin();
	  feature != fl.end();
	  ++feature ) {
      AMIS_DEBUG_MESSAGE( 5, "feature=" << feature->id() << '\n' );
      empirical_expectation[ feature->id() ] += static_cast< Real >( feature->freq() * freq );
    }
  }

  //////////////////////////////////////////////////////////////////////

public:
  void writeObject( objstream& os ) const {
    os << freq_count;
    observed_feature_list.writeObject( os );
    os << static_cast< EventTreeNodeID >( node_list.size() );
    for ( typename std::vector< EventTreeNode< Feature > >::const_iterator it = node_list.begin();
          it != node_list.end();
          ++it ) {
      it->writeObject( os );
    }
  }
  void readObject( objstream& is ) {
    is >> freq_count;
    observed_feature_list.readObject( is );
    EventTreeNodeID max;
    is >> max;
    node_list.resize( max );
    for ( typename std::vector< EventTreeNode< Feature > >::iterator it = node_list.begin();
          it != node_list.end();
          ++it ) {
      it->readObject( is );
    }
  }
};

/// </body>
/// </classdef>

AMIS_NAMESPACE_END
#endif // EventTree_h_
// end of EventTree.h

//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: Event.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_Event_h_

#define Amis_Event_h_

#include <amis/configure.h>
#include <amis/FeatureList.h>
#include <amis/EventBase.h>
#include <amis/ErrorBase.h>
#include <amis/Real.h>
#include <amis/ModelBase.h>
#include <amis/objstream.h>
#include <amis/StringStream.h>
#include <amis/Tokenizer.h>
#include <amis/EventReader.h>

#include <vector>
#include <algorithm>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/// <classdef>
/// <name>Event</name>
/// <overview>An event</overview>
/// <desc>
/// This class describes an event object.
/// </desc>
/// <body>

template < class Feature, bool ReferenceProbability >
class Event : public EventBase {
public:
  typedef Feature FeatureType;
  typedef typename Feature::FeatureFreq FeatureFreq;

  //typedef FeatureList< Feature >* iterator;
  //typedef const FeatureList< Feature >* const_iterator;

  typedef typename std::vector< FeatureList<Feature> >::iterator iterator;
  typedef typename std::vector< FeatureList<Feature> >::const_iterator const_iterator;
  typedef typename std::vector< FeatureList<Feature> >::size_type size_type;
  
 
  iterator begin() {
    return feature_lists.begin();
  }
  
  const_iterator begin() const {
    return feature_lists.begin();
  }
  
  
  iterator end() {
    return feature_lists.end();
  }
  
  
  const_iterator end() const {
    return feature_lists.end();
  }

private:
  std::vector< FeatureList< Feature > > feature_lists;
  int active_id;
  static const int NULL_ACTIVE_ID = -1;

public:
  Event() {
    freq_count = 0;
    active_id = NULL_ACTIVE_ID;
  }

  ~Event() {}

  void clear() {
    freq_count = 0;
    active_id = NULL_ACTIVE_ID;
    feature_lists.resize( 0 );
  }
  
  void swap( Event< Feature, ReferenceProbability >& event ) {
    std::swap( freq_count, event.freq_count );
    feature_lists.swap( event.feature_lists );
    std::swap( active_id, event.active_id );
  }
  
  
  void addFeatureList( const std::vector< Feature >& vec, int f ) {
    if ( f > 0 ) {
      if ( active_id != NULL_ACTIVE_ID ) throw IllegalEventError( "Ambiguous event is disallowed" );
      active_id = feature_lists.size();
      feature_lists.push_back( FeatureList< Feature >( vec, 1 ) );
      freq_count = f;
    }
    else if ( f == 0 ) {
      feature_lists.push_back( FeatureList< Feature >( vec, 0 ) );
    }
    else {
      throw IllegalEventError( "Event frequency must be positive" );
    }
  }

  const FeatureList< Feature >& observedEvent() const {
    return feature_lists[ active_id ];
  }
  
  int observedEventID() const {
    return active_id;
  }

  size_t size() const {
    return feature_lists.size();
  }
  size_t numFeatureList() const {
    return size();
  }
  const int eventFrequency() const {
    return freq_count;
  }

  const FeatureList< Feature >& operator[]( size_t i ) const {
    return feature_lists[ i ];
  }

  Real eventProbability() const {
    return 1.0;
  }
  void setEventProbability( Real p ) { }

  Real featureCount( size_t id ) const
  {
    return (*this)[ id ].featureCount();
  }

  Real maxFeatureCount() const
  {
    FeatureFreq freq = Feature::MIN_FEATURE_FREQ;
    for ( const_iterator it = begin(); it != end(); ++it ) {
      freq = std::max( freq, it->featureCount() );
    }
    return freq;
  }

  bool isObserved( size_t i ) const
  {
    if ( active_id == NULL_ACTIVE_ID ) throw IllegalEventError( "Observed feature list not found" );
    return i == active_id;
  }

  bool isObserved( const_iterator fl ) const
  {
    return isObserved( fl - begin() );
  }

  ////////////////////////////////////////////////////////////

public:

  void readObject( objstream& is ) {
    is >> freq_count;
    size_t num_fl;
    is >> num_fl;
    is >> active_id;
    feature_lists.resize( num_fl );
    for ( iterator it = begin();
          it != end();
          ++it ) {
      it->readObject( is );
    }
  }
  /// Read an object from a stream
  void writeObject( objstream& os ) const {
    os << freq_count;
    os << feature_lists.size();
    os << active_id;
    for ( const_iterator it = begin();
          it != end();
          ++it ) {
      it->writeObject( os );
    }
  }
  /// Write an object into a stream
};

AMIS_NAMESPACE_END

/// </body>
/// </classdef>

#endif // Event_h_
// end of Event.h

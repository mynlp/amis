//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: FeatureList.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_FeatureList_h_

#define Amis_FeatureList_h_

#include <amis/configure.h>
#include <amis/Feature.h>
#include <amis/objstream.h>
#include <amis/Array.h>

#include <vector>
#include <algorithm>
#include <memory>

AMIS_NAMESPACE_BEGIN

class IllegalFeatureListError : public ErrorBase {
 public:
  IllegalFeatureListError( const std::string& s ) : ErrorBase( s ) {}
  IllegalFeatureListError( const char* s ) : ErrorBase( s ) {}
};

//////////////////////////////////////////////////////////////////////

/// <classdef>
/// <name>FeatureList</name>
/// <overview>A set of activated features</overview>
/// <desc>
/// This class describes a set of activated features for an event.
/// </desc>
/// <body>

template < class Feature, class Alloc = Allocator< Feature > >
class FeatureList {
public:
  typedef typename Feature::FeatureFreq FeatureFreq;
  typedef Alloc allocator_type;
  typedef typename allocator_type::size_type size_type;

private:
  FeatureFreq count;
  Array< Feature > features;
  
  Real prob;  // reference probability

public:
  typedef typename Array< Feature >::const_iterator const_iterator;
 
  const_iterator begin() const { return features.begin(); }
  const_iterator end() const { return features.end(); }
  
public:
  size_type size() const { return features.size(); }

  FeatureList() : count( 0 ), features(), prob( 1.0 ) {
  }

  FeatureList( const std::vector< Feature >& vec, Real p = 1.0, bool do_sort = false) : count( 0 ), features( vec ), prob( p ) {
    // sort history list according to the history id.
    // may be junt an overhead, but needed for the fast kernel calculation 

    for ( const_iterator it = begin(); it != end(); ++it ) {
      count += it->freq();
    }
    
  }
  
  
  FeatureList& copy_and_set( const std::vector<Feature>& v, Real p = 1.0, bool do_sort = false ) {
    // sort history list according to the history id.
    // may be junt an overhead, but needed for the fast kernel calculation 

    features = Array< Feature >( v );
    prob = p;
    
    count = 0;
    for ( const_iterator it = begin(); it != end(); ++it ) {
      count += it->freq();
    }

    return *this;
  }
  
  /*
  void swap( FeatureList< Feature >& fl ) {
    ::swap( count, fl.count );
    ::swap( features, fl.features );
    ::swap( tail, fl.tail );
    ::swap( prob, fl.prob );
  }
  */
  
  void swap( FeatureList< Feature >& fl ) {
    std::swap( count, fl.count );
    features.swap( fl.features );
    std::swap( prob, fl.prob );
  }
  
  FeatureFreq featureCount() const {
    return count;
  }
  //FeatureListFreq frequency() const {
  //return 1;
  //}

  void setReferenceProbability( Real p ) {  
    prob = p;
  }
  
  Real referenceProbability() const {
    return prob;
  }

public:
  void readObject( objstream& is ) {
    //if ( features != NULL ) deallocate( features, tail - features );
    is >> count;
    features.readObject( is );
    is >> prob;
  }

  void writeObject( objstream& os ) const {
    os << count;
    features.writeObject( os );
    os << prob;
  }
};

AMIS_NAMESPACE_END

/// </body>
/// </classdef>

#endif // FeatureList_h_
// end of FeatureList.h

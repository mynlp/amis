//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: EventFix.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_EventFix_h_

#define Amis_EventFix_h_

#include <amis/configure.h>
#include <amis/EventBase.h>
#include <amis/Event.h>
#include <amis/FeatureList.h>
#include <amis/FixedTarget.h>
#include <amis/objstream.h>
#include <amis/PropertyItem.h>
#include <vector>

AMIS_NAMESPACE_BEGIN


template < class Feature_, bool ReferenceProbability >
class EventFix : public EventBase {
public:
  typedef Feature_ Feature;
  typedef typename Feature::FeatureFreq FeatureFreq;
  typedef EventFix< Feature, ReferenceProbability > Self;
  typedef Feature_ FeatureType;
  typedef typename std::vector< Feature >::size_type size_type;

private:
  FeatureList<Feature>    history_list;
  FeatureList<Feature>    observed_list;
  FeatureID               active_target;
  std::vector<Real> prob; // reference probability

public:
  EventFix() 
  {
    freq_count  = 0;
    active_target = 0;
  }

  virtual ~EventFix(){  }

  bool isValid() const { return true; }

  size_t size() const { return prob.size(); }

  size_t numFeatureList() const { return prob.size(); }

  size_t numTargets() const
  {
    return prob.size();
  }

  size_t historySize() const { return history_list.size(); }

  FeatureID observedEventID() const { return active_target; }
  
  const FeatureList<Feature>& observedEvent() const {
    return observed_list; 
  }
  
  const FeatureList<Feature>& getHistoryList() const { 
    return history_list;
  }
  
  
  void genObservedFeatureList( FixedTarget fixed_target,
			       FeatureList<Feature>& to ) const {
	
    const FeatureList<Feature>& hist_list  = getHistoryList();
    std::vector<Feature> feature_vec( hist_list.size() );
    feature_vec.resize( 0 );
	  
    for ( typename FeatureList<Feature>::const_iterator itr = hist_list.begin(); itr != hist_list.end(); ++itr ) {
      FeatureID   hid   = itr->id();
      FeatureFreq freq  = itr->freq();
		  
      const std::vector< std::pair<FeatureID, FeatureID> >& fv = fixed_target.getFeatures( hid );
		  
      for ( size_t k = 0; k < fv.size(); k++ ) {
	FeatureID tid = fv[k].first;
	if ( tid == active_target ) {
	  FeatureID fid = fv[k].second;
          //AMIS_DEBUG_MESSAGE( 2, "IN: " << __PRETTY_FUNCTION__ << " active_tid=" << tid << "active_fid=" << fid <<  '\n' );
	  feature_vec.push_back( Feature(fid, freq) );
	}
      }
    }
	  
    to.copy_and_set( feature_vec, eventFrequency() );
  }
  
  void add( FixedTarget fixed_target,
            int f,
	    FeatureID tid,
            const FeatureList<Feature>& hl) {
    freq_count          = f;
    history_list  = hl;
    active_target = tid;
    prob.resize( fixed_target.numTargets() );
    genObservedFeatureList( fixed_target, observed_list );
  }

  Real eventProbability() const {
    return 1.0;
  }

  Real referenceProbability( size_t i ) const {
    return prob[ i ];
  }

  Real featureCount( FeatureID tid ) const {
    return history_list.featureCount();
  }

  Real maxFeatureCount( void ) const {
    return history_list.featureCount();
  }

  //////////////////////////////////////////////////////////////////////

public:
  void readObject( objstream& is ) {
    is >> freq_count;
    is >> active_target;
    history_list.readObject( is );
    observed_list.readObject( is );
    size_t num_targets = 0;
    is >> num_targets;
    prob.resize( num_targets );
    for ( std::vector< Real >::iterator it = prob.begin();
	  it != prob.end();
	  ++it ) {
      is >> *it;
    }
  }
  /// Read an object from a stream

  void writeObject( objstream& os ) const {
    os << freq_count;
    os << active_target;
    history_list.writeObject( os );
    observed_list.writeObject( os );
    os << prob.size();
    for ( std::vector< Real >::const_iterator it = prob.begin();
	  it != prob.end();
	  ++it ) {
      os << *it;
    }
  }
  /// Write an object into a stream
};

AMIS_NAMESPACE_END

#endif

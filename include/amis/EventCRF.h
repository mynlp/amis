//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: EventCRF.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_EventCRF_h_

#define Amis_EventCRF_h_

#include <amis/configure.h>
#include <amis/EventBase.h>
#include <amis/Event.h>
#include <amis/FeatureList.h>
#include <amis/FixedTarget.h>
#include <amis/CRFTransition.h>
#include <amis/objstream.h>
#include <amis/PropertyItem.h>
#include <vector>

AMIS_NAMESPACE_BEGIN


template < class Feature_, bool ReferenceProbability >
class EventCRF : public EventBase {
public:
  typedef Feature_ Feature;
  typedef typename Feature::FeatureFreq FeatureFreq;
  typedef EventCRF< Feature, ReferenceProbability > Self;
  typedef Feature_ FeatureType;
  typedef typename std::vector< Feature >::size_type size_type;

private:
  std::vector<std::vector<Feature> >   history_list;
  FeatureList<Feature>                 observed_list;
  std::vector<FeatureID>               active_list;
  //std::vector<Real> prob; // reference probability

public:
  EventCRF() 
  {
    freq_count  = 0;
  }

  virtual ~EventCRF(){  }

  bool isValid() const { return true; }

  size_t numFeatureList() const
  {
    return history_list.size();
  }

  size_t length() const {
    return history_list.size();
  }
  //size_t numTargets() const
  //{
  //return prob.size();
  //}

  //size_t historySize() const { return history_list.size(); }

  const std::vector<FeatureID>& observedEventIDs() const { return active_list; }
  
  const FeatureList<Feature>& observedEvent() const {
   return observed_list; 
  }
  
  const std::vector<std::vector<Feature> >& getHistoryList() const { 
    return history_list;
  }
  
  
  void genObservedFeatureList( const FixedTarget& fixed_target, 
			       const CRFTransition& crf_transition,
			       FeatureList<Feature>& to ) const
  {
	
    const std::vector<std::vector<Feature> >& hist_list  = getHistoryList();
    std::vector<Feature> feature_vec;

    for ( size_t i = 0; i < hist_list.size(); ++i ) {
      FeatureID active_state = active_list[i];
      const std::vector<FeatureID>& active_targets = crf_transition.out(active_state);
      for ( typename std::vector<Feature>::const_iterator itr = hist_list[i].begin(); itr != hist_list[i].end(); ++itr ) {
	FeatureID   hid   = itr->id();
	FeatureFreq freq  = itr->freq();
	
	const std::vector< std::pair<FeatureID, FeatureID> >& fv = fixed_target.getFeatures( hid );
      
	for ( size_t k = 0; k < fv.size(); k++ ) {
	  FeatureID tid = fv[k].first;
	  if ( std::find(active_targets.begin(), active_targets.end(), tid) != active_targets.end() ) {
	    FeatureID fid = fv[k].second;
	    //AMIS_DEBUG_MESSAGE( 2, "IN: " << __PRETTY_FUNCTION__ << " active_tid=" << tid << "active_fid=" << fid <<  '\n' );
	    feature_vec.push_back( Feature(fid, freq) );
	  }
	}
      }
    }
    to.copy_and_set(feature_vec, freq_count);
  }
  
  int eventFrequency( void ) const
  {
    return freq_count;
  }

  void add( const FixedTarget& fixed_target, 
	    const CRFTransition& crf_transition, 
	    int f,
	    const std::vector<FeatureID>& tl,
            const std::vector<std::vector<Feature> >& hl)
  {
    freq_count          = f;
    active_list = tl;
    history_list  = hl;
    genObservedFeatureList( fixed_target, crf_transition, observed_list );
  }

  int frequency() const {
    return freq_count;
  }

  Real eventProbability() const {
    return 1.0;
  }

  Real referenceProbability( size_t i ) const {
    assert(false);
    return 1.0;
  }

  Real featureCount( FeatureID tid ) const
  {
    FeatureFreq ret = 0;
    for ( typename std::vector<std::vector<Feature> >::const_iterator itr = history_list.begin(); itr != history_list.end(); ++itr ) {
      ret += itr->size();
    }
    
    return ret;
  }

  Real maxFeatureCount() const {
    FeatureFreq ret = 0;
    for ( typename std::vector<std::vector<Feature> >::const_iterator itr = history_list.begin(); itr != history_list.end(); ++itr ) {
      ret += itr->size();
    }
    
    return ret;
  }

  //////////////////////////////////////////////////////////////////////

public:
  void writeObject( objstream& os ) const
  {
    os << freq_count;
    observed_list.writeObject(os);
    os << static_cast<size_t>( active_list.size() );
    for ( typename std::vector<FeatureID>::const_iterator it = active_list.begin();
          it != active_list.end();
          ++it ) {
      os << *it;
    }
    os << static_cast<size_t>( history_list.size() );
    for ( typename std::vector<std::vector<Feature> >::const_iterator it = history_list.begin();
          it != history_list.end();
          ++it ) {
      os << static_cast<size_t>( it->size() );
      for ( typename std::vector<Feature>::const_iterator jt = it->begin();
	    jt != it->end();
	    ++jt ) {
	jt->writeObject(os);
      }
    }
  }
  /// Write an object into a stream

  void readObject( objstream& is )
  {
    is >> freq_count;
    observed_list.readObject( is );
    size_t size;
    is >> size;
    active_list.resize(size);
    for ( typename std::vector<FeatureID>::iterator it = active_list.begin();
          it != active_list.end();
          ++it ) {
      is >> *it;
    }
    is >> size;
    history_list.resize(size);
    for ( typename std::vector<std::vector<Feature> >::iterator it = history_list.begin();
          it != history_list.end();
          ++it ) {
      is >> size;
      it->resize(size);
      for ( typename std::vector<Feature>::iterator jt = it->begin();
	    jt != it->end();
	    ++jt ) {
	jt->readObject(is);
      }
    }
  }
  /// Read an object from a stream

};

AMIS_NAMESPACE_END

#endif

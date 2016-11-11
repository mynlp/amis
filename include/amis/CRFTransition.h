//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: CRFTransition.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#ifndef Amis_CRFTransition_h_

#define Amis_CRFTransition_h_

#include <amis/configure.h>
#include <amis/Feature.h>
#include <amis/ErrorBase.h>
#include <vector>
#include <map>
#include <set>
#include <utility>
#include <cassert>


AMIS_NAMESPACE_BEGIN

class IllegalCRFTransitionError : public ErrorBase {
 public:
  IllegalCRFTransitionError( const std::string& s ) : ErrorBase( s ) {}
  IllegalCRFTransitionError( const char* s ) : ErrorBase( s ) {}
};

class CRFTransition {
private:
  typedef std::map< FeatureID, std::string > NameMap;
  typedef std::map< std::string, FeatureID > IDMap;

  typedef std::vector<std::vector<bool> > TransTable;
  typedef std::vector<std::vector<FeatureID> > RelTable;

  NameMap state_name;
  IDMap state_id;

  FeatureID num_states;
  FeatureID num_target;

  const FixedTarget& fixed_target;
  TransTable transition_table;
  RelTable out_table;
  RelTable in_table;
  RelTable next_state;
  RelTable prev_state;

public:
  CRFTransition(const FixedTarget& ft)
    : fixed_target(ft)
  {
    num_states = 0;
    num_target = fixed_target.numTargets();
  }
  virtual ~CRFTransition() {}

  
  void empty()
  {
    num_target = fixed_target.numTargets();
    transition_table.clear();
    next_state.clear();
    prev_state.clear();
    num_states = 0;
  }

  FeatureID numStates() const
  {
    return num_states;
  }

  void initTransition()
  {
    num_target = fixed_target.numTargets();
    num_states = 0;
    transition_table.clear();
    next_state.clear();
    prev_state.clear();
  }
  
  void finalize()
  {
    transition_table.clear();
    transition_table.resize(num_states, std::vector<bool>(num_states, false));
    next_state.clear();
    next_state.resize(num_states);
    prev_state.clear();
    prev_state.resize(num_states);

    out_table.resize(num_states);
    in_table.resize(num_target);
  }
  
  FeatureID registerNewState( const std::string& name ) {
    FeatureID id = num_states++;

    if ( state_id.find( name ) != state_id.end() ) {
      OStringStream oss;
      oss << "State " << name << " already exists";
      throw IllegalFeatureError( oss.str() );
    }
    state_name[id] = name;
    state_id[name] = id;
    return id;
  }


  FeatureID stateID( const std::string& name ) const
  {
    IDMap::const_iterator it = state_id.find( name );
    if ( it == state_id.end() ) {
      OStringStream oss;
      oss << "State " << name << " not found";
      throw IllegalFeatureError( oss.str() );
    }
    return it->second;
  }

  const std::string& stateName( FeatureID state_id ) const {
    NameMap::const_iterator it = state_name.find( state_id );
    if ( it == state_name.end() ) {
      OStringStream oss;
      oss << "State ID " << state_id << " not found";
      throw IllegalFeatureError( oss.str() );
    }
    return it->second;
  }


  void addTarget( FeatureID state, FeatureID target )
  {
    if ( state >= num_states ) {
      OStringStream oss;
      oss << "addTarget() state: " << state << " is not registered.";
      throw IllegalCRFTransitionError( oss.str() );
    }
    if ( target >= num_target ) {
      OStringStream oss;
      oss << "addTarget() target: " << target << " is not registered.";
      throw IllegalCRFTransitionError( oss.str() ); 
    }

    out_table[state].push_back(target);
    in_table[target].push_back(state);
  }



  void addPath( FeatureID state1, FeatureID state2 )
  {
    if ( state1 >= num_states ) {
      OStringStream oss;
      oss << "addPath() state: " << state1 << " is not registered.";
      throw IllegalCRFTransitionError( oss.str() );
    }
    if ( state2 >= num_states ) {
      OStringStream oss;
      oss << "addPath() state: " << state2 << " is not registered.";
      throw IllegalCRFTransitionError( oss.str() ); 
    }
    
    if ( transition_table[state1][state2] == true ) {
      OStringStream oss;
      oss << "addPath() transition from " << stateName(state1)
	  << " to " << stateName(state2) << " is already registered.";
      throw IllegalCRFTransitionError( oss.str() ); 
    }

    transition_table[state1][state2] = true;
    next_state[state1].push_back(state2);
    prev_state[state2].push_back(state1);
  }

  const std::vector<FeatureID>& out(FeatureID id) const
  {
    return out_table[id];
  }
  
  const std::vector<FeatureID>& in(FeatureID id) const
  {
    return in_table[id];
  }
  

  const std::vector<FeatureID>& prevStates(FeatureID cur) const
  {
    return prev_state[cur];
  }
  
  const std::vector<FeatureID>& nextStates(FeatureID cur) const
  {
    return next_state[cur];
  }
  
  bool transTable(FeatureID cur, FeatureID next) const
  {
    return transition_table[cur][next];
  }
  
  FeatureID newState()  { return num_states++; }
  
  /*FeatureID newHistory() {
    FeatureID hid = numHistories();
    feature_map.push_back( std::vector< std::pair< FeatureID, FeatureID > >( 0 ) );
    return hid;
  }
  
  
  FeatureID featureID( FeatureID hid, FeatureID tid ) const {
    std::map< std::pair< FeatureID, FeatureID>, FeatureID >::const_iterator itr = feature_map2.find( std::pair<FeatureID, FeatureID>( hid, tid ) );
    if ( itr == feature_map2.end() ) {
      OStringStream oss;
      oss << "No feature for hid=" << hid << ", tid=" << tid;
      throw IllegalFeatureError( oss.str() );
    }
    else {
      return itr->second;
    }
  }

  FeatureID historyID( const std::string& name ) const {
    IDMap::const_iterator it = history_id.find( name );
    if ( it == history_id.end() ) {
      OStringStream oss;
      oss << "Feature " << name << " not found";
      throw IllegalFeatureError( oss.str() );
    }
    return it->second;
  }

  FeatureID targetID( const std::string& name ) const
  {
    IDMap::const_iterator it = target_id.find( name );
    if ( it == target_id.end() ) {
      OStringStream oss;
      oss << "Feature " << name << " not found";
      throw IllegalFeatureError( oss.str() );
    }
    return it->second;
  }

  
  const std::pair< FeatureID, FeatureID >& historyAndTargetID( FeatureID fid ) const {
    std::map< FeatureID, std::pair< FeatureID, FeatureID> >::const_iterator itr = feature_map3.find( fid  );
    if ( itr == feature_map3.end() ) {
       OStringStream oss;
       oss << "No feature:" << fid;
       throw IllegalFeatureError( oss.str() );
    }
    else {
      return itr->second;
    }
    }*/
  
  public:
  // Debugging
  void debugInfo( std::ostream& ostr ) const
  {
    std::cerr << this << std::endl;
    for ( size_t i = 0; i < num_states; ++i ) {
      ostr << stateName(i) << " ->";
      for ( std::vector<FeatureID>::const_iterator jt = next_state[i].begin();
	    jt != next_state[i].end();
	    ++jt ) {
	ostr << ' ' << stateName(*jt);
      }
      ostr << std::endl;
    }
  }
};

AMIS_NAMESPACE_END

#endif // Amis_CRFTransition_h_
// end of CRFTransition.h

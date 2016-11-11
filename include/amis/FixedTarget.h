//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: FixedTarget.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#ifndef Amis_FixedTarget_h_

#define Amis_FixedTarget_h_

#include <amis/configure.h>
#include <amis/Feature.h>
#include <amis/ErrorBase.h>
#include <vector>
#include <map>
#include <utility>
#include <cassert>


AMIS_NAMESPACE_BEGIN

class IllegalFixedTargetError : public ErrorBase {
 public:
  IllegalFixedTargetError( const std::string& s ) : ErrorBase( s ) {}
  IllegalFixedTargetError( const char* s ) : ErrorBase( s ) {}
};

class FixedTarget {
private:
  
  typedef std::map< FeatureID, std::string > NameMap;
  typedef std::map< std::string, FeatureID > IDMap;

  NameMap target_name;
  IDMap target_id;
  NameMap history_name;
  IDMap history_id;

  size_t num_targets;
  
  std::vector< std::vector< std::pair< FeatureID, FeatureID > > > feature_map;
  
  std::map< std::pair< FeatureID, FeatureID>, FeatureID > feature_map2;
  
  std::map< FeatureID, std::pair< FeatureID, FeatureID> > feature_map3;
  
public:
  FixedTarget() {
    num_targets = 0;
  }
  virtual ~FixedTarget() {}

  FeatureID registerNewTarget( const std::string& name ) {
    FeatureID id = newTarget();
    if ( target_id.find( name ) != target_id.end() ) {
      OStringStream oss;
      oss << "Target " << name << " already exists";
      throw IllegalFeatureError( oss.str() );
    }
    target_name[ id ] = name;
    target_id[ name ] = id;
    return id;
  }

  FeatureID registerNewHistory( const std::string& name ) {
    FeatureID id = newHistory();
    if ( history_id.find( name ) != history_id.end() ) {
      OStringStream oss;
      oss << "History " << name << " already exists";
      throw IllegalFeatureError( oss.str() );
    }
    history_name[ id ] = name;
    history_id[ name ] = id;
    return id;
  }
  
  void empty() {
   feature_map.clear();
   feature_map2.clear();
   feature_map3.clear();
   num_targets = 0;
  }
  
  const std::vector< std::pair< FeatureID, FeatureID > >& getFeatures( FeatureID i ) const {
    assert( i < feature_map.size() );
    return feature_map[ i ];
  }

  void addFeatureMap( FeatureID hid, FeatureID tid, FeatureID fid ) {
    if ( hid >= feature_map.size() ) {
      OStringStream oss;
      oss << "addFeatureMap() history: " << hid << " is not registered.";
      throw IllegalFixedTargetError( oss.str() );
    }
    if ( tid >= num_targets ) {
      OStringStream oss;
      oss << "addFeatureMap() target: " << tid << " is not registered.";
      throw IllegalFixedTargetError( oss.str() ); 
    }
    
    std::pair< FeatureID, FeatureID > p( tid, fid );
    feature_map[ hid ].push_back( p );
    feature_map2[ std::pair< FeatureID, FeatureID >( hid, tid) ] = fid;
    feature_map3[ fid ] = std::pair< FeatureID, FeatureID >( hid, tid );
  }

  size_t numTargets() const { return num_targets; }
  size_t numHistories() const { return feature_map.size(); }
  
  FeatureID newTarget()  { return num_targets++; }
  
  FeatureID newHistory() {
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
      oss << "History " << name << " not found";
      throw IllegalFeatureError( oss.str() );
    }
    return it->second;
  }

  FeatureID targetID( const std::string& name ) const
  {
    IDMap::const_iterator it = target_id.find( name );
    if ( it == target_id.end() ) {
      OStringStream oss;
      oss << "Target " << name << " not found";
      throw IllegalFeatureError( oss.str() );
    }
    return it->second;
  }

  const std::string& historyName( FeatureID history_id ) const {
    NameMap::const_iterator it = history_name.find( history_id );
    if ( it == history_name.end() ) {
      OStringStream oss;
      oss << "History ID " << history_id << " not found";
      throw IllegalFeatureError( oss.str() );
    }
    return it->second;
  }

  const std::string& targetName( FeatureID target_id ) const {
    NameMap::const_iterator it = target_name.find( target_id );
    if ( it == target_name.end() ) {
      OStringStream oss;
      oss << "Target ID " << target_id << " not found";
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
  }
  
  public:
  // Debugging
  void debugInfo( std::ostream& ostr ) const {
    ostr << "====================\n";
    for ( size_t hid = 0; hid < feature_map.size(); hid++ ) {
      ostr << "hid=" << hid << " ";
      const std::vector< std::pair< FeatureID, FeatureID > >& fv = getFeatures( hid );
      for ( size_t k = 0; k < fv.size(); k++ ) {
        ostr << "[tid=" << fv[k].first << ",fid=" << fv[k].second << "] ";
      }
      ostr << "\n";
    }
  }
};





/*class RestrictedTarget {
private:
  
  typedef std::map< FeatureID, std::string > NameMap;
  typedef std::map< std::string, FeatureID > IDMap;

  NameMap group_name;
  IDMap group_id;
  FeatureID num_groups;

  std::vector< std::vector<FeatureID> > restriction;
public:
  RestrictedTarget()
  {
    num_groups = 0;
  }
  virtual ~RestrictedTarget() {}


  FeatureID registerNewGroup( const std::string& name )
  {
    FeatureID id = newGroup();
    if ( group_id.find( name ) != group_id.end() ) {
      OStringStream oss;
      oss << "Target " << name << " already exists";
      throw IllegalFeatureError( oss.str() );
    }
    group_name[id] = name;
    group_id[name] = id;
    return id;
  }

  void empty() {
    group_name.clear();
    group_id.clear();
    restriction.clear();
    num_groups = 0;
  }
  
  void addRestriction( FixedTarget& fixed_target,
		       FeatureID gid, FeatureID tid )
  {
    if ( gid >= group_name.size() ) {
      OStringStream oss;
      oss << "addResriction() group: " << gid << " is not registered.";
      throw IllegalFixedTargetError( oss.str() );
    }
    if ( tid >= fixed_target.numTargets() ) {
      OStringStream oss;
      oss << "addResriction() target: " << tid << " is not registered.";
      throw IllegalFixedTargetError( oss.str() ); 
    }

    restriction[gid].push_back(tid);
  }

  bool validate( FeatureID gid, FeatureID tid ) const
  {
    if ( gid >= group_name.size() ) {
      OStringStream oss;
      oss << "addResriction() group: " << gid << " is not registered.";
      throw IllegalFixedTargetError( oss.str() );
    }
    for ( size_t i = 0; i < restriction[gid].size(); ++i ) {
      if ( restriction[gid][i] == tid ) return true;
    }
    return false;
  }

  const std::vector<FeatureID>& targets(FeatureID gid) const
  {
    return restriction[gid];
  }

  void addAllTarget(FixedTarget& fixed_target)
  {
    registerNewGroup("_");
    FeatureID all_id = groupID("_");
    for ( FeatureID id = 0; id < fixed_target.numTargets(); ++id ) {
      addRestriction(fixed_target, all_id, id);
    }
  }

  size_t numGroups() const
  {
    return num_groups;
  }
  
  FeatureID newGroup()
  {
    ++num_groups;
    restriction.resize(num_groups);
    return num_groups-1;
  }
  
  FeatureID groupID( const std::string& name ) const
  {
    IDMap::const_iterator it = group_id.find( name );
    if ( it == group_id.end() ) {
      OStringStream oss;
      oss << "Group " << name << " not found";
      throw IllegalFeatureError( oss.str() );
    }
    return it->second;
  }

  const std::string& groupName( FeatureID group_id ) const
  {
    NameMap::const_iterator it = group_name.find( group_id );
    if ( it == group_name.end() ) {
      OStringStream oss;
      oss << "Group ID " << group_id << " not found";
      throw IllegalFeatureError( oss.str() );
    }
    return it->second;
  }


  public:
  // Debugging
  void debugInfo( std::ostream& ostr ) const {
    ostr << "===========RestrictedTarget::debugInfo, not implemented=========\n";
  }
  };*/

AMIS_NAMESPACE_END

#endif // Amis_FixedTarget_h_
// end of FixedTarget.h

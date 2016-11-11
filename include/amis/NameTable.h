//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: NameTable.h,v 1.2 2008-07-29 06:05:57 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#ifndef Amis_NameTable_h_

#define Amis_NameTable_h_

#include <amis/configure.h>
#include <amis/StringStream.h>
#include <amis/Feature.h>
#include <amis/DynamicCast.h>
#include <string>
#include <vector>
#include <memory>

AMIS_NAMESPACE_BEGIN

class NameTable {
protected:
  typedef amis_hash_map< FeatureID, std::string > NameMap;
  typedef amis_hash_map< std::string, FeatureID > IDMap;
  NameMap feature_name;
  IDMap feature_id;

public:
  NameTable() {}
  virtual ~NameTable() {}
  
  virtual void empty() {
    feature_name.clear();
    feature_id.clear();
  }
  
  size_t numFeatures() const { return feature_name.size(); }
  std::string featureNameString( FeatureID id ) const {
    OStringStream oss;
    oss << featureName( id );
    return oss.str();
  }
  virtual const std::string& featureName( FeatureID id ) const {
    NameMap::const_iterator it = feature_name.find( id );
    if ( it == feature_name.end() ) {
      OStringStream oss;
      oss << "FeatureID " << id << " not found";
      throw IllegalFeatureError( oss.str() );
    }
    return it->second;
  }
  virtual FeatureID featureID( const std::string& name, bool unk_on_not_found = false )
  {
    IDMap::const_iterator it = feature_id.find( name );
    if ( it == feature_id.end() ) {
      if( unk_on_not_found ) {
        return UNKNOWN_FEATURE_ID;
      }
      else {
        OStringStream oss;
        oss << "Feature " << name << " not found";
        throw IllegalFeatureError( oss.str() );
      }
    }
    return it->second;
  }
  
  virtual void registerNewFeature( const std::string& name, FeatureID id ) {
    if ( feature_name.find( id ) != feature_name.end() ) {
      OStringStream oss;
      oss << "Feature ID " << id << " already exists";
      throw IllegalFeatureError( oss.str() );
    }
    if ( feature_id.find( name ) != feature_id.end() ) {
      OStringStream oss;
      oss << "Feature " << name << " already exists";
      throw IllegalFeatureError( oss.str() );
    }
    feature_name[ id ] = name;
    feature_id[ name ] = id;
  }
  
  /*  virtual void importName( const NameTableBase& name_table_base, const std::vector< FeatureID >& fids ) {
    const NameTable* from = DynamicCast< const NameTable* >( &name_table_base, __PRETTY_FUNCTION__ );
    
    empty();
    
    for ( size_t i = 0; i < fids.size(); i++ ) {
      const Name& name_from = from->featureName( fids[ i ] );
      registerNewFeature( name_from, i ); 
    }
    
    }*/
  
};

typedef std::auto_ptr< NameTable > NameTablePtr;

class UnknownName {
  public:
  static const std::string unk_name;
};


AMIS_NAMESPACE_END

#endif // Amis_NameTable_h_
// end of NameTable.h

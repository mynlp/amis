//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: OptionManager.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_OptionManager_h_
#define Amis_OptionManager_h_

#include <amis/configure.h>
#include <amis/Initializer.h>
#include <amis/ErrorBase.h>
#include <map>
#include <vector>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/// <classdef>
/// <name>IllegalOptionError</name>
/// <overview>Exception for illegal options</overview>
/// <desc>
/// The class signals illegal property specifications in
/// command-line arguments and in a property file
/// </desc>
/// <body>

class IllegalOptionError : public ErrorBase {
public:
  /// Initialize with an error message
  explicit IllegalOptionError( const std::string& m ) : ErrorBase( m ) {}
  /// Initialize with an error message
  explicit IllegalOptionError( const char* m ) : ErrorBase( m ) {}
  /// Destructor
  virtual ~IllegalOptionError() {}
};

/// </body>
/// </classdef>

//////////////////////////////////////////////////////////

class OptionBase;

class OptionManager {
protected:
  Initializer< OptionManager* >*& queue;
  std::vector< OptionBase* > option_vector;
  std::map< std::string, OptionBase* > name_hash;
  std::map< std::string, OptionBase* > long_hash;
  std::map< std::string, OptionBase* > short_hash;

  // accumulate non-optional argumens
  // comm -o1 <a1> <n1> -o0 <n2> <n3> => [n1, n2, n3] 
  //std::vector< std::string >  last_vec;

public:
  OptionManager( Initializer< OptionManager* >*& q ) : queue( q ) {}
  virtual ~OptionManager() {}

  void initAllOptions() {
    Initializer< OptionManager* >::initAll( queue, this );
  }

  void initOption( OptionBase* opt );
  std::string getShortDescription() const;
  std::string getFullDescription() const;
  std::string getSpecificationStatus() const;

  OptionBase* findOption( const std::string& name ) {
    std::map< std::string, OptionBase* >::iterator it = name_hash.find( name );
    if ( it == name_hash.end() ) {
      return NULL;
    } else {
      return it->second;
    }
  }
  OptionBase* findOptionLongName( const std::string& name ) {
    std::map< std::string, OptionBase* >::iterator it = long_hash.find( name );
    if ( it == long_hash.end() ) {
      return NULL;
    } else {
      return it->second;
    }
  }
  OptionBase* findOptionShortName( const std::string& name ) {
    std::map< std::string, OptionBase* >::iterator it = short_hash.find( name );
    if ( it == short_hash.end() ) {
      return NULL;
    } else {
      return it->second;
    }
  }

  void setOption( const std::string& name, const std::string& value );
  void setOptionLongName( const std::string& name, const std::string& value );
  void setOptionShortName( const std::string& name, const std::string& value );

  void resetToInit();
  void resetToUnspecified();
};

AMIS_NAMESPACE_END

#endif // Amis_OptionManager_h_
// end of OptionManager.h

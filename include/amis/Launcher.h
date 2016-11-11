//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: Launcher.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_Launcher_h_

#define Amis_Launcher_h_

#include <amis/configure.h>
#include <amis/Initializer.h>
#include <amis/StringHash.h>
#include <amis/LauncherItem.h>
#include <amis/StringStream.h>
#include <vector>
#include <map>
#include <iostream>

AMIS_NAMESPACE_BEGIN

template < class T, class ArgType, class Name > class LauncherItem;

template < class T, class ArgType, class Name = std::string >
class Launcher {
protected:
  Initializer< Launcher< T, ArgType, Name >* >*& queue;
  std::string name;
  std::vector< LauncherItem< T, ArgType, Name >* > launcher_items;
  std::map< Name, LauncherItem< T, ArgType, Name >* > launcher_hash;
  bool initialized;

public:
  Launcher( const std::string& n,
            Initializer< Launcher< T, ArgType, Name >* >*& q )
    : queue( q ) {
    name = n;
    initialized = false;
  }
  virtual ~Launcher() {}

  const std::string& getName() const {
    return name;
  }

  void initLauncherItems() {
    if ( ! initialized ) {
      Initializer< Launcher< T, ArgType, Name >* >::initAll( queue, this );
      initialized = true;
    }
  }

  virtual void setLauncherItem( LauncherItem< T, ArgType, Name >* item ) {
    typename std::map< Name, LauncherItem< T, ArgType, Name >* >::iterator it = launcher_hash.find( item->getName() );
    if ( it != launcher_hash.end() ) {
      OStringStream oss;
      oss << "Duplicate launcher item: " << item->getName();
	  oss << " in " << getName() << "\n";
      throw IllegalLauncherItemError( oss.str() );
    }
    launcher_items.push_back( item );
    launcher_hash[ item->getName() ] = item;
  }
  virtual T launch( const Name& name, ArgType arg ) {
    typename std::map< Name, LauncherItem< T, ArgType, Name >* >::iterator it = launcher_hash.find( name );
    if ( it == launcher_hash.end() ) {
      OStringStream oss;
      oss << "Cannot find a launcher item for " << getName() << ": " << name;
      throw IllegalLauncherItemError( oss.str() );
    }
    AMIS_DEBUG_MESSAGE( 3, getName() << " launching " << name << '\n' );
    return it->second->launch( arg );
  }

  virtual void showLauncherItems( std::ostream& os ) {
    for ( typename std::map< Name, LauncherItem< T, ArgType, Name >* >::iterator it = launcher_hash.begin();
          it != launcher_hash.end();
          ++it ) {
      os << it->first << '\t' << it->second->getDescription() << '\n';
    }
  }
};

AMIS_NAMESPACE_END

#endif // Amis_Launcher_h_
// end of Launcher.h

//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: EstimatorLauncher.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_EstimatorLauncher_h_

#define Amis_EstimatorLauncher_h_

#include <amis/configure.h>
#include <amis/Launcher.h>
#include <amis/LauncherItem.h>
#include <amis/Estimator.h>
#include <string>

#include <amis/PropertyItem.h>

AMIS_NAMESPACE_BEGIN

class Property;

class EstimatorIdentifier {
private:
  std::string name;     // algorithm name

public:
  EstimatorIdentifier( const std::string& n )
    : name( n ) { }
  virtual ~EstimatorIdentifier() {}

  const std::string& getName() const { return name; }

  bool operator<( const EstimatorIdentifier& id ) const {
    return name < id.name;
  }
};

inline std::ostream& operator<<( std::ostream& os, const EstimatorIdentifier& id ) {
  os << '(' << id.getName();
  return os << ')';
}

//////////////////////////////////////////////////////////////////////

class EstimatorLauncher : public Launcher< EstimatorPtr, Property*, EstimatorIdentifier > {
private:
  static Initializer< Launcher< EstimatorPtr, Property*, EstimatorIdentifier >* >* queue;

public:
  
  EstimatorLauncher( void ) : Launcher< EstimatorPtr, Property*, EstimatorIdentifier >( "EstimatorLauncher", queue ) {}
  virtual ~EstimatorLauncher() {}

  friend class EstimatorLauncherItem;
};

//////////////////////////////////////////////////////////////////////

class EstimatorLauncherItem : public LauncherItem< EstimatorPtr, Property*, EstimatorIdentifier > {
public:
  EstimatorLauncherItem( const EstimatorIdentifier& id, const std::string& desc = "" ) : LauncherItem< EstimatorPtr, Property*, EstimatorIdentifier >(EstimatorLauncher::queue, id, desc ) { }
  
  EstimatorLauncherItem( const std::string& name, const std::string& desc = "" )
  : LauncherItem< EstimatorPtr, Property*, EstimatorIdentifier >( EstimatorLauncher::queue, EstimatorIdentifier( name ), desc ) {}
  
  virtual ~EstimatorLauncherItem() {}
};

AMIS_NAMESPACE_END

#endif // Amis_EstimatorLauncher_h_
// end of EstimatorLauncher.h

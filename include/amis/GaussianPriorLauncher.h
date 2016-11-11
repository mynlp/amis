//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: GaussianPriorLauncher.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#ifndef Amis_GaussianPriorLauncher_h_

#define Amis_GaussianPriorLauncher_h_

#include <amis/configure.h>
#include <amis/Launcher.h>
#include <amis/LauncherItem.h>
#include <amis/GaussianPrior.h>
#include <amis/PropertyItem.h>

AMIS_NAMESPACE_BEGIN

class Property;

class GaussianPriorLauncher : public Launcher< GaussianPriorPtr, Property* > {
private:
  static Initializer< Launcher< GaussianPriorPtr, Property* >* >* queue;

public:
  //static PropertyItem< std::string > variance_type;
  // seems to be defined in Property.h (J.K 2003/9/10)
  
  static PropertyItem< Real > map_sigma;

public:
  GaussianPriorLauncher( void ) : Launcher< GaussianPriorPtr, Property* >( "GaussianPriorLauncher", queue ) {}
  virtual ~GaussianPriorLauncher() {}

  template < class GaussianPrior >
  friend class GaussianPriorLauncherItem;
};

template < class GaussianPrior >
class GaussianPriorLauncherItem : public LauncherItem< GaussianPriorPtr, Property* > {
public:
  GaussianPriorLauncherItem( const std::string& n, const std::string& desc = "" )
    : LauncherItem< GaussianPriorPtr, Property* >( GaussianPriorLauncher::queue, n, desc ) {}
  virtual ~GaussianPriorLauncherItem() {}
  GaussianPriorPtr launch( Property* property ) {
    return GaussianPriorPtr( new GaussianPrior( GaussianPriorLauncher::map_sigma.getValue() ) );
  }
};

AMIS_NAMESPACE_END

#endif // Amis_GaussianPriorLauncher_h_
// end of GaussianPriorLauncher.h

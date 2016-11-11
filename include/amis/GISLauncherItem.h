//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: GISLauncherItem.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#ifndef Amis_GISLauncherItem_h_

#define Amis_GISLauncherItem_h_

#include <amis/configure.h>
#include <amis/EstimatorLauncher.h>
#include <amis/Property.h>

AMIS_NAMESPACE_BEGIN

template < class GIS >
class GISLauncherItem : public EstimatorLauncherItem {
public:
  GISLauncherItem( const EstimatorIdentifier& id,  const std::string& desc )
    : EstimatorLauncherItem( id, desc ) {}
    
  virtual ~GISLauncherItem() {}

  EstimatorPtr launch( Property* property ) {
    GIS* gis = new GIS();
    gis->setNumThreads( property->getNumThreads() );
    return EstimatorPtr( gis );
  }
};

AMIS_NAMESPACE_END

#endif // Amis_GISLauncherItem_h_
// end of GISLauncherItem.h

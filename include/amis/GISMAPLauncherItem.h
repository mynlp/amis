//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: GISMAPLauncherItem.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#ifndef Amis_GISMAPLauncherItem_h_

#define Amis_GISMAPLauncherItem_h_

#include <amis/configure.h>
#include <amis/GISLauncherItem.h>
#include <amis/GaussianPriorLauncher.h>
#include <amis/Property.h>

AMIS_NAMESPACE_BEGIN

template < class GISMAP >
class GISMAPLauncherItem : public GISLauncherItem< GISMAP > {
public:
  GISMAPLauncherItem( const EstimatorIdentifier& id,  const std::string& desc )
    : GISLauncherItem< GISMAP >( id, desc ) {}
    
  virtual ~GISMAPLauncherItem() {}

  EstimatorPtr launch( Property* property ) {
    GISMAP* gis = new GISMAP();
    gis->setNumThreads( property->getNumThreads() );
    gis->setGaussianPrior( property->newGaussianPrior() );
    return EstimatorPtr( gis );
  }
};

AMIS_NAMESPACE_END

#endif // Amis_GISMAPLauncherItem_h_
// end of GISMAPLauncherItem.h

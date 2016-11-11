//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: OWBFGSBCMAPLauncherItem.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_OWBFGSBCMAPLauncherItem_h_

#define Amis_OWBFGSBCMAPLauncherItem_h_

#include <amis/configure.h>
#include <amis/OWBFGSBCLauncherItem.h>
#include <amis/GaussianPriorLauncher.h>
#include <amis/Property.h>

AMIS_NAMESPACE_BEGIN

/*
 * LauncherItem for OWBFGSBCMAP estimators.
 * Set memory size, number of threads, and a gaussian prior.
 */

template < class OWBFGSBCMAP >
class OWBFGSBCMAPLauncherItem : public OWBFGSBCLauncherItem< OWBFGSBCMAP > {
public:
  /// Constructor
  OWBFGSBCMAPLauncherItem( const EstimatorIdentifier& id, const std::string& desc )
    : OWBFGSBCLauncherItem< OWBFGSBCMAP >( id, desc ) {}
  
  
  /// Launch a new OWBFGSBCMAP estimator
  EstimatorPtr launch( Property* property ) {
    OWBFGSBCMAP* bfgs = new OWBFGSBCMAP();
    bfgs->setMemorySize( OWBFGSBCLauncherItem< OWBFGSBCMAP >::memory_size.getValue() );
    bfgs->setNumThreads( property->getNumThreads() );
    bfgs->setGaussianPrior( property->newGaussianPrior() );
    return EstimatorPtr( bfgs );
  }
};

AMIS_NAMESPACE_END

#endif // Amis_OWBFGSBCMAPLauncherItem_h_
// end of OWBFGSBCMAPLauncherItem.h

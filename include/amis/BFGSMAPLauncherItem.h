//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: BFGSMAPLauncherItem.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_BFGSMAPLauncherItem_h_

#define Amis_BFGSMAPLauncherItem_h_

#include <amis/configure.h>
#include <amis/BFGSLauncherItem.h>
#include <amis/GaussianPriorLauncher.h>
#include <amis/Property.h>

AMIS_NAMESPACE_BEGIN

/*
 * LauncherItem for BFGSMAP estimators.
 * Set memory size, number of threads, and a gaussian prior.
 */

template < class BFGSMAP >
class BFGSMAPLauncherItem : public BFGSLauncherItem< BFGSMAP > {
public:
  /// Constructor
  BFGSMAPLauncherItem( const EstimatorIdentifier& id, const std::string& desc )
    : BFGSLauncherItem< BFGSMAP >( id, desc ) {}
  
  
  /// Launch a new BFGSMAP estimator
  EstimatorPtr launch( Property* property ) {
    BFGSMAP* bfgs = new BFGSMAP();
    bfgs->setMemorySize( BFGSLauncherItem< BFGSMAP >::memory_size.getValue() );
    bfgs->setNumThreads( property->getNumThreads() );
    bfgs->setGaussianPrior( property->newGaussianPrior() );
    return EstimatorPtr( bfgs );
  }
};

AMIS_NAMESPACE_END

#endif // Amis_BFGSMAPLauncherItem_h_
// end of BFGSMAPLauncherItem.h

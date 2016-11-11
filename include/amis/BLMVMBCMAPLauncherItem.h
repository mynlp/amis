//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: BLMVMBCMAPLauncherItem.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_BLMVMBCMAPLauncherItem_h_

#define Amis_BLMVMBCMAPLauncherItem_h_

#include <amis/configure.h>
#include <amis/BLMVMBCLauncherItem.h>
#include <amis/GaussianPriorLauncher.h>
#include <amis/Property.h>

AMIS_NAMESPACE_BEGIN

/*
 * LauncherItem for BLMVMBCMAP estimators.
 * Set memory size, number of threads, and a gaussian prior.
 */

template < class BLMVMBCMAP >
class BLMVMBCMAPLauncherItem : public BLMVMBCLauncherItem< BLMVMBCMAP > {
public:
  /// Constructor
  BLMVMBCMAPLauncherItem( const EstimatorIdentifier& id, const std::string& desc )
    : BLMVMBCLauncherItem< BLMVMBCMAP >( id, desc ) {}
  
  
  /// Launch a new BLMVMBCMAP estimator
  EstimatorPtr launch( Property* property ) {
    BLMVMBCMAP* bfgs = new BLMVMBCMAP();
    bfgs->setMemorySize( BLMVMBCLauncherItem< BLMVMBCMAP >::memory_size.getValue() );
    bfgs->setNumThreads( property->getNumThreads() );
    bfgs->setGaussianPrior( property->newGaussianPrior() );
    return EstimatorPtr( bfgs );
  }
};

AMIS_NAMESPACE_END

#endif // Amis_BLMVMBCMAPLauncherItem_h_
// end of BLMVMBCMAPLauncherItem.h

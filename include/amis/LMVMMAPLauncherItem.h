//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: LMVMMAPLauncherItem.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_LMVMMAPLauncherItem_h_

#define Amis_LMVMMAPLauncherItem_h_

#include <amis/configure.h>
#include <amis/LMVMLauncherItem.h>
#include <amis/GaussianPriorLauncher.h>
#include <amis/Property.h>

AMIS_NAMESPACE_BEGIN

/*
 * LauncherItem for LMVMMAP estimators.
 * Set memory size, number of threads, and a gaussian prior.
 */

template < class LMVMMAP >
class LMVMMAPLauncherItem : public LMVMLauncherItem< LMVMMAP > {
public:
  /// Constructor
  LMVMMAPLauncherItem( const EstimatorIdentifier& id, const std::string& desc )
    : LMVMLauncherItem< LMVMMAP >( id, desc ) {}
  
  
  /// Launch a new LMVMMAP estimator
  EstimatorPtr launch( Property* property ) {
    LMVMMAP* bfgs = new LMVMMAP();
    bfgs->setMemorySize( this->memory_size.getValue() );
    bfgs->setNumThreads( property->getNumThreads() );
    bfgs->setGaussianPrior( property->newGaussianPrior() );
    return EstimatorPtr( bfgs );
  }
};

AMIS_NAMESPACE_END

#endif // Amis_LMVMMAPLauncherItem_h_
// end of LMVMMAPLauncherItem.h

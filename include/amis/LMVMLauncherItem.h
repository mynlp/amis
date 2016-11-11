//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: LMVMLauncherItem.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_LMVMLauncherItem_h_

#define Amis_LMVMLauncherItem_h_

#include <amis/configure.h>
#include <amis/EstimatorLauncher.h>
#include <amis/BFGSLauncherItem.h>
#include <amis/Property.h>

AMIS_NAMESPACE_BEGIN

/*
 * Base class for LauncherItem for LMVM estimators
 */

class LMVMLauncherItemBase : public BFGSLauncherItemBase {
public:
  /// Constructor
  LMVMLauncherItemBase( const EstimatorIdentifier& id, const std::string& desc )
    : BFGSLauncherItemBase(id , desc) {}
  
  /// Destructor
  virtual ~LMVMLauncherItemBase() {}
};

//////////////////////////////////////////////////////////////////////

/*
 * LauncherItem for LMVM estimators.
 * Set memory size and number of threads.
 */

template < class LMVM >
class LMVMLauncherItem : public LMVMLauncherItemBase {
public:
  /// Constructor
  LMVMLauncherItem( const EstimatorIdentifier& id,  const std::string& desc )
    : LMVMLauncherItemBase( id , desc ) {}
  
  /// Destructor
  virtual ~LMVMLauncherItem() {}

  /// Launch a new LMVM estimator
  EstimatorPtr launch( Property* property ) {
    LMVM* lmvmbc = new LMVM();
    lmvmbc->setMemorySize( this->memory_size.getValue() );
    lmvmbc->setNumThreads( property->getNumThreads() );
    return EstimatorPtr(lmvmbc);
  }
};

AMIS_NAMESPACE_END

#endif // Amis_LMVMLauncherItem_h_
// end of LMVMLauncherItem.h

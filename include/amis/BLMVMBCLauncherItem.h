//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: BLMVMBCLauncherItem.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_BLMVMBCLauncherItem_h_

#define Amis_BLMVMBCLauncherItem_h_

#include <amis/configure.h>
#include <amis/EstimatorLauncher.h>
#include <amis/BFGSLauncherItem.h>
#include <amis/Property.h>

AMIS_NAMESPACE_BEGIN

/*
 * Base class for LauncherItem for BLMVMBC estimators
 */

class BLMVMBCLauncherItemBase : public BFGSLauncherItemBase {
public:
  /// Property of the size of the memory of BLMVMBC estimators
  static PropertyItem< Real > bc_lower;
  static PropertyItem< Real > bc_upper;

public:
  /// Constructor
  BLMVMBCLauncherItemBase( const EstimatorIdentifier& id, const std::string& desc )
    : BFGSLauncherItemBase(id , desc) {}
  
  /// Destructor
  virtual ~BLMVMBCLauncherItemBase() {}
};

//////////////////////////////////////////////////////////////////////

/*
 * LauncherItem for BLMVMBC estimators.
 * Set memory size and number of threads.
 */

template < class BLMVMBC >
class BLMVMBCLauncherItem : public BLMVMBCLauncherItemBase {
public:
  /// Constructor
  BLMVMBCLauncherItem( const EstimatorIdentifier& id,  const std::string& desc )
    : BLMVMBCLauncherItemBase( id , desc ) {}
  
  /// Destructor
  virtual ~BLMVMBCLauncherItem() {}

  /// Launch a new BLMVMBC estimator
  EstimatorPtr launch( Property* property ) {
    BLMVMBC* lmvmbc = new BLMVMBC();
    lmvmbc->setMemorySize( this->memory_size.getValue() );
    lmvmbc->setNumThreads( property->getNumThreads() );
    return EstimatorPtr(lmvmbc);
  }
};

AMIS_NAMESPACE_END

#endif // Amis_BLMVMBCLauncherItem_h_
// end of BLMVMBCLauncherItem.h

//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: OWBFGSBCLauncherItem.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_OWBFGSBCLauncherItem_h_

#define Amis_OWBFGSBCLauncherItem_h_

#include <amis/configure.h>
#include <amis/EstimatorLauncher.h>
#include <amis/BFGSLauncherItem.h>
#include <amis/OWBFGSBCLauncherItem.h>
#include <amis/Property.h>

AMIS_NAMESPACE_BEGIN

/*
 * Base class for LauncherItem for OWBFGSBC estimators
 */

class OWBFGSBCLauncherItemBase : public BFGSLauncherItemBase {
public:
  /// Constructor
  OWBFGSBCLauncherItemBase( const EstimatorIdentifier& id, const std::string& desc )
    : BFGSLauncherItemBase(id , desc) {}
  
  /// Destructor
  virtual ~OWBFGSBCLauncherItemBase() {}
};

//////////////////////////////////////////////////////////////////////

/*
 * LauncherItem for OWBFGSBC estimators.
 * Set memory size and number of threads.
 */

template < class OWBFGSBC >
class OWBFGSBCLauncherItem : public OWBFGSBCLauncherItemBase {
public:
  /// Constructor
  OWBFGSBCLauncherItem( const EstimatorIdentifier& id,  const std::string& desc )
    : OWBFGSBCLauncherItemBase( id , desc ) {}
  
  /// Destructor
  virtual ~OWBFGSBCLauncherItem() {}

  /// Launch a new OWBFGSBC estimator
  EstimatorPtr launch( Property* property ) {
    OWBFGSBC* lmvmbc = new OWBFGSBC();
    lmvmbc->setMemorySize( this->memory_size.getValue() );
    lmvmbc->setNumThreads( property->getNumThreads() );
    return EstimatorPtr(lmvmbc);
  }
};

AMIS_NAMESPACE_END

#endif // Amis_OWBFGSBCLauncherItem_h_
// end of OWBFGSBCLauncherItem.h

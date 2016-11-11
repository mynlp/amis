//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: BFGSLauncherItem.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_BFGSLauncherItem_h_

#define Amis_BFGSLauncherItem_h_

#include <amis/configure.h>
#include <amis/EstimatorLauncher.h>
#include <amis/Property.h>

AMIS_NAMESPACE_BEGIN

/*
 * Base class for LauncherItem for BFGS estimators
 */

class BFGSLauncherItemBase : public EstimatorLauncherItem {
public:
  /// Property of the size of the memory of BFGS estimators
  static PropertyItem< int > memory_size;

public:
  /// Constructor
  BFGSLauncherItemBase( const EstimatorIdentifier& id, const std::string& desc )
    : EstimatorLauncherItem( id , desc ) {}
  
  /// Destructor
  virtual ~BFGSLauncherItemBase() {}
};

//////////////////////////////////////////////////////////////////////

/*
 * LauncherItem for BFGS estimators.
 * Set memory size and number of threads.
 */

template < class BFGS >
class BFGSLauncherItem : public BFGSLauncherItemBase {
public:
  /// Constructor
  BFGSLauncherItem( const EstimatorIdentifier& id,  const std::string& desc )
    : BFGSLauncherItemBase( id , desc ) {}
  
  /// Destructor
  virtual ~BFGSLauncherItem() {}

  /// Launch a new BFGS estimator
  EstimatorPtr launch( Property* property ) {
    BFGS* bfgs = new BFGS();
    bfgs->setMemorySize( memory_size.getValue() );
    bfgs->setNumThreads( property->getNumThreads() );
    return EstimatorPtr( bfgs );
  }
};

AMIS_NAMESPACE_END

#endif // Amis_BFGSLauncherItem_h_
// end of BFGSLauncherItem.h

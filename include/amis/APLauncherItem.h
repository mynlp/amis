//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: APLauncherItem.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_APLauncherItem_h_

#define Amis_APLauncherItem_h_

#include <amis/configure.h>
#include <amis/EstimatorLauncher.h>
#include <amis/Property.h>
#include <amis/PropertyItem.h>
#include <amis/APEstimator.h>

AMIS_NAMESPACE_BEGIN

class APLauncherItem : public EstimatorLauncherItem {
  static PropertyItem<Real> step_size;
public:
  /// Constructor
  APLauncherItem( const EstimatorIdentifier& id,  const std::string& desc )
    : EstimatorLauncherItem( id , desc ) {}
  
  /// Destructor
  virtual ~APLauncherItem() {}

  /// Launch a new AP estimator
  EstimatorPtr launch( Property* property )
  {
    APEstimator* ap = new APEstimator();
    ap->setStepSize(step_size.getValue());
    return EstimatorPtr(ap);
  }
};

AMIS_NAMESPACE_END

#endif // Amis_APLauncherItem_h_
// end of APLauncherItem.h

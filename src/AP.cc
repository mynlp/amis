//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: AP.cc,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#include <amis/APEstimator.h>
#include <amis/APLauncherItem.h>

AMIS_NAMESPACE_BEGIN

PropertyItem<Real> APLauncherItem::step_size( "AP_STEP_SIZE", "--ap-step-size", "", 1.0, "Step size of Averaged Perceptron" );

AMIS_NAMESPACE_END

namespace {

  using namespace amis;

  APLauncherItem
  ap( EstimatorIdentifier( "AP" ), "Averaged Perceptron" );

}

// end of AP.cc

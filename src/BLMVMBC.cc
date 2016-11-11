//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: BLMVMBC.cc,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#include <amis/BLMVMBC.h>
#include <amis/BLMVMBCLauncherItem.h>

AMIS_NAMESPACE_BEGIN

PropertyItem< Real > BLMVMBCLauncherItemBase::bc_lower("BC_LOWER",
						      "--bc-lower",
						      "", 10,
						      "Lower bound of the box constraint" );
PropertyItem< Real > BLMVMBCLauncherItemBase::bc_upper("BC_UPPER",
						      "--bc-upper",
						      "", 10,
						      "Upper bound of the box constraint" );


AMIS_NAMESPACE_END

namespace {

  using namespace amis;

  BLMVMBCLauncherItem< BLMVMBC >
  blmvmbc( EstimatorIdentifier( "BLMVMBC" ), "BLMVMBC" );

}

// end of BLMVMBC.cc

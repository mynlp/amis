//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: BFGS.cc,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#include <amis/BFGS.h>
#include <amis/BFGSMAP.h>
#include <amis/BFGSLauncherItem.h>
#include <amis/BFGSMAPLauncherItem.h>

#include <amis/OWBFGSBC.h>
#include <amis/OWBFGSBCLauncherItem.h>
#include <amis/OWBFGSBCMAP.h>
#include <amis/OWBFGSBCMAPLauncherItem.h>

AMIS_NAMESPACE_BEGIN

PropertyItem< int > BFGSLauncherItemBase::memory_size( "MEMORY_SIZE", "--memory-size", "", 5, "Size of the memory used in Limited-memory BFGS method" );

AMIS_NAMESPACE_END

namespace {

  using namespace amis;

  BFGSLauncherItem< BFGS >
  bfgs( EstimatorIdentifier( "BFGS" ), "Limited-memory BFGS" );

  BFGSMAPLauncherItem< BFGSMAP >
  bfgsmap( EstimatorIdentifier( "BFGSMAP" ), "Limited-memory BFGS with MAP estimation" );

  OWBFGSBCLauncherItem< OWBFGSBC >
  owbfgsbc( EstimatorIdentifier( "OWBFGSBC" ), "Orthant-wise BFGS with exponential priors" );

  OWBFGSBCMAPLauncherItem< OWBFGSBCMAP >
  owbfgsbcmap( EstimatorIdentifier( "OWBFGSBCMAP" ), "Orthant-wise BFGS with exponential + gaussian priors" );

}

// end of BFGS.cc

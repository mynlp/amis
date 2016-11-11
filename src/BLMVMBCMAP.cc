//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: BLMVMBCMAP.cc,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#include <amis/BLMVMBCMAP.h>
#include <amis/BLMVMBCMAPLauncherItem.h>

namespace {

  using namespace amis;

  BLMVMBCMAPLauncherItem< BLMVMBCMAP >
  blmvmbcmap( EstimatorIdentifier( "BLMVMBCMAP" ), "BLMVMBC with MAP estimation" );

}

// end of BLMVMBCMAP.cc

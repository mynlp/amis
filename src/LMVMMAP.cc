//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: LMVMMAP.cc,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#include <amis/LMVMMAP.h>
#include <amis/LMVMMAPLauncherItem.h>

namespace {

  using namespace amis;

  LMVMMAPLauncherItem< LMVMMAP >
  lmvmmap( EstimatorIdentifier( "LMVMMAP" ), "LMVM with MAP estimation" );

}

// end of LMVMMAP.cc

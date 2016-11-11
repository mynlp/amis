//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: LMVM.cc,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#include <amis/LMVM.h>
#include <amis/LMVMLauncherItem.h>

namespace {

  using namespace amis;

  LMVMLauncherItem< LMVM >
  lmvm( EstimatorIdentifier( "LMVM" ), "LMVM" );

}

// end of LMVM.cc

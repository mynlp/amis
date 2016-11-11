//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: GISMAP.cc,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#include <amis/GISMAP.h>
#include <amis/GISMAPLauncherItem.h>

namespace {

  using namespace amis;

  GISMAPLauncherItem< GISMAP >
  gismap( EstimatorIdentifier( "GISMAP" ), "GIS with MAP estimation" );

}

// end of GISMAP.cc

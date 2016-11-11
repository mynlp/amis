//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: GIS.cc,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#include <amis/GIS.h>
#include <amis/GISLauncherItem.h>
#include <amis/Property.h>

namespace {

  using namespace amis;

  GISLauncherItem< GIS > gis( EstimatorIdentifier( "GIS" ), "Generalized Iterative Scaling" );

}

// end of GIS.cc

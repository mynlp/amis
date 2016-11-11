//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: GaussianPrior.cc,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#include <amis/GaussianPrior.h>
#include <amis/GaussianPriorLauncher.h>

AMIS_NAMESPACE_BEGIN

Initializer< Launcher< GaussianPriorPtr, Property* >* >* GaussianPriorLauncher::queue = NULL;

PropertyItem< Real > GaussianPriorLauncher::map_sigma( "MAP_SIGMA", "--map-sigma", "-s", 1.0, "Standard deviation of parameters used for MAP estimation" );

AMIS_NAMESPACE_END

namespace {

  using namespace amis;

  GaussianPriorLauncherItem< GaussianPriorSingle > single( "single", "Single value for all variances" );
  GaussianPriorLauncherItem< GaussianPriorInvEmp > inv_emp( "invemp", "Inverse of empirical expectations" );

}

// end of GaussianPrior.cc

//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: Real.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_Real_h_

#define Amis_Real_h_

#include <amis/configure.h>

#include <cfloat>
#include <cmath>

#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>
#endif

#ifdef HAVE_LIMITS
#include <limits>


AMIS_NAMESPACE_BEGIN

#ifdef AMIS_USE_LONG_DOUBLE
typedef long double Real;
#else // AMIS_USE_LONG_DOUBLE
typedef double Real;
#endif // AMIS_USE_LONG_DOUBLE

const Real REAL_MAX = std::numeric_limits<Real>::max();
const Real REAL_MIN = std::numeric_limits<Real>::min();
const Real REAL_EPSILON = std::numeric_limits<Real>::epsilon();
const Real REAL_TOL = REAL_MIN * 1.0E100;
const Real REAL_INF = std::numeric_limits<Real>::max() * std::numeric_limits<Real>::max();

AMIS_NAMESPACE_END

#else // HAVE_LIMITS

#include <climits>

AMIS_NAMESPACE_BEGIN

#ifdef AMIS_USE_LONG_DOUBLE
typedef long double Real;
const Real REAL_MAX = DBL_MAX;
const Real REAL_MIN = DBL_MIN;
const Real REAL_EPSILON = DBL_EPSILON;
const Real REAL_TOL = REAL_MIN * 1.0E100;
const Real REAL_INF = DBL_MAX * DBL_MAX;
#else // AMIS_USE_LONG_DOUBLE
typedef double Real;
const Real REAL_MAX = DBL_MAX;
const Real REAL_MIN = DBL_MIN;
const Real REAL_EPSILON = DBL_EPSILON;
const Real REAL_TOL = REAL_MIN * 1.0E100;
const Real REAL_INF = DBL_MAX * DBL_MAX;
#endif // AMIS_USE_LONG_DOUBLE

AMIS_NAMESPACE_END

#endif // HAVE_LIMITS

#endif // Real_h_
// end of Real.h

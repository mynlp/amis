//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: Feature.cc,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#include <amis/Feature.h>
#include <climits>

AMIS_NAMESPACE_BEGIN

const BinaryFeature::FeatureFreq BinaryFeature::MAX_FEATURE_FREQ = INT_MAX;
const BinaryFeature::FeatureFreq BinaryFeature::MIN_FEATURE_FREQ = 0;

const IntegerFeature::FeatureFreq IntegerFeature::MAX_FEATURE_FREQ = INT_MAX;
const IntegerFeature::FeatureFreq IntegerFeature::MIN_FEATURE_FREQ = 0;

const RealFeature::FeatureFreq RealFeature::MAX_FEATURE_FREQ = REAL_MAX;
const RealFeature::FeatureFreq RealFeature::MIN_FEATURE_FREQ = 0.0;

AMIS_NAMESPACE_END

// end of Feature.cc

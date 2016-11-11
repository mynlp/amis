//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: Feature.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_Feature_h_
#define Amis_Feature_h_

#include <amis/configure.h>
#include <amis/Real.h>
#include <amis/ErrorBase.h>
#include <amis/objstream.h>
#include <string>

#ifdef HAVE_LIMITS
#include <limits>
#else // HAVE_LIMITS
#include <climits>
#endif // HAVE_LIMITS

AMIS_NAMESPACE_BEGIN

// typedef int FeatureID
typedef unsigned int FeatureID;
#ifdef HAVE_LIMITS
const FeatureID UNKNOWN_FEATURE_ID = std::numeric_limits< FeatureID >::max();
#else // HAVE_LIMITS
const FeatureID UNKNOWN_FEATURE_ID = UINT_MAX;
#endif // HAVE_LIMITS

//////////////////////////////////////////////////////////////////////

/**
 * This class signals an illegal feature functions found.
 */

class IllegalFeatureError : public ErrorBase {
 public:
  /// Constructor
  IllegalFeatureError( const std::string& s ) : ErrorBase( s ) {}
  /// Constructor
  IllegalFeatureError( const char* s ) : ErrorBase( s ) {}
  /// Destructor
  virtual ~IllegalFeatureError() {}
};

//////////////////////////////////////////////////////////////////////

/**
 * Binary-valued feature function.
 * An implementation of feature functions which returns binary values.
 */

class BinaryFeature {
public:
  /// Type of feature frequency
  typedef int FeatureFreq;

private:
  FeatureID feature_id;

public:
  /// Maximum value of a feature frequency
  static const FeatureFreq MAX_FEATURE_FREQ;
  /// Minimum value of a feature frequency
  static const FeatureFreq MIN_FEATURE_FREQ;
  /// Get the type of the features
  static const std::string featureTypeName() { return "BinaryFeature"; }
public:
  /// Make a feature without initial values
  BinaryFeature() {
    feature_id = 0;
  }
  /// Make a feature with ID = i, frequency = f
  BinaryFeature( FeatureID i, FeatureFreq f = 1 ) {
    if ( f != 1 ) throw( IllegalFeatureError( "Feature frequency must be 1 (BinaryFeature)" ) );
    feature_id = i;
  }

  /// Set the feature ID and frequency
  void set( FeatureID i, FeatureFreq f ) {
	  feature_id   = i;
  }
  
  /// The ID of the feature
  FeatureID id() const {
    return feature_id;
  }
  /// The frequency of the feature
  FeatureFreq freq() const {
    return 1;
  }

  /// Read a Feature object from a stream
  void readObject( objstream& is ) {
    is >> feature_id;
  }
  /// Write a Feature object to a stream
  void writeObject( objstream& os ) const
  {
    os << feature_id;
  }
  
  /// comparator for sorting
  bool operator<(const BinaryFeature& f) const {
	  return feature_id < f.id();
  }
};

//////////////////////////////////////////////////////////////////////

/**
 * Integer-valued feature function.
 * An implementation of feature functions which returns integer values.
 */

class IntegerFeature {
public:
  typedef int FeatureFreq;

private:
  FeatureID feature_id;
  FeatureFreq feature_freq;

public:
  /// Maximum value of a feature frequency
  static const FeatureFreq MAX_FEATURE_FREQ;
  /// Minimum value of a feature frequency
  static const FeatureFreq MIN_FEATURE_FREQ;
  /// Get the type of the features
  static const std::string featureTypeName() { return "IntegerFeature"; }
public:
  /// Make a feature without initial values
  IntegerFeature() {
    feature_id = 0;
    feature_freq = MIN_FEATURE_FREQ;
  }
  /// Make a feature with ID = i, frequency = f
  IntegerFeature( FeatureID i, FeatureFreq f = 1 ) {
    //if ( f <= 0 ) throw( IllegalFeatureError( "Feature frequency must be non positive (IntegerFeature)" ) );
    feature_id = i;
    feature_freq = f;
  }

  /// Set the ID and frequency
  void set( FeatureID i, FeatureFreq f ) {
	  feature_id   = i;
	  feature_freq = f;
  }
  
  /// The ID of the feature
  FeatureID id() const {
    return feature_id;
  }
  /// The frequency of the feature
  FeatureFreq freq() const {
    return feature_freq;
  }

  /// Read a Feature object from a stream
  void readObject( objstream& is ) {
    is >> feature_id;
    is >> feature_freq;
  }
  /// Write a Feature object to a stream
  void writeObject( objstream& os ) const
  {
    os << feature_id;
    os << feature_freq;
  }
  
  /// comparator for sorting
  bool operator<(const IntegerFeature& f) const {
	  return feature_id < f.id();
  }
};

//////////////////////////////////////////////////////////////////////

/**
 * Real-valued feature function.
 * An implementation of feature functions which returns real values.
 */

class RealFeature {
public:
  typedef Real FeatureFreq;

private:
  FeatureID feature_id;
  FeatureFreq feature_freq;

public:
  /// Maximum value of a feature frequency
  static const FeatureFreq MAX_FEATURE_FREQ;
  /// Minimum value of a feature frequency
  static const FeatureFreq MIN_FEATURE_FREQ;
  /// Get the type of the features
  static const std::string featureTypeName() { return "RealFeature"; }
public:
  /// Make a feature without initial values
  RealFeature() {
    feature_id = 0;
    feature_freq = MIN_FEATURE_FREQ;
  }
  /// Make a feature with ID = i, frequency = f
  RealFeature( FeatureID i, FeatureFreq f = 1 ) {
//     if ( f <= 0.0 ) {
// 		AMIS_DEBUG_MESSAGE(2, "f=" << f << '\n' << std::flush);
// 	    throw( IllegalFeatureError( "Feature frequency must be non positive (RealFeature)" ) );
// 	}
    feature_id = i;
    feature_freq = f;
  }

  /// Set the ID and frequency
  void set( FeatureID i, FeatureFreq f ) {
	  feature_id   = i;
	  feature_freq = f;
  }
  
  /// The ID of the feature
  FeatureID id() const {
    return feature_id;
  }
  /// The frequency of the feature
  FeatureFreq freq() const {
    return feature_freq;
  }

  /// Read a Feature object from a stream
  void readObject( objstream& is ) {
    is >> feature_id;
    is >> feature_freq;
  }
  /// Write a Feature object to a stream
  void writeObject( objstream& os ) const
  {
    os << feature_id;
    os << feature_freq;
  }
  
  /// comparator for sorting
  bool operator<(const RealFeature& f) const {
	  return feature_id < f.id();
  }
};

AMIS_NAMESPACE_END

#endif // Feature_h_
// end of Feature_h_

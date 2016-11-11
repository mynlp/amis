//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: ParameterValue.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_ParameterValue_h_

#define Amis_ParameterValue_h_

#include <amis/configure.h>
#include <amis/Profile.h>
#include <amis/Real.h>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>

#include <climits>
#include <cmath>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

namespace Limits {
  const Real LAMBDA_MAX    = 1.0E+9;
  const Real LAMBDA_MIN    = -1.0E+9;
  const Real MAX_EXPONENT  = 1.0E+100;
  const Real LOG_PROB_ZERO = log( REAL_MIN );
  const Real PROB_ZERO     = REAL_MIN;
}

class LambdaValue {
private:
  Real lambda;

public:
  /// Constructor

  static Real defaultValue()
  {
    return 0.0;
  }

  LambdaValue()
  {
    lambda = 0.0;
  }
  explicit LambdaValue(const Real& l)
  {
    lambda = l;
  }

  /// Get value
  Real getLambda() const {
    return lambda;
  }

  Real safeGetLambda() const {
    return lambda;
  }

  Real getAlpha() const {
    return exp( lambda );
  }

  Real getValue() const {
    return lambda;
  }

  /// Clear the value
  void clear() {
    lambda = -REAL_INF;
  }

  void initValue( Real v ) {
    lambda = v;
  }
  void initAlpha( Real a ) {
    lambda = log( a );
  }
  /// Initialize the value with lambda
  void initLambda( Real l ) {
    lambda = l;
  }

  void ensureSafeValue(Real max_val, Real small_val) {
    /*if( !finite( value ) ) {
      AMIS_DEBUG_MESSAGE(2, "model expect value is not finite\n");
      value = max_val;
    }
    else if( value == 0.0) {
      AMIS_DEBUG_MESSAGE(2, "model expect value is zero\n" );
      value = small_val;
      }*/
  }

  void incQuotient( LambdaValue arg1, LambdaValue arg2 ) {
    Real l = arg1.lambda - arg2.lambda;
    Real scale = std::max( lambda, l );
    if ( scale <= -REAL_INF ) {
      lambda = -REAL_INF;
    }
    else {
      lambda = log( exp( lambda - scale ) + exp( l - scale ) ) + scale;
    }
  }

  bool isZero() {
    return lambda <= Limits::LOG_PROB_ZERO;
  }

  /// set reciprocal number
  void reciprocal() {
    lambda = -lambda;
  }

  /// Same as *=
  LambdaValue& operator*=( LambdaValue x ) {
    lambda += x.lambda;
    return *this;
  }

  /// Same as *=
  LambdaValue& operator*=( Real x ) {
    lambda += log( x );
    return *this;
  }

  /// Set the value to the summation of alphas corresponding to the members of index_list
  template < class Vector, class ID >
  void accumulate( const Vector& map_list, const std::vector< ID >& index_list ) {
    accumulate( map_list, index_list.begin(), index_list.end() );
  }
  template < class Vector, class InputIterator >
  void accumulate( const Vector& map_list, InputIterator begin, InputIterator end ) {
    Real scale = -REAL_INF;
    for ( InputIterator it = begin; it != end; ++it ) {
      scale = std::max( scale, map_list[ *it ].lambda );
    }
    if ( scale <= -REAL_INF ) {
      lambda = -REAL_INF;
      return;
    }
    //std::cerr << "scale = " << scale << std::endl;
    Real sum = 0.0;
    for ( InputIterator it = begin; it != end; ++it ) {
      //std::cerr << "map_list = " << map_list[*it].lambda << std::endl;
      sum += exp( map_list[ *it ].lambda - scale );
    }
    //std::cerr << "sum = " << sum << std::endl;
    lambda = log( sum ) + scale;
  }

  template<class Iter>
  static void scale_prob(Iter p, Iter end) {
    Real max_weight = -REAL_INF;
    for ( Iter i = p; i != end; ++ i) {
      max_weight = std::max(i->getValue(), max_weight);
    }
    for( ; p != end; ++p ) {
      p->initValue(p->getValue() - max_weight );
    }
  }

  template < class ParameterSpace, class FeatureList >
  static Real exponent( const ParameterSpace& parameterspace, const FeatureList& fl ) {
    Real sum = 0.0;
    for ( typename FeatureList::const_iterator feature = fl.begin(); feature != fl.end(); ++feature ) {
      sum += parameterspace[feature->id()].getLambda() * feature->freq();
    }
    if( !finite( sum ) && sum > 0.0 ) {
      sum = Limits::MAX_EXPONENT;
    }
    return sum;
  }


  template < class ParameterSpace, class FeatureList, class FeatureFreq, class Vector >
  static void exponent_fix( const ParameterSpace& parameterspace, const FeatureList& fv, FeatureFreq& freq, Vector& prod) {
    typename FeatureList::const_iterator end = fv.end();
    for( typename FeatureList::const_iterator it = fv.begin();
	 it != end;
	 ++it ) {
      prod[it->first].lambda += parameterspace[it->second].getLambda() * freq;
    }
  }

  template < class Vector >
  static Real scale_weight( Vector& fp, Vector& sw)
  {
    // do nothing
    /*Real scale = -REAL_INF;
    for ( typename Vector::const_iterator it = fp.begin();
	  it != fp.end();
	  ++it ) {
      if ( it->lambda > scale ) {
	scale = it->lambda;
      }
    }
    if ( scale <= -REAL_INF ) return;

    for ( size_t i = 0; i < fp.size(); ++i ) {
      fp[i].lambda -= scale;
      sw[i].lambda -= scale;
      }*/
    return 1.0;
  }

  /// Same as +=
  LambdaValue& operator+=( const LambdaValue& x ) {
    Real a = lambda;
    Real b = x.lambda;
    if ( a > b ) {
      a = x.lambda;
      b = lambda;
    }
    if ( b <= -REAL_INF ) {
      lambda = -REAL_INF;
    }
    else {
      lambda = b + log( 1.0 + exp( a - b ) );
    }
    return *this;
  }
  /// Same as +=
  LambdaValue& operator+=( const Real& x ) {
    Real a = lambda;
    Real b = log( x );
    if ( a > b ) {
      a = b;
      b = lambda;
    }
    if ( b <= -REAL_INF ) {
      lambda = -REAL_INF;
    }
    else {
      lambda = b + log( 1.0 + exp( a - b ) );
    }
    return *this;
  }

  LambdaValue& incLambda( const Real& x ) {
    lambda += x;
    return *this;
  }

  LambdaValue& multLambda( const Real& x ) {
    lambda *= x;
    return *this;
  }
};

/// Print the value
inline std::ostream& operator<<( std::ostream& os, const LambdaValue& x ) {
  os << x.getValue();
  return os;
}

inline LambdaValue product( const LambdaValue arg1, const LambdaValue arg2 ) {
  LambdaValue ret( arg1.getLambda() + arg2.getLambda() );
  return ret;
}

inline LambdaValue quotient( const LambdaValue arg1, const LambdaValue arg2 ) {
  LambdaValue ret( arg1.getLambda() - arg2.getLambda() );
  return ret;
}


class AlphaValue {
private:
  Real alpha;

public:
  /// Constructor

  static Real defaultValue()
  {
    return 1.0;
  }

  AlphaValue()
  {
    alpha = 1.0;
  }

  explicit AlphaValue(const Real& l)
  {
    alpha = exp( l );
  }
  
  /// Get value
  Real getLambda() const {
    return log( alpha );
  }

  Real safeGetLambda() const {
    Real ret = log( alpha );
    return ( finite( ret ) ? ret : Limits::LOG_PROB_ZERO );
  }

  Real getAlpha() const {
    return alpha;
  }

  Real getValue() const {
    return alpha;
  }

  /// Clear the value
  void clear() {
    alpha = 0.0;
  }

  void initValue( Real v ) {
    alpha = v;
  }
  void initAlpha( Real a ) {
    alpha = a;
  }
  /// Initialize the value with lambda
  void initLambda( Real l ) {
    alpha = exp( l );
  }

  void ensureSafeValue(Real max_val, Real small_val) {
    if( !finite( alpha ) ) {
      AMIS_DEBUG_MESSAGE(2, "model expect value is not finite\n");
      alpha = max_val;
    }
    else if( alpha == 0.0) {
      AMIS_DEBUG_MESSAGE(2, "model expect value is zero\n" );
      alpha = small_val;
    }
  }

  void incQuotient( AlphaValue arg1, AlphaValue arg2 ) {
    alpha += arg1.alpha / arg2.alpha;
  }

  bool isZero() {
    return alpha <= Limits::PROB_ZERO;
  }

  /// set reciprocal number
  void reciprocal() {
    alpha = 1.0 / alpha;
  }

  /// Same as *=
  AlphaValue& operator*=( AlphaValue x ) {
    alpha *= x.alpha;
    return *this;
  }

  /// Same as *=
  AlphaValue& operator*=( Real x ) {
    alpha *= x;
    return *this;
  }

  /// Set the value to the summation of alphas corresponding to the members of index_list
  template < class Vector, class ID >
  void accumulate( const Vector& map_list, const std::vector< ID >& index_list ) {
    accumulate( map_list, index_list.begin(), index_list.end() );
  }
  template < class Vector, class InputIterator >
  void accumulate( const Vector& map_list, InputIterator begin, InputIterator end ) {
    Real a = 0.0;
    for ( InputIterator it = begin; it != end; ++it ) {
      a += map_list[ *it ].alpha;
    }
    alpha = a;
  }

  template<class Iter>
  static void scale_prob(Iter , Iter ) { }

  template < class ParameterSpace, class FeatureList >
  static Real exponent( const ParameterSpace& parameterspace, const FeatureList& fl ) {
    Real prod = 1.0;
    for ( typename FeatureList::const_iterator feature = fl.begin(); feature != fl.end(); ++feature ) {
      prod *= power( parameterspace[feature->id()].getAlpha(), feature->freq() );
    }
    if ( ! finite( prod ) ) {
      prod = exp( Limits::MAX_EXPONENT );
    }
    return prod;
  }

  template < class ParameterSpace, class FeatureList, class FeatureFreq, class Vector >
  static void exponent_fix( const ParameterSpace& parameterspace, const FeatureList& fv, FeatureFreq& freq, Vector& prod)
  {
    for ( typename FeatureList::const_iterator it = fv.begin();
	  it != fv.end();
	  ++it ) {
      Real tmp = parameterspace[it->second].getAlpha();
      if ( tmp != 1.0 ) {
	prod[it->first] *= power( tmp, freq );
	//prod[it->first] *= parameterspace[it->second].getAlpha();
      }
    }
  }

  template < class Vector >
  static Real scale_weight( Vector& fp, Vector& sw)
  {
    Real scale = 0.0;
    for ( typename Vector::const_iterator it = fp.begin();
	  it != fp.end();
	  ++it ) {
      if ( it->alpha > scale ) {
	scale = it->alpha;
      }
    }
    if ( scale <= 0.0 || !finite(scale) ) return 1.0;

    for ( size_t i = 0; i < fp.size(); ++i ) {
      fp[i].alpha /= scale;
    }
    for ( size_t i = 0; i < sw.size(); ++i ) {
      sw[i].alpha /= scale;
    }
    return scale;
  }


  /// Same as +=
  AlphaValue& operator+=( const AlphaValue& x ) {
    alpha += x.alpha;
    return *this;
  }
  /// Same as +=
  AlphaValue& operator+=( const Real& x ) {
    alpha += x;
    return *this;
  }

  AlphaValue& incLambda( const Real& x ) {
    alpha *= exp( x );
    return *this;
  }

  AlphaValue& multLambda( const Real& x ) {
    alpha = pow(alpha, x);
    return *this;
  }

  static Real power(Real a, Real x)
  {
    //std::cerr << "real power" << std::endl;
    return pow(a, x);
  }
  static Real power(Real a, int x)
  {
    //std::cerr << "int power" << std::endl;
    Real p = 1.0;
    for ( ; x != 0; --x ) {
      p *= a;
    }
    return p;
  }
};

/// Print the value
inline std::ostream& operator<<( std::ostream& os, const AlphaValue& x ) {
  os << x.getValue();
  return os;
}

inline AlphaValue product( const AlphaValue arg1, const AlphaValue arg2 ) {
  AlphaValue ret;
  ret.initValue( arg1.getAlpha() * arg2.getAlpha() );
  return ret;
}

inline AlphaValue quotient( const AlphaValue arg1, const AlphaValue arg2 ) {
  AlphaValue ret;
  ret.initValue( arg1.getAlpha() / arg2.getAlpha() );
  return ret;
}

AMIS_NAMESPACE_END

#endif // Amis_ParameterValue_h_
// end of ParameterValue.h

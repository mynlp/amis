//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: WeightAccumulator.h,v 1.3 2008-07-29 17:05:14 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_WeightAccumulator_h_

#define Amis_WeightAccumulator_h_

#include <amis/EventBase.h>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/**
 * The class imports event data from Amis-style data files.
 * The Amis format enables non-binary features and packed events.
 * For details, see README.
 * @see Event, EventSpace, WeightAccumulator
 */

class WeightAccumulatorError : public ErrorBase {
public:
  /// Constructor
  explicit WeightAccumulatorError( const std::string& s ) : ErrorBase( s ) {}
  /// Constructor
  explicit WeightAccumulatorError( const char* s ) : ErrorBase( s ) {}
  /// Destructor
  virtual ~WeightAccumulatorError() {}
};

class WeightAccumulator {
  size_t ignored;
public:
  WeightAccumulator()
  {
    ignored = 0;
  }

  void clearIgnored() { ignored = 0; }
  size_t numIgnored() { return ignored; }
  void incIgnored() { ++ignored; }
  virtual ~WeightAccumulator()
  { }
  virtual void empiricalCount(std::vector<Real>& ee,
			      const EventBase& event,
			      Real event_ee) = 0;
  virtual Real modelCount(std::vector<Real>& me,
			  const EventBase& event,
			  Real event_ee,
			  const ModelBase& model) = 0;

  virtual void perceptronUpdate(const EventBase& event,
				Real step_size,
				int online_iteration,
				ModelBase& model,
				std::vector<Real>& history) = 0;

};



AMIS_NAMESPACE_END

#endif // Amis_WeightAccumulator_h_
// end of WeightAccumulator.h

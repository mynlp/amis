//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: EventReader.h,v 1.3 2008-07-29 06:05:57 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_EventReader_h_

#define Amis_EventReader_h_

#include <amis/NameTable.h>
#include <amis/FeatureParser.h>
#include <amis/EventBase.h>

//#include <amis/AmisDriver.h>
//#include <amis/Property.h>
#include <amis/FeatureWeightType.h>
#include <amis/WeightAccumulator.h>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/**
 * The class imports event data from Amis-style data files.
 * The Amis format enables non-binary features and packed events.
 * For details, see README.
 * @see Event, EventSpace, EventReader
 */

class EventReader {
protected:
  NameTable* name_table;
  FeatureWeightType feature_weight_type;

  Real inputLambda( Real x ) const
  {
    switch (feature_weight_type) {
    case LAMBDA: return exp(x);
    case ALPHA: return x;
    default: return x;
    }
  }
public:
  EventReader(FeatureWeightType fwt)
    : feature_weight_type(fwt)
  { }
  virtual ~EventReader()
  { }
  virtual EventBase* allocate() const = 0;
  virtual EventBase* input( std::string& name, Tokenizer& t ) const = 0;
  virtual EventBase* inputWithProb( std::string& name, Tokenizer& t1, Tokenizer& t2 ) const = 0;
  virtual WeightAccumulator* makeAccumulator() const = 0;
  virtual void initialize() { }

  void setFeatureNameTable(NameTable* nt)
  {
    name_table = nt;
  }
  NameTable& featureNameTable()
  {
    return *name_table;
  }
};


typedef std::auto_ptr<EventReader> EventReaderPtr;


AMIS_NAMESPACE_END

#endif // Amis_EventReader_h_
// end of EventReader.h

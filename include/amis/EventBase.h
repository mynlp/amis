//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: EventBase.h,v 1.2 2008-07-28 20:30:56 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_EventBase_h_

#define Amis_EventBase_h_

#include <amis/configure.h>
#include <amis/FeatureList.h>
#include <amis/EventBase.h>
#include <amis/ErrorBase.h>
#include <amis/Real.h>
#include <amis/ModelBase.h>
#include <amis/objstream.h>
#include <amis/StringStream.h>
#include <amis/Tokenizer.h>

#include <vector>
#include <algorithm>

AMIS_NAMESPACE_BEGIN

//typedef FeatureListFreq EventFreq;

class IllegalEventError : public ErrorBase {
 public:
  IllegalEventError( const std::string& s ) : ErrorBase( s ) {}
  IllegalEventError( const char* s ) : ErrorBase( s ) {}
};

class IllegalEventFormatError : public ErrorBase {
public:
  IllegalEventFormatError( const std::string& s, int num ) : ErrorBase() {
    OStringStream os;
    os << s << " at line " << num + 1;
    m = os.str();
  }
  /// Initialize with an error message
  IllegalEventFormatError( const char* s, int num ) : ErrorBase() {
    OStringStream os;
    os << s << " at line " << num + 1;
    m = os.str();
  }
  /// Initialize with an error message
};

class IllegalModelExpectationError : public ErrorBase {
public:
  IllegalModelExpectationError( const std::string& s ) : ErrorBase( s ) {}
  /// Initialize with an error message
  IllegalModelExpectationError( const char* s ) : ErrorBase( s ) {}
  /// Initialize with an error message
};


class EventReader;
//////////////////////////////////////////////////////////////////////

/// <classdef>
/// <name>Event</name>
/// <overview>An event</overview>
/// <desc>
/// This class describes an event object.
/// </desc>
/// <body>

class EventBase {
public:
  //typedef FeatureList< Feature >* iterator;
  //typedef const FeatureList< Feature >* const_iterator;

  size_t freq_count;
  
public:
  EventBase() {
    freq_count = 0;
  }

  virtual ~EventBase() {}

  //virtual bool inputEvent( const EventReader* event_reader_base,
  //Tokenizer& t ) = 0;
  //virtual bool inputEventWithProb( const EventReader* event_reader_base,
  //Tokenizer& t1,
  //Tokenizer& t2 ) = 0;


  void clear()
  {
    freq_count = 0;
  }
  
  virtual size_t numFeatureList() const = 0;
  Real eventFrequency() const {
    return freq_count;
  }

  Real eventProbability() const {
    return 1.0;
  }
  void setEventProbability( Real p ) { }

  //virtual Real featureCount( size_t id ) const = 0;

  virtual Real maxFeatureCount() const = 0;

  ////////////////////////////////////////////////////////////

public:
  virtual void readObject( objstream& is ) = 0;

  /// Read an object from a stream
  virtual void writeObject( objstream& os ) const = 0;

};

AMIS_NAMESPACE_END

/// </body>
/// </classdef>

#endif // EventBase_h_
// end of Event.h

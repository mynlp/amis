//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: EventSpace.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_EventSpace_h_

#define Amis_EventSpace_h_

#include <amis/configure.h>
#include <amis/EventBase.h>
#include <amis/VectorOnFile.h>
#include <amis/Tokenizer.h>
#include <amis/EventReader.h>
#include <amis/WeightAccumulator.h>

#include <vector>


AMIS_NAMESPACE_BEGIN

typedef size_t EventCount;


//////////////////////////////////////////////////////////////////////

/// <classdef>
/// <name>EventSpace</name>
/// <overview>A base class for implementation of a set of events</overview>
/// <desc>A set of all possible events in the task</desc>
/// <body>

//template < class Feature, class Event >
class EventSpace {//: public EventSpaceBase {
public:
  //typedef Feature FeatureType;
  //typedef Event EventType;
  //typedef typename Feature::FeatureFreq FeatureFreq;
  EventCount num_events;
  EventCount num_feature_lists;
  Real   sum_event_count;
private:
  Real max_feature_count;
  mutable bool is_dirty;
  mutable Real event_empirical_expectation;
  WeightAccumulator* accumulator;

public:
  EventSpace()
  {
    is_dirty = true;
    num_events = 0;
    num_feature_lists = 0;
    sum_event_count = 0.0;
    max_feature_count = 0.0;
  }
  virtual ~EventSpace() {}

protected:
  //virtual void push( Event* event ) = 0;
  /// Add a new event to an event list

public:
  virtual const EventBase& operator[]( int i ) const = 0;
  /// Get the i-th event

protected:
  void updateMaxFeatureCount( Real m ) {
    if ( m > max_feature_count ) max_feature_count = m;
  }
  /// Update a maximum value of feature counts if needed

public:
  Real maxFeatureCount() const {
    return max_feature_count;
  }

public:
  virtual bool inputEvent( const EventReader* , Tokenizer& t ) = 0;
  virtual bool inputEventWithProb( const EventReader* , Tokenizer& t1, Tokenizer& t2 ) = 0;

  virtual WeightAccumulator* getAccumulator()
  {
    return accumulator;
  }
  /// Add a new event to the event space
  
  const std::string eventSpaceName() const {
    return "EventSpace";
  }

  virtual Real eventEmpiricalExpectation() const
  {
    if ( is_dirty ) {
      event_empirical_expectation =
	1.0 / sum_event_count;
      is_dirty = false;
    }
    //AMIS_DEBUG_MESSAGE(3, "eventEmpiricalExpectation()=" << event_empirical_expectation << '\n' << std::flush); 
    return event_empirical_expectation;
  }

  void addEvent(const EventBase& eb)
  {
    ++num_events;
    sum_event_count += eb.eventFrequency();
    num_feature_lists += eb.numFeatureList();
    max_feature_count = std::max(max_feature_count, eb.maxFeatureCount());
  }

  virtual void validate( const EventReader* reader)
  {
    accumulator = reader->makeAccumulator();
  }

  virtual Real sumEventCount() const {
    return sum_event_count;
  }
  virtual EventCount numFeatureLists() const {
    return num_feature_lists;
  }
  virtual EventCount numEvents() const {
    return num_events;
  }
};



typedef std::auto_ptr< EventSpace > EventSpacePtr;




class EventSpaceOnMemory : public EventSpace {//: public EventSpaceBase {
private:
  std::vector< EventBase* > event_list;

public:
  EventSpaceOnMemory( void ) {
  }
  virtual ~EventSpaceOnMemory() {}

  bool inputEvent( const EventReader* reader, Tokenizer& t )
  {
    std::string event_name;
    EventBase* event = reader->input(event_name, t);
    if ( event != 0 ) {
    //event->inputEvent(reader, t);
      event_list.push_back(event);
      this->addEvent(*event);
      return true;
    }
    else {
      return false;
    }
  }
  bool inputEventWithProb( const EventReader* reader, Tokenizer& t1, Tokenizer& t2 )
  {
    std::string event_name;
    EventBase* event = reader->inputWithProb(event_name, t1, t2);
    if ( event != 0 ) {
    //event->inputEvent(reader, t);
      event_list.push_back(event);
      this->addEvent(*event);
      return true;
    }
    else {
      return false;
    }
  }
  
  virtual const EventBase& operator[]( int i ) const
  {
    return *event_list[i];
  }

  const std::string eventSpaceName() const {
    return "EventSpace";
  }
};



class EventSpaceOnFile : public EventSpace {
private:
  VectorOnFile< EventBase > event_vector;
  EventBase* workspace;

protected:
  void push( EventBase* event )
  {
    event_vector.push_back( *event );
    this->addEvent(*event);
    delete event;
  }
  /// Push a new event to an event space

public:
  EventSpaceOnFile( objstream* s ) : event_vector( s ) {
    //std::cerr << s << std::endl;
  }
  /// Constructor for EventSpaceOnFile
  virtual ~EventSpaceOnFile() {}
  /// Destructor for EventSpaceOnFile
  
  virtual void clearSub() {
    event_vector.clear(); 
  }
  
  const std::string eventSpaceName( void ) const
  {
    return "EventSpaceOnFile";
  }
  /// Get the name of this class

  bool inputEvent( const EventReader* reader, Tokenizer& t )
  {
    std::string event_name;
    EventBase* event = reader->input(event_name, t);
    if ( event != 0 ) {
    //event->inputEvent(reader, t);
      this->push(event);
      return true;
    }
    else {
      return false;
    }
  }
  bool inputEventWithProb( const EventReader* reader, Tokenizer& t1, Tokenizer& t2 )
  {
    std::string event_name;
    EventBase* event = reader->inputWithProb(event_name, t1, t2);
    if ( event != 0 ) {
      //event->inputEvent(reader, t);
      this->push(event);
      return true;
    }
    else {
      return false;
    }
  }

  void validate( const EventReader* reader)
  {
    EventSpace::validate(reader);
    event_vector.validate(*reader->allocate());
  }

  const EventBase& operator[]( int i ) const
  {
    return event_vector[ i ];
  }
  /// Get the i-th event
};



AMIS_NAMESPACE_END

/// </body>
/// </classdef>

#endif // EventSpace_h_
// end of EventSpace.h

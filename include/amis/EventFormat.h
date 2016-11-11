//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: EventFormat.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_EventFormat_h_

#define Amis_EventFormat_h_

#include <amis/configure.h>
#include <amis/FeatureParser.h>
#include <amis/NameTable.h>
#include <amis/EventSpace.h>
#include <amis/Tokenizer.h>
#include <amis/EventReader.h>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/**
 * The class imports event data from Amis-style data files.
 * The Amis format enables non-binary features and packed events.
 * For details, see README.
 * @see Event, EventSpace, EventFormat
 */

class EventFormat {
public:
  /// Feature frequency
  //typedef typename Feature::FeatureFreq FeatureFreq;

private:
  EventReader* event_reader;

public:
  /// Constructor
  EventFormat(EventReader* er)
    : event_reader(er)
  { }
  /// Destructor
  virtual ~EventFormat() { }

  /// Name of the event format
  std::string eventFormatName() const {
    return "AmisEvent";
  }

  /*/// Output event and probabilities to stream
  void outputProb( std::ostream& os, const NameTable<Name>& name_table, const Event< Feature >& event, const Real* prob ) const {
    os << current_event;
    os << '\n';
    for ( typename Event< Feature >::const_iterator it = event.begin(); it != event.end(); ++it, ++prob ) {
      os << *prob << '\n';
    }
    os << '\n';
  }

  /// Input a probability of an event
  bool inputProb( Tokenizer& t,
                  NameTable<Name>& name_table, 
                  Event< Feature >& event ) const {
    if ( ! t.nextToken( current_event ) ) return false; // empty line
    if ( t.nextToken( dummy ) ) {
      throw IllegalEventFormatError( "Too many tokens found in probability file in Event " + current_event, t.lineNumber() );
    }
    for ( typename Event< Feature >::iterator fl = event.begin();
	  fl != event.end();
	  ++fl ) {
      Real p = 0.0;
      if ( t.nextToken( p ) ) {
	fl->setReferenceProbability( this->inputLambda(p) );
      } else {
        throw IllegalEventFormatError( "Cannot read probability value in Event " + current_event, t.lineNumber() );
      }
      if ( t.nextToken( dummy ) ) {
	throw IllegalEventFormatError( "More than one probability values found in Event " + current_event, t.lineNumber() );
      }
    }
    if ( t.nextToken( dummy ) ) {
      throw IllegalEventFormatError( "Too many lines for probability values found in Event " + current_event, t.lineNumber() );
    }
    return true;
    }*/

  /// Input an event from tokenizer
  /// Input event data from an input stream
  virtual void inputEventSpace( std::istream& s,
				EventSpace& event_space ) const  {
    AMIS_PROF1( "EventFormat::inputEventSpace" );
    Tokenizer t( s );
    AMIS_DEBUG_MESSAGE( 3, "\nInput events...\n" );
    AMIS_DEBUG_MESSAGE( 5, "\t----------------------------------------\n" );
    AMIS_DEBUG_MESSAGE( 5, "\tEvent\tFreq.\t# FLs\n" );

    try {
      //while ( ! t.endOfStream() ) {
      while ( event_space.inputEvent(event_reader, t) ) {
      }
    }
    catch ( TokenError& e ) {
      throw IllegalEventFormatError( e.message(), t.lineNumber() );
    }
    event_space.validate(event_reader);
  }

    /*        if ( inputEvent( t, name_table, event ) ) {
          AMIS_DEBUG_MESSAGE( 5, '\t' << current_event << '\t' << event.eventFrequency() << '\t' << event.numFeatureList() + 1 << '\n' );
          try {
            event_space.validateEvent( event );
          } catch ( IllegalEventError& e ) {
            throw IllegalEventFormatError( e.message() + " in Event " + current_event, t.lineNumber() );
          }
        }
	else {
	  event_space.deleteEvent();
	}
      }
    } catch ( TokenError& e ) {
      throw IllegalEventFormatError( e.message() + " in Event " + current_event, t.lineNumber() );
    }
    AMIS_DEBUG_MESSAGE( 5, "\t----------------------------------------\n" );
    }*/

  /// Interface defined by class EventFormat
    /*  void inputEventSpaceWithProb( std::istream& s1, std::istream& s2, NameTable< Name >& name_table, EventSpaceBase& event_space_base ) const {
    EventSpace< Feature, Event< Feature > >* event_space =
      dynamic_cast< EventSpace< Feature, Event< Feature > >* >( &event_space_base );
    if ( event_space == NULL ) {
      throw IllegalEventFormatError( "EventFormat can be used only for EventSpace class", 0 );
    }
    inputEventSpaceWithProb( s1, s2, name_table, *event_space );
    }*/

  /// Input event data and its reference distribution from input streams
  virtual void inputEventSpaceWithProb( std::istream& s1, std::istream& s2,
					EventSpace& event_space ) const {
    Tokenizer t1( s1 );
    Tokenizer t2( s2 );
    try {
      //while ( ! t1.endOfStream() ) {
      while ( event_space.inputEventWithProb(event_reader, t1, t2) ) {
      }
    }
    catch ( TokenError& e ) {
      throw IllegalEventFormatError( e.message(), t1.lineNumber() );
    }
    event_space.validate(event_reader);
  }
  /*   while ( ! t1.endOfStream() ) {
        Event< Feature >& event = event_space.createEvent();
        if ( inputEvent( t1, name_table, event ) ) {
          std::string event_name( current_event );
          while ( ! inputProb( t2, name_table, event ) ) {
            if ( t2.endOfStream() ) {
              throw IllegalEventFormatError( "Probability file is shorter than event file", t1.lineNumber() );
            }
          }
          if ( current_event != event_name ) {
            throw IllegalEventFormatError( "Event names are inconsistent: " + event_name + " vs. " + current_event, t1.lineNumber() );
          }
          try {
            event_space.validateEvent( event );
          }
	  catch ( IllegalEventError& e ) {
            throw IllegalEventFormatError( e.message() + " in Event " + current_event, t1.lineNumber() );
          }
        }
	else {
	  event_space.deleteEvent();
	}
      }
    } catch ( TokenError& e ) {
      throw IllegalEventFormatError( e.message() + " in Event " + current_event, t1.lineNumber() );
    }
    }*/
};

AMIS_NAMESPACE_END

#endif // Amis_EventFormat_h_
// end of EventFormat.h

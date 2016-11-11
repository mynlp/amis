//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: EmpiricalExpect.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_EmpiricalExpect_h_

#define Amis_EmpiricalExpect_h_

#include <amis/configure.h>
#include <amis/Real.h>
#include <amis/ModelBase.h>
#include <amis/EventSpace.h>
#include <functional>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/**
 * Empirical expectation of features
 */

class EmpiricalExpect : public std::vector<Real> {
public:
  bool calculated;
  const ModelBase* model;
  EventSpace* event_space;

  bool isCalculated() const { return calculated; }
public:
  /// Constructor
  EmpiricalExpect()
  {
    model = NULL;
    event_space = NULL;
  }
  /// Destructor
  virtual ~EmpiricalExpect() {}

public:
  /// Initialize with a model and an event space
  void initialize( const ModelBase* init_model, EventSpace* init_event_space ) {
    model = init_model;
    event_space = init_event_space;
    assign( model->numFeatures(), 0.0 );
  }
  /// Set empirical expectations of features
  void setEmpiricalExpectation() {
    AMIS_DEBUG_MESSAGE( 5, "Start setEmpiricalExpectation" );
    for( EventCount i = 0; i < event_space->numEvents(); ++i ) {
      AMIS_DEBUG_MESSAGE( 5, "Event " << i << "\n" );
      const EventBase& event = (*event_space)[ i ];
      event_space->getAccumulator()->empiricalCount(*this, event,
						    event_space->eventEmpiricalExpectation());
      //EmpiricalFeatureAccumulator< Event >().accumulate( event, EmpiricalExpectFunction< Feature >( *this, event.eventFrequency() ) );
    }
    AMIS_DEBUG_MESSAGE( 5, "END 1st for loop\n" );

    calculated = true;
    
    AMIS_DEBUG_MESSAGE( 5, "END setEmpiricalExpectation( void )\n" );
  }

  virtual void debugInfo( std::ostream& ostr ) const
  {
    for ( std::vector< Real >::const_iterator it = this->begin();
          it != this->end();
          ++it ) {
      ostr << *it << '\n';
    }
  }
};

inline std::ostream& operator<<( std::ostream& os, const EmpiricalExpect& e ) {
  e.debugInfo( os );
  return os;
}

//////////////////////////////////////////////////////////////////////

AMIS_NAMESPACE_END

#endif // Amis_EmpiricalExpect_h_
// end of EmpiricalExpect.h

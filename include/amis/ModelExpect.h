//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: ModelExpect.h,v 1.2 2008-07-29 17:05:14 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_ModelExpect_h_

#define Amis_ModelExpect_h_


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

class ModelExpect : public std::vector<Real> {
public:
  const ModelBase* model;
  EventSpace* event_space;

public:
  /// Constructor
  ModelExpect()
  {
    model = NULL;
    event_space = NULL;
  }
  /// Destructor
  virtual ~ModelExpect() {}

public:
  /// Initialize with a model and an event space
  void initialize( const ModelBase* init_model, EventSpace* init_event_space ) {
    model = init_model;
    event_space = init_event_space;
    assign( model->numFeatures(), 0.0 );
  }
  /// Set empirical expectations of features
  Real setModelExpectation()
  {
    Real log_likelihood = 0.0;
    AMIS_DEBUG_MESSAGE( 5, "Start setModelExpectation" );
    this->assign(this->size(), 0.0);

    
    event_space->getAccumulator()->clearIgnored();
    for( EventCount i = 0; i < event_space->numEvents(); ++i ) {
      AMIS_DEBUG_MESSAGE( 5, "Event " << i << "\n" );
      const EventBase& event = (*event_space)[ i ];
      log_likelihood +=
	event_space->getAccumulator()->modelCount(*this, event,
						  event_space->eventEmpiricalExpectation(),
						  *model);
    }
    if ( event_space->getAccumulator()->numIgnored() > 0 ) {
      AMIS_WARNING_MESSAGE(event_space->getAccumulator()->numIgnored() << " events are ignored because of infinite weight sum\n");
    }
    AMIS_DEBUG_MESSAGE( 5, "END 1st for loop\n" );

    //std::cerr << std::endl;
    AMIS_DEBUG_MESSAGE( 5, "END setModelExpectation( void )\n" );

    //std::cerr << "log_likelihood = " << log_likelihood << std::endl;
    return log_likelihood;
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

inline std::ostream& operator<<( std::ostream& os, const ModelExpect& e ) {
  e.debugInfo( os );
  return os;
}

//////////////////////////////////////////////////////////////////////

AMIS_NAMESPACE_END

#endif // ModelExpect_h_
// end of ModelExpect.h

//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: Initializer.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_Initializer_h_

#define Amis_Initializer_h_

#include <amis/configure.h>
#include <utility>

AMIS_NAMESPACE_BEGIN

template < class ArgType >
class Initializer {
private:
  int priority;
  Initializer* next;

public:
  Initializer( Initializer*& queue, int pr = 0 ) {
    priority = pr;
    Initializer** pointer = &queue;
    while ( *pointer != NULL ) {
      if ( pr <= (*pointer)->priority ) break;
      pointer = &( (*pointer)->next );
    }
    this->next = *pointer;
    *pointer = this;
  }
  virtual ~Initializer() {}

  virtual void start( ArgType arg ) = 0;

  int getPriority( void ) const {
    return priority;
  }

  static void initAll( Initializer* queue, ArgType arg ) {
    Initializer* q = queue;
    while ( q != NULL ) {
      q->start( arg );
      q = q->next;
    }
  }
};

AMIS_NAMESPACE_END

#endif // Amis_Initializer_h_
// end of Initializer_h_

//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: EventSpaceLauncher.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#ifndef Amis_EventSpaceLauncher_h_

#define Amis_EventSpaceLauncher_h_

#include <amis/configure.h>
#include <amis/Launcher.h>
#include <amis/EventSpace.h>
#include <amis/Utility.h>

AMIS_NAMESPACE_BEGIN

class Property;

class EventSpaceLauncher : public Launcher< EventSpacePtr, Property*, std::string > {
private:
  static Initializer< Launcher< EventSpacePtr, Property*, std::string >* >* queue;

public:
  EventSpaceLauncher( void ) : Launcher< EventSpacePtr, Property*, std::string >( "EventSpaceLauncher", queue ) {}
  virtual ~EventSpaceLauncher() {}

  template < class EventSpaceOnMemory, class EventSpaceOnFile >
  friend class EventSpaceLauncherItem;
};

AMIS_NAMESPACE_END

#endif // Amis_EventSpaceLauncher_h_
// end of EventSpaceLauncher.h

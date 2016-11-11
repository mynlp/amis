//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: EventSpace.cc,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#include <amis/EventSpace.h>
#include <amis/EventSpaceLauncherItem.h>

AMIS_NAMESPACE_BEGIN

Initializer< Launcher< EventSpacePtr, Property*, std::string >* >* EventSpaceLauncher::queue = NULL;

AMIS_NAMESPACE_END

namespace {

  using namespace amis;

  EventSpaceLauncherItem< EventSpaceOnMemory, EventSpaceOnFile >
  event_space( "Event", "Standard event space" );

}

// end of EventSpace.cc

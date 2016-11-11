//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: EventSpaceLauncherItem.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_EventSpaceLauncherItem_h_

#define Amis_EventSpaceLauncherItem_h_

#include <amis/configure.h>
#include <amis/LauncherItem.h>
#include <amis/EventSpace.h>
#include <amis/EventSpaceLauncher.h>
#include <amis/Utility.h>
#include <amis/Property.h>

AMIS_NAMESPACE_BEGIN

template < class EventSpaceOnMemory, class EventSpaceOnFile >
class EventSpaceLauncherItem : public LauncherItem< EventSpacePtr, Property*, std::string > {
public:
  EventSpaceLauncherItem( const std::string& name, const std::string& desc = "" )
    : LauncherItem< EventSpacePtr, Property*, std::string >( EventSpaceLauncher::queue, name , desc )
  { }
  virtual ~EventSpaceLauncherItem() {}
  EventSpacePtr launch( Property* property ) {
    if ( property->isEventOnFile() ) {
      assert(false);
      objstream* os = property->eventOnFileStream();
      return EventSpacePtr( new EventSpaceOnFile( os ) );
    }
    else {
      return EventSpacePtr( new EventSpaceOnMemory() );
    }
  }
};

AMIS_NAMESPACE_END

#endif // Amis_EventSpaceLauncherItem_h_
// end of EventSpaceLauncherItem.h

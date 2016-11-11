//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: EventReaderLauncherItem.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#ifndef Amis_EventReaderLauncherItem_h_

#define Amis_EventReaderLauncherItem_h_

#include <amis/configure.h>
#include <amis/LauncherItem.h>
#include <amis/EventReaderInst.h>
#include <amis/EventReaderLauncher.h>
#include <amis/Utility.h>
#include <amis/Property.h>

AMIS_NAMESPACE_BEGIN

template < class EventReader >
class EventReaderLauncherItem : public LauncherItem< EventReaderPtr, Property*, EventReaderIdentifier > {
public:
  EventReaderLauncherItem( const EventReaderIdentifier& id, const std::string& desc = "" )
    : LauncherItem< EventReaderPtr, Property*, EventReaderIdentifier >( EventReaderLauncher::queue, id, desc ) {}
  virtual ~EventReaderLauncherItem() {}
  EventReaderPtr launch( Property* property )
  {
    return EventReaderPtr( new EventReader(property) );
  }
};

AMIS_NAMESPACE_END

#endif // Amis_EventReaderLauncherItem_h_
// end of EventReaderLauncherItem.h

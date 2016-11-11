//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: EventReaderLauncher.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#ifndef Amis_EventReaderLauncher_h_

#define Amis_EventReaderLauncher_h_

#include <amis/configure.h>
#include <amis/Launcher.h>
#include <amis/EventReader.h>
#include <amis/Utility.h>

AMIS_NAMESPACE_BEGIN

class Property;
class EventReaderIdentifier {
private:
  std::string model;
  std::string event;
  std::string feature;
  std::string parameter;
  bool use_reference;

public:
  EventReaderIdentifier(const std::string& e,
			const std::string& f,
			const std::string& p,
			bool ref )
    : event( e ), feature( f ), parameter(p), use_reference( ref ) { }
  virtual ~EventReaderIdentifier() {}

  const std::string& getEvent() const { return event; }
  const std::string& getParameter() const { return parameter; }
  const std::string& getFeature() const { return feature; }
  const bool useReference() const { return use_reference; }

  bool operator<( const EventReaderIdentifier& id ) const {
    return (event < id.event ||
	    (event == id.event &&
	     (feature < id.feature ||
	      (feature == id.feature &&
	       (parameter < id.parameter ||
		(parameter == id.parameter &&
		 use_reference < id.use_reference))))));
  }
};

inline std::ostream& operator<<( std::ostream& os, const EventReaderIdentifier& id ) {
  os << '('
     << id.getEvent() << ", "
     << id.getFeature() << ", "
     << id.getParameter();
  if ( id.useReference() ) {
    os << ", with reference distribution";
  }
  return os << ')';
}


class EventReaderLauncher : public Launcher< EventReaderPtr, Property*, EventReaderIdentifier > {
private:
  static Initializer< Launcher< EventReaderPtr, Property*, EventReaderIdentifier >* >* queue;

public:
  EventReaderLauncher( void ) : Launcher< EventReaderPtr, Property*, EventReaderIdentifier >( "EventReaderLauncher", queue ) {}
  virtual ~EventReaderLauncher() {}

  template < class EventReader >
  friend class EventReaderLauncherItem;
};

AMIS_NAMESPACE_END

#endif // Amis_EventReaderLauncher_h_
// end of EventReaderLauncher.h

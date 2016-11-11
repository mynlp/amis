//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: PropertyItem.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_PropertyItem_h_

#define Amis_PropertyItem_h_

#include <amis/configure.h>
#include <amis/Option.h>

AMIS_NAMESPACE_BEGIN

class PropertyItemBase {
public:
  static Initializer< OptionManager* >* OPTION_QUEUE;

public:
  PropertyItemBase( void ) {}
  virtual ~PropertyItemBase() {}
};

//////////////////////////////////////////////////////////////////////

template < class T >
class PropertyItem : public Option< T >, PropertyItemBase {
public:
  PropertyItem( const std::string& n,
		const std::string& ln,
		const std::string& sn,
		const T& def,
		const std::string& desc = "",
		bool ao = true )
    : Option< T >( PropertyItemBase::OPTION_QUEUE,
		   n, ln, sn, def, desc, ao )
  { }
  virtual ~PropertyItem() {}
};

//////////////////////////////////////////////////////////////////////

template < class T >
class PropertyItemVector : public OptionVector< T >, PropertyItemBase {
public:
  PropertyItemVector( const std::string& n,
		      const std::string& ln,
		      const std::string& sn,
		      const std::vector< T >& def,
		      const std::string& desc = "",
		      bool ao = true )
    : OptionVector< T >( PropertyItemBase::OPTION_QUEUE,
			 n, ln, sn, def, desc, ao ) {}
  virtual ~PropertyItemVector() {}
};

//////////////////////////////////////////////////////////////////////

class PropertyItemToggle : public OptionToggle, PropertyItemBase {
public:
  PropertyItemToggle( const std::string& n,
		      const std::string& ln,
		      const std::string& sn,
		      bool def = false,
		      const std::string& desc = "",
		      bool ao = true )
    : OptionToggle( PropertyItemBase::OPTION_QUEUE,
		    n, ln, sn, def, desc, ao ) {}
  virtual ~PropertyItemToggle() {}
};

//////////////////////////////////////////////////////////////////////

template < class T >
class PropertyItemNamedInt : public OptionNamedInt< T >, PropertyItemBase {
public:
  PropertyItemNamedInt( const std::string& n,
			const std::string& ln,
			const std::string& sn,
			const StringHash< T >& names,
			T def = 0,
			const std::string& desc = "",
			bool ao = true )
    : OptionNamedInt< T >( PropertyItemBase::OPTION_QUEUE,
			   n, ln, sn, names, def, desc, ao ) {}
  virtual ~PropertyItemNamedInt() {}
};

AMIS_NAMESPACE_END

#endif // Amis_PropertyItem_h_
// end of PropertyItem.h

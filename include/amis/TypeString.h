//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: TypeString.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_TypeString_h_
#define Amis_TypeString_h_

#include <amis/configure.h>
#include <amis/StringStream.h>

#include <vector>
#include <string>

AMIS_NAMESPACE_BEGIN

template<class T>
class TypeString {
  public:
	static std::string getString() {
		return "unknown";
	}
};

template<typename T> 
class TypeString< std::vector< T > > {
	public:
  static std::string getString() {
		OStringStream os;
		os << "vector<" << TypeString<T>::getString() << ">";
		return os.str();
	}
};

template<> class TypeString<int> {
	public:
  static std::string getString() {
		return "int";
	}
};

template<> class TypeString<bool> {
	public:
  static std::string getString() {
		return "bool";
	}
};

template<> class TypeString<double> {
	public:
  static std::string getString() {
		return "double";
	}
};

template<> class TypeString<std::string> {
	public:
  static std::string getString() {
		return "string";
	}
};



AMIS_NAMESPACE_END

#endif

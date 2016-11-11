//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: StringStream.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_StringStream_h_

#define Amis_StringStream_h_

#include <amis/configure.h>
#ifdef HAVE_SSTREAM
#include <sstream>
#else // HAVE_SSTREAM
#include <strstream>
#endif // HAVE_SSTREAM
#include <iostream>

AMIS_NAMESPACE_BEGIN

#ifdef HAVE_SSTREAM
class IStringStream : public std::istringstream
{
	public:
	IStringStream( const char* s ) : std::istringstream( s ) { }
	IStringStream( const std::string& s ) : std::istringstream( s ) { }
};
#else
class IStringStream : public std::istrstream
{
	public:
	IStringStream( const char* s ) : std::istrstream( s ) { }
	IStringStream( const std::string& s ) : std::istrstream( s ) { }
};
#endif


class OStringStream : 
#ifdef HAVE_SSTREAM
public std::ostringstream
#else
public std::ostrstream
#endif
{
public:
  OStringStream( void ) {}
  ~OStringStream() {
#ifndef HAVE_SSTREAM
    freeze( false );
#endif // HAVE_SSTREAM
  }

#ifndef HAVE_SSTREAM
public:
  std::string str( void ) {
    (*this) << std::ends;
    return std::string( std::ostrstream::str() );
  }
#endif // HAVE_SSTREAM
};

AMIS_NAMESPACE_END

#endif // Amis_StringStream_h_
// end of StringStream.h

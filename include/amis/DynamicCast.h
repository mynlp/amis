//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: DynamicCast.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#ifndef Amis_DynamicCast_h_
#define Amis_DynamicCast_h_

#include <amis/ErrorBase.h>
#include <string>
#include <amis/StringStream.h>

AMIS_NAMESPACE_BEGIN

class DynamicCastError : public ErrorBase {
	public:
	explicit DynamicCastError( const std::string& s ) : ErrorBase( s ) { }
	explicit DynamicCastError( const char* s ) : ErrorBase( s ) {}
	virtual ~DynamicCastError() {}
};

template< class T, class F > 
T DynamicCast(F from, const std::string& position_info ) {
  T to = T();
  to = dynamic_cast< T >( from );
  if ( to == T() ) {
    OStringStream oss;
    oss << "DynamicCast failed: IN " << position_info;
    throw DynamicCastError( oss.str() );
  }
  return to;
}

AMIS_NAMESPACE_END

#endif

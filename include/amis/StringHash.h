//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: StringHash.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#ifndef Amis_StringHash_h_

#define Amis_StringHash_h_

#include <amis/configure.h>
#include <string>

AMIS_NAMESPACE_BEGIN

template < class Obj >
class StringHash : public amis_hash_map< std::string, Obj > {};


AMIS_NAMESPACE_END

#endif // StringHash_h_
// end of StringHash.h

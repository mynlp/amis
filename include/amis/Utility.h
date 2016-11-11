//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: Utility.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#ifndef Amis_Utility_h_

#define Amis_Utility_h_

#include <amis/configure.h>
#include <iostream>
#include <cmath>

AMIS_NAMESPACE_BEGIN

class StringPair : public std::pair< std::string, std::string > {
public:
  StringPair( const std::string& s1, const std::string& s2 )
    : std::pair< std::string, std::string >( s1, s2 ) {}
};

inline std::ostream& operator<<( std::ostream& os, const StringPair& s ) {
  return os << '(' << s.first << ',' << s.second << ')';
}

class StringTriple : public std::pair< std::string, std::pair< std::string, std::string > > {
public:
  StringTriple( const std::string& s1, const std::string& s2, const std::string& s3 )
    : std::pair< std::string, std::pair< std::string, std::string > >( s1, std::pair< std::string, std::string >( s2, s3 ) ) {}
};

inline std::ostream& operator<<( std::ostream& os, const StringTriple& s ) {
  return os << '(' << s.first << ',' << s.second.first << ',' << s.second.second << ')';
}

class StringQuad : public std::pair< std::string, std::pair< std::string, std::pair< std::string, std::string > > > {
public:
  StringQuad( const std::string& s1, const std::string& s2, const std::string& s3, const std::string& s4 )
    : std::pair< std::string, std::pair< std::string, std::pair< std::string, std::string > > >( s1, std::pair< std::string, std::pair< std::string, std::string > >( s2, std::pair< std::string, std::string >(s3, s4) ) ) {}
};

inline std::ostream& operator<<( std::ostream& os, const StringQuad& s ) {
  return os << '(' << s.first << ',' << s.second.first << ','
	    << s.second.second.first << ',' << s.second.second.second << ')';
}

class Utility {
  public:
  
  static void find_max( Real* v, size_t size, Real* max_v, int* max_i ) {
    Real* p = v;
    
    Real mv = *p;
    int  mi = 0;
    
    ++p;
    for( size_t i = 1; i < size; ++i, ++p ) {
      if( *p > mv ) {
        mv = *p;
        mi = i;
      }
    }
    
    *max_v = mv;
    *max_i = mi;
  }
  
  
  
};

AMIS_NAMESPACE_END

#endif // Amis_Utility_h_
// end of Utility.h

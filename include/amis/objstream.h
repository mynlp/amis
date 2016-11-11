//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: objstream.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_objstream_h_
#define Amis_objstream_h_   

#include <amis/configure.h>
#include <amis/LogStream.h>

#include <iostream>
#include <fstream>
#include <string>

AMIS_NAMESPACE_BEGIN

class objstream : public std::fstream
{
private:
  std::string name;
  objstream(std::fstream* st){
    AMIS_ABORT( "Not yet implemented. Please tell me how to make a stream from another stream (kazama@is.s.u-tokyo.ac.jp)." );
  }

public:
  objstream(const std::string& filename,
            std::ios::openmode mode = std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc ) : std::fstream(filename.c_str(), mode) {
    name = filename;
    seekg(0);
  }
  /// Make a new objstream of a specified file name

  const std::string& fileName( void ) const { return name; }
  /// Get a name of the file

  template< class T > objstream& operator<< (const T& obj);
  /// Input from an objstream
  template< class T > objstream& operator>> (T& obj);
  /// Output to an objstream
};

template< class T >
objstream& objstream::operator<<( const T& obj )
{
  int len = sizeof( obj );
  write( reinterpret_cast< const char* >( &obj ), len );
  return *this;
}

template< class T >
objstream& objstream::operator>>( T& obj )
{
  int len = sizeof( obj );
  read( reinterpret_cast< char* >( &obj ), len );
  return *this;
}

AMIS_NAMESPACE_END

#endif // objstream_h_
// end of objstream.h

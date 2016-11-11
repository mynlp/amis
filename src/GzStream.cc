//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: GzStream.cc,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#include <amis/GzStream.h>

#ifdef AMIS_ZLIB

//#include <zutil.h>
#include <zlib.h>

AMIS_NAMESPACE_BEGIN

const int ASCII_FLAG  = 0x01;
const int HEAD_CRC    = 0x02;
const int EXTRA_FIELD = 0x04;
const int ORIG_NAME   = 0x08;
const int COMMENT     = 0x10;
const int RESERVED    = 0xE0;

const char gz_magic_number[] = { 0x1f, 0x8b };
//const char gz_header[] = { gz_magic_number[ 0 ], gz_magic_number[ 1 ], Z_DEFLATED, 0, 0, 0, 0, 0, 0, OS_CODE };
const char gz_header[] = { gz_magic_number[ 0 ], gz_magic_number[ 1 ], Z_DEFLATED, 0, 0, 0, 0, 0, 0, 0 };

bool OGzStreamBuf::writeGzHeader() {
  out_stream.write( gz_header, sizeof( gz_header ) );
  return out_stream;
}

bool IGzStreamBuf::readGzHeader() {
  char buffer[10];
  in_stream.read( buffer, 10 );
  //std::cerr << "gcount=" << in_stream.gcount() << std::endl;
  //std::cerr << "buffer=" << buffer[0] << ' ' << buffer[1] << std::endl;
  //std::cerr << "magic=" << gz_magic_number[0] << ' ' << gz_magic_number[1] << std::endl;
  if ( in_stream.gcount() < 10 ) return false;
  if ( ! ( buffer[ 0 ] == gz_magic_number[ 0 ] &&
           buffer[ 1 ] == gz_magic_number[ 1 ] ) ) return false;
  if ( buffer[ 2 ] != Z_DEFLATED ) return false;
  int flag = buffer[ 3 ];
  //std::cerr << "flag=" << std::hex << flag << std::endl;
  if ( ( flag & EXTRA_FIELD ) != 0 ) {
    //std::cerr << "extra field" << std::endl;
    in_stream.read( buffer, 2 );
    if ( in_stream.gcount() < 2 ) return false;
    int len = buffer[ 0 ] + ( buffer[ 1 ] << 8 );
    //std::cerr << "len=" << len << std::endl;
    in_stream.ignore( len );
    if ( ! in_stream ) return false;
  }
  if ( ( flag & ORIG_NAME ) != 0 ) {
    //std::cerr << "orig name" << std::endl;
    while ( in_stream ) {
      //std::cerr << in_stream.peek() << std::endl;
      if ( in_stream.get() == '\0' ) break;
    }
    if ( ! in_stream ) return false;
  }
  if ( ( flag & COMMENT ) != 0 ) {
    //std::cerr << "comment" << std::endl;
    while ( in_stream ) {
      //std::cerr << in_stream.peek() << std::endl;
      if ( in_stream.get() == '\0' ) break;
    }
    if ( ! in_stream ) return false;
  }
  if ( ( flag & HEAD_CRC ) != 0 ) {
    //std::cerr << "head crc" << std::endl;
    in_stream.read( buffer, 2 );
    if ( in_stream.gcount() < 2 ) return false;
    if ( ! in_stream ) return false;
  }
  //std::cerr << "end" << std::endl;
  return true;
}

AMIS_NAMESPACE_END

#endif // AMIS_ZLIB

// end of GzStream.cc

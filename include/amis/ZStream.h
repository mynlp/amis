//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: ZStream.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_ZStream_h_

#define Amis_ZStream_h_

#include <amis/configure.h>
#include <amis/ErrorBase.h>
#include <iostream>
#include <fstream>
#ifdef AMIS_ZLIB
#include <zlib.h>
#endif // AMIS_ZLIB

AMIS_NAMESPACE_BEGIN

/**
 * This class signals an error on zlib
 */
class ZStreamError : public ErrorBase {
public:
  /// Initialize with an error message
  explicit ZStreamError( const std::string& s ) : ErrorBase( s ) {}
  /// Initialize with an error message
  explicit ZStreamError( const char* s ) : ErrorBase( s ) {}
  /// Destructor
  virtual ~ZStreamError() {}
};

/**
 * error that zlib is not supported
 */
class ZStreamUnsupportedError : public ErrorBase {
public:
  /// Initialize with an error message
  explicit ZStreamUnsupportedError( const std::string& s ) : ErrorBase( s ) {}
  /// Initialize with an error message
  explicit ZStreamUnsupportedError( const char* s ) : ErrorBase( s ) {}
  /// Destructor
  virtual ~ZStreamUnsupportedError() {}
};

#ifdef AMIS_ZLIB //////////////////////////////////////////////////////////////////////

/**
 * c++-stream-like interface for zlib.  Input stream.
 */

class IZStreamBuf : public std::streambuf {
public:
  /// Type of size
  typedef size_t size_type;
  /// Type of data
  typedef char char_type;
  /// Default size of input buffer
  static const size_type DEFAULT_BUFFER_SIZE = 8192;

private:
  z_stream z;
  std::istream& in_stream;
  size_type in_buffer_size;
  char_type* in_buffer;
  size_type out_buffer_size;
  char_type* out_buffer;

public:
  /// Constructor
  explicit IZStreamBuf( std::istream& is, size_type s = DEFAULT_BUFFER_SIZE )
    : in_stream( is ) {
    z.next_in = Z_NULL;
    z.avail_in = 0;
    z.zalloc = Z_NULL;
    z.zfree = Z_NULL;
    z.opaque = Z_NULL;
    if ( inflateInit( &z ) != Z_OK ) {
      throw ZStreamError( "Initialization of zlib failed" );
    }
    in_buffer_size = s;
    in_buffer = new char_type[ in_buffer_size ];
    out_buffer_size = s;
    out_buffer = new char_type[ out_buffer_size ];
  }
  /// Destructor
  virtual ~IZStreamBuf() {
    delete [] out_buffer;
    delete [] in_buffer;
    if ( inflateEnd( &z ) != Z_OK ) {
      throw ZStreamError( "Finalization of zlib failed" );
    }
  }

public:
  /// Called when out_buffer is empty
  int underflow() {
    z.next_out = reinterpret_cast< Bytef* >( out_buffer );
    z.avail_out = out_buffer_size;
    while ( true ) {
      if ( z.avail_in == 0 ) {
        in_stream.read( in_buffer, in_buffer_size );
        if ( in_stream.gcount() == 0 ) return EOF;
        z.next_in = reinterpret_cast< Bytef* >( in_buffer );
        z.avail_in = in_stream.gcount();
      }
      int status = inflate( &z, Z_NO_FLUSH );
      if ( status == Z_STREAM_END ) {
        if ( out_buffer_size == z.avail_out ) {
          return EOF;
        }
        std::streambuf::setg( out_buffer, out_buffer, out_buffer + out_buffer_size - z.avail_out );
        return *out_buffer;
      }
      std::cerr << status << std::endl;
      if ( status != Z_OK ) throw ZStreamError( "Decompression by zlib failed" );
      if ( z.avail_out == 0 ) {
        std::streambuf::setg( out_buffer, out_buffer, out_buffer + out_buffer_size );
        return *out_buffer;
      }
    }
  }
};

//////////////////////////////////////////////////////////////////////

/**
 * c++-stream-like interface for zlib.  Output stream.
 */

class OZStreamBuf : public std::streambuf {
public:
  /// Type of size
  typedef size_t size_type;
  /// Type of data
  typedef char char_type;
  /// Default size of input buffer
  static const size_type DEFAULT_BUFFER_SIZE = 8192;
  /// Default compression level (used by zlib)
  static const int DEFAULT_COMPRESSION = Z_DEFAULT_COMPRESSION;

private:
  z_stream z;
  std::ostream& out_stream;
  size_type in_buffer_size;
  char_type* in_buffer;
  size_type out_buffer_size;
  char_type* out_buffer;

protected:
  /// Compress the data in the buffer
  void compress() {
    z.next_in = reinterpret_cast< Bytef* >( in_buffer );
    z.avail_in = std::streambuf::pptr() - in_buffer;
    z.next_out = reinterpret_cast< Bytef* >( out_buffer );
    z.avail_out = out_buffer_size;
    while ( true ) {
      int status = deflate( &z, Z_NO_FLUSH );
      if ( status != Z_OK ) throw ZStreamError( "Compression by zlib failed" );
      out_stream.write( out_buffer, out_buffer_size - z.avail_out );
      if ( z.avail_in == 0 ) return;
      z.next_out = reinterpret_cast< Bytef* >( out_buffer );
      z.avail_out = out_buffer_size;
    }
  }

  /// Close the stream
  void finalize() {
    int status = Z_OK;
    do {
      z.next_out = reinterpret_cast< Bytef* >( out_buffer );
      z.avail_out = out_buffer_size;
      status = deflate( &z, Z_FINISH );
      if ( status != Z_OK && status != Z_STREAM_END ) throw ( "Compression by zlib failed" );
      out_stream.write( out_buffer, out_buffer_size - z.avail_out );
    } while ( status == Z_OK );
  }

public:
  /// Constructor
  explicit OZStreamBuf( std::ostream& os,
			 size_type s = DEFAULT_BUFFER_SIZE,
			 int comp_level = DEFAULT_COMPRESSION )
    : out_stream( os ) {
    z.zalloc = Z_NULL;
    z.zfree = Z_NULL;
    z.opaque = Z_NULL;
    if ( deflateInit( &z, comp_level ) != Z_OK ) {
      throw ZStreamError( "Initialization of zlib failed" );
    }
    in_buffer_size = s;
    in_buffer = new char_type[ in_buffer_size ];
    setp( in_buffer, in_buffer + in_buffer_size );
    out_buffer_size = s;
    out_buffer = new char_type[ out_buffer_size ];
  }
  /// Destructor
  virtual ~OZStreamBuf() {
    sync();
    finalize();
    delete [] out_buffer;
    delete [] in_buffer;
    if ( deflateEnd( &z ) != Z_OK ) {
      throw ZStreamError( "Finalization of zlib failed" );
    }
  }

  /// Flush the output
  int sync() {
    compress();
    return 0;
  }

public:
  /// Called when out_buffer is empty
  int overflow( int c = EOF ) {
    compress();
    if ( c == EOF ) {
      finalize();
      return EOF;
    } else {
      setp( in_buffer, in_buffer + in_buffer_size );
      *in_buffer = c;
      pbump( 1 );
      return c;
    }
  }
};

#else // AMIS_ZLIB //////////////////////////////////////////////////////////////////////

class IZStreamBuf : public std::streambuf {
public:
  typedef size_t size_type;
  typedef char char_type;
  static const size_type DEFAULT_BUFFER_SIZE = 8192;

private:
  //istream* in_stream;
  //size_type size;

public:
  explicit IZStreamBuf( std::istream&, size_type = DEFAULT_BUFFER_SIZE ) {
    throw ZStreamUnsupportedError( "zlib not supported" );
  }
  virtual ~IZStreamBuf() {
  }
};

//////////////////////////////////////////////////////////////////////

class OZStreamBuf : public std::streambuf {
public:
  typedef size_t size_type;
  typedef char char_type;
  static const size_type DEFAULT_BUFFER_SIZE = 8192;
  static const int DEFAULT_COMPRESSION = 6;

private:
  //ostream* out_stream;
  //size_type size;

public:
  explicit OZStreamBuf( std::ostream&,
			 size_type = DEFAULT_BUFFER_SIZE,
			 int = DEFAULT_COMPRESSION ) {
    throw ZStreamUnsupportedError( "zlib not supported" );
  }
  virtual ~OZStreamBuf() {
  }
};

#endif // AMIS_ZLIB //////////////////////////////////////////////////////////////////////

/**
 * Input stream for uncompressing
 */

class IZStream : public std::istream {
private:
  IZStreamBuf iz_buf;
public:
  /// Constructor with an input stream and buffer size
  explicit IZStream( std::istream& is, size_t s = IZStreamBuf::DEFAULT_BUFFER_SIZE )
    : std::istream( NULL ), iz_buf( is, s ) {
    init( &iz_buf );
  }
  /// Destructor
  virtual ~IZStream() {
  }
};

/**
 * Input stream for compressed files
 */

class IZFStream : public std::istream {
private:
  std::ifstream fs;
  IZStreamBuf iz_buf;
public:
  /// Constructor with a file name and buffer size
  explicit IZFStream( const std::string& name, size_t s = IZStreamBuf::DEFAULT_BUFFER_SIZE )
    : std::istream( NULL ), fs( name.c_str() ), iz_buf( fs, s ) {
    init( &iz_buf );
  }
  /// Constructor with a file name and buffer size
  explicit IZFStream( const char* name, size_t s = IZStreamBuf::DEFAULT_BUFFER_SIZE )
    : std::istream( NULL ), fs( name ), iz_buf( fs, s ) {
    init( &iz_buf );
  }
  /// Destructor
  virtual ~IZFStream() {
  }
};

/**
 * Output stream for compressing
 */

class OZStream : public std::ostream {
private:
  OZStreamBuf oz_buf;
public:
  /// Constructor with an output stream and buffer size
  explicit OZStream( std::ostream& os, size_t s = OZStreamBuf::DEFAULT_BUFFER_SIZE )
    : std::ostream( NULL ), oz_buf( os, s ) {
    init( &oz_buf );
  }
  // Destructor
  virtual ~OZStream() {
  }
};

/**
 * Output stream for compressing files
 */

class OZFStream : public std::ostream {
private:
  std::ofstream fs;
  OZStreamBuf oz_buf;
public:
  /// Constructor with a file name and buffer size
  explicit OZFStream( const std::string& name, size_t s = OZStreamBuf::DEFAULT_BUFFER_SIZE )
    : std::ostream( NULL ), fs( name.c_str() ), oz_buf( fs, s ) {
    init( &oz_buf );
  }
  /// Constructor with a file name and buffer size
  explicit OZFStream( const char* name, size_t s = OZStreamBuf::DEFAULT_BUFFER_SIZE )
    : std::ostream( NULL ), fs( name ), oz_buf( fs, s ) {
    init( &oz_buf );
  }
  /// Destructor
  virtual ~OZFStream() {
  }
};

AMIS_NAMESPACE_END

#endif // Amis_ZStream_h_
// end of ZStream.h

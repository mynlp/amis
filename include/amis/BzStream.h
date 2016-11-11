//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: BzStream.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_BzStream_h_

#define Amis_BzStream_h_

#include <amis/configure.h>
#include <amis/ErrorBase.h>
#include <iostream>
#include <fstream>
#ifdef AMIS_BZLIB
#define BZ_NO_STDIO
#include <bzlib.h>
#endif // AMIS_BZLIB

AMIS_NAMESPACE_BEGIN

/**
 * This class signals an error on bzlib
 */
class BzStreamError : public ErrorBase {
public:
  /// Initialize with an error message
  explicit BzStreamError( const std::string& s ) : ErrorBase( s ) {}
  /// Initialize with an error message
  explicit BzStreamError( const char* s ) : ErrorBase( s ) {}
  /// Destructor
  virtual ~BzStreamError() {}
};

/**
 * error that bzlib is not supported
 */
class BzStreamUnsupportedError : public ErrorBase {
public:
  /// Initialize with an error message
  explicit BzStreamUnsupportedError( const std::string& s ) : ErrorBase( s ) {}
  /// Initialize with an error message
  explicit BzStreamUnsupportedError( const char* s ) : ErrorBase( s ) {}
  /// Destructor
  virtual ~BzStreamUnsupportedError() {}
};

#ifdef AMIS_BZLIB //////////////////////////////////////////////////////////////////////

/**
 * c++-stream-like interface for bzlib.  Input stream.
 */

class IBzStreamBuf : public std::streambuf {
public:
  /// Type of size
  typedef size_t size_type;
  /// Type of data
  typedef char char_type;
  /// Default size of input buffer
  static const size_type DEFAULT_BUFFER_SIZE = 8192;

private:
  bz_stream bz;
  std::istream& in_stream;
  size_type in_buffer_size;
  char_type* in_buffer;
  size_type out_buffer_size;
  char_type* out_buffer;

public:
  /// Constructor
  explicit IBzStreamBuf( std::istream& is, size_type s = DEFAULT_BUFFER_SIZE )
    : in_stream( is ) {
    bz.next_in = NULL;
    bz.avail_in = 0;
    bz.bzalloc = NULL;
    bz.bzfree = NULL;
    bz.opaque = NULL;
    if ( BZ2_bzDecompressInit( &bz, 0, 0 ) != BZ_OK ) {
      throw BzStreamError( "Initialization of bzlib failed" );
    }
    in_buffer_size = s;
    in_buffer = new char_type[ in_buffer_size ];
    out_buffer_size = s;
    out_buffer = new char_type[ out_buffer_size ];
  }
  /// Destructor
  virtual ~IBzStreamBuf() {
    delete [] out_buffer;
    delete [] in_buffer;
    if ( BZ2_bzDecompressEnd( &bz ) != BZ_OK ) {
      throw BzStreamError( "Finalization of bzlib failed" );
    }
  }

public:
  /// Called when out_buffer is empty
  int underflow() {
    bz.next_out = out_buffer;
    bz.avail_out = out_buffer_size;
    while ( true ) {
      if ( bz.avail_in == 0 ) {
        in_stream.read( in_buffer, in_buffer_size );
        if ( in_stream.gcount() == 0 ) return EOF;
        bz.next_in = in_buffer;
        bz.avail_in = in_stream.gcount();
      }
      int status = BZ2_bzDecompress( &bz );
      if ( status == BZ_STREAM_END ) {
        std::streambuf::setg( out_buffer, out_buffer, out_buffer + out_buffer_size - bz.avail_out );
        return *out_buffer;
      }
      if ( status != BZ_OK ) throw BzStreamError( "Decompression by bzlib failed" );
      if ( bz.avail_out == 0 ) {
        std::streambuf::setg( out_buffer, out_buffer, out_buffer + out_buffer_size );
        return *out_buffer;
      }
    }
  }
};

//////////////////////////////////////////////////////////////////////

/**
 * c++-stream-like interface for bzlib.  Output stream.
 */

class OBzStreamBuf : public std::streambuf {
public:
  /// Type of size
  typedef size_t size_type;
  /// Type of data
  typedef char char_type;
  /// Default size of input buffer
  static const size_type DEFAULT_BUFFER_SIZE = 8192;
  /// Default size of block (used by bzlib)
  static const int DEFAULT_BLOCK_SIZE = 9;

private:
  bz_stream bz;
  std::ostream& out_stream;
  size_type in_buffer_size;
  char_type* in_buffer;
  size_type out_buffer_size;
  char_type* out_buffer;

protected:
  /// Compress the data in the buffer
  void compress() {
    bz.next_in = in_buffer;
    bz.avail_in = std::streambuf::pptr() - in_buffer;
    bz.next_out = out_buffer;
    bz.avail_out = out_buffer_size;
    while ( true ) {
      int status = BZ2_bzCompress( &bz, BZ_RUN );
      if ( status != BZ_RUN_OK ) throw BzStreamError( "Compression by bzlib failed" );
      out_stream.write( out_buffer, out_buffer_size - bz.avail_out );
      if ( bz.avail_in == 0 ) return;
      bz.next_out = out_buffer;
      bz.avail_out = out_buffer_size;
    }
  }

  /// Close the stream
  void finalize() {
    int status = BZ_FINISH_OK;
    do {
      bz.next_out = out_buffer;
      bz.avail_out = out_buffer_size;
      status = BZ2_bzCompress( &bz, BZ_FINISH );
      if ( status != BZ_FINISH_OK && status != BZ_STREAM_END ) throw ( "Compression by bzlib failed" );
      out_stream.write( out_buffer, out_buffer_size - bz.avail_out );
    } while ( status == BZ_FINISH_OK );
  }

public:
  /// Constructor
  explicit OBzStreamBuf( std::ostream& os,
			 size_type s = DEFAULT_BUFFER_SIZE,
			 int block_size = DEFAULT_BLOCK_SIZE )
    : out_stream( os ) {
    bz.bzalloc = NULL;
    bz.bzfree = NULL;
    bz.opaque = NULL;
    if ( BZ2_bzCompressInit( &bz, block_size, 0, 0 ) != BZ_OK ) {
      throw BzStreamError( "Initialization of bzlib failed" );
    }
    in_buffer_size = s;
    in_buffer = new char_type[ in_buffer_size ];
    setp( in_buffer, in_buffer + in_buffer_size );
    out_buffer_size = s;
    out_buffer = new char_type[ out_buffer_size ];
  }
  /// Destructor
  virtual ~OBzStreamBuf() {
    sync();
    finalize();
    delete [] out_buffer;
    delete [] in_buffer;
    if ( BZ2_bzCompressEnd( &bz ) != BZ_OK ) {
      throw BzStreamError( "Finalization of bzlib failed" );
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

#else // AMIS_BZLIB //////////////////////////////////////////////////////////////////////

class IBzStreamBuf : public std::streambuf {
public:
  typedef size_t size_type;
  typedef char char_type;
  static const size_type DEFAULT_BUFFER_SIZE = 8192;

private:
  //istream* in_stream;
  //size_type size;

public:
  explicit IBzStreamBuf( std::istream&, size_type = DEFAULT_BUFFER_SIZE ) {
    throw BzStreamUnsupportedError( "bzlib not supported" );
  }
  virtual ~IBzStreamBuf() {
  }
};

//////////////////////////////////////////////////////////////////////

class OBzStreamBuf : public std::streambuf {
public:
  typedef size_t size_type;
  typedef char char_type;
  static const size_type DEFAULT_BUFFER_SIZE = 8192;
  static const int DEFAULT_BLOCK_SIZE = 9;

private:
  //ostream* out_stream;
  //size_type size;

public:
  explicit OBzStreamBuf( std::ostream&,
			 size_type = DEFAULT_BUFFER_SIZE,
			 int = DEFAULT_BLOCK_SIZE ) {
    throw BzStreamUnsupportedError( "bzlib not supported" );
  }
  virtual ~OBzStreamBuf() {
  }
};

#endif // AMIS_BZLIB //////////////////////////////////////////////////////////////////////

/**
 * Input stream for uncompressing
 */

class IBzStream : public std::istream {
private:
  IBzStreamBuf ibz_buf;
public:
  /// Constructor with an input stream and buffer size
  explicit IBzStream( std::istream& is, size_t s = IBzStreamBuf::DEFAULT_BUFFER_SIZE )
    : std::istream( NULL ), ibz_buf( is, s ) {
    init( &ibz_buf );
  }
  /// Destructor
  virtual ~IBzStream() {
  }
};

/**
 * Input stream for compressed files
 */

class IBzFStream : public std::istream {
private:
  std::ifstream fs;
  IBzStreamBuf ibz_buf;
public:
  /// Constructor with a file name and buffer size
  explicit IBzFStream( const std::string& name, size_t s = IBzStreamBuf::DEFAULT_BUFFER_SIZE )
    : std::istream( NULL ), fs( name.c_str() ), ibz_buf( fs, s ) {
    init( &ibz_buf );
  }
  /// Constructor with a file name and buffer size
  explicit IBzFStream( const char* name, size_t s = IBzStreamBuf::DEFAULT_BUFFER_SIZE )
    : std::istream( NULL ), fs( name ), ibz_buf( fs, s ) {
    init( &ibz_buf );
  }
  /// Destructor
  virtual ~IBzFStream() {
  }
};

/**
 * Output stream for compressing
 */

class OBzStream : public std::ostream {
private:
  OBzStreamBuf obz_buf;
public:
  /// Constructor with an output stream and buffer size
  explicit OBzStream( std::ostream& os, size_t s = OBzStreamBuf::DEFAULT_BUFFER_SIZE )
    : std::ostream( NULL ), obz_buf( os, s ) {
    init( &obz_buf );
  }
  // Destructor
  virtual ~OBzStream() {
  }
};

/**
 * Output stream for compressing files
 */

class OBzFStream : public std::ostream {
private:
  std::ofstream fs;
  OBzStreamBuf obz_buf;
public:
  /// Constructor with a file name and buffer size
  explicit OBzFStream( const std::string& name, size_t s = OBzStreamBuf::DEFAULT_BUFFER_SIZE )
    : std::ostream( NULL ), fs( name.c_str() ), obz_buf( fs, s ) {
    init( &obz_buf );
  }
  /// Constructor with a file name and buffer size
  explicit OBzFStream( const char* name, size_t s = OBzStreamBuf::DEFAULT_BUFFER_SIZE )
    : std::ostream( NULL ), fs( name ), obz_buf( fs, s ) {
    init( &obz_buf );
  }
  /// Destructor
  virtual ~OBzFStream() {
  }
};

AMIS_NAMESPACE_END

#endif // Amis_BzStream_h_
// end of BzStream.h

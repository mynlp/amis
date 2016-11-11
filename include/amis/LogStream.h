//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: LogStream.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_LogStream_h_

#define Amis_LogStream_h_

#include <amis/configure.h>
#include <amis/ErrorBase.h>

#include <iostream>
#include <fstream>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/// <classdef>
/// <name>LogStreamError</name>
/// <overview>Error in LogStream</overview>
/// <desc>
/// The error is thrown when an operation is not permitted in a stream.
/// </desc>
/// <body>

class LogStreamError : public ErrorBase {
 public:
  LogStreamError( const std::string& s ) : ErrorBase( s ) {}
  /// Initialize with an error message
  LogStreamError( const char* s ) : ErrorBase( s ) {}
  /// Initialize with an error message
};

/// </body>
/// </classdef>

//////////////////////////////////////////////////////////////////////

class LogStreamBuf : public std::streambuf {
private:
  std::string filename;
  std::ofstream* log_file;
  bool suppressed;

public:
  LogStreamBuf();
  virtual ~LogStreamBuf();

  LogStreamBuf* open( const std::string& name );
  LogStreamBuf* close();

  bool setSuppressed( bool t = true ) {
    bool old = suppressed;
    suppressed = t;
    return old;
  }
  bool isSuppressed() const {
    return suppressed;
  }

  int sync();
  int overflow( int c = EOF );
};

/// <classdef>
/// <name>LogStream</name>
/// <overview>Stream for logging</overview>
/// <desc>
/// A stream for logging messages for profiling/warning.
/// </desc>
/// <body>

class LogStream : public std::ostream {
private:
  LogStreamBuf log_buf;

public:
  static LogStream DefaultLogStream;

public:
  LogStream();
  LogStream( const std::string& name );
  virtual ~LogStream();

  bool setSuppressed( bool t = true ) {
    return log_buf.setSuppressed( t );
  }
  bool isSuppressed() const {
    return log_buf.isSuppressed();
  }

  void open( const std::string& name );
  void close();
};

/// </body>
/// </classdef>

#define AMIS_ERROR_MESSAGE( message ) \
  LogStream::DefaultLogStream << message
#define AMIS_WARNING_MESSAGE( message ) \
  LogStream::DefaultLogStream << message
#define AMIS_PROF_MESSAGE( message ) \
  LogStream::DefaultLogStream << message
#define AMIS_ABORT( message ) \
  do { \
    std::cerr << message; \
    std::cerr << "\n"; \
    LogStream::DefaultLogStream << message << '\n'; \
    abort(); \
  } while ( 0 )

AMIS_NAMESPACE_END

#endif // LogStream_h_
// end of LogStream.h

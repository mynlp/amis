//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: LogStream.cc,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#include <amis/LogStream.h>

AMIS_NAMESPACE_BEGIN

LogStreamBuf::LogStreamBuf() : log_file( NULL ), suppressed( false ) {
  setbuf( 0, 0 );
}
LogStreamBuf::~LogStreamBuf() {
  delete log_file;
}

LogStreamBuf* LogStreamBuf::open( const std::string& name ) {
  delete log_file;
  filename = name;
  log_file = new std::ofstream;
  if ( log_file == NULL ) return NULL;
  log_file->open( filename.c_str(), std::ios::out | std::ios::trunc );
  if ( *log_file ) {
    return this;
  } else {
    log_file->close();
    log_file = NULL;
    return NULL;
  }
}

LogStreamBuf* LogStreamBuf::close() {
  if ( log_file != NULL ) {
    log_file->close();
    if ( *log_file ) {
      return this;
    } else {
      return NULL;
    }
  }
  return this;
}

int LogStreamBuf::sync() {
  if ( ! suppressed ) {
    std::cerr.flush();
  }
  if ( log_file != NULL ) {
    log_file->flush();
  }
  return 0;
}

int LogStreamBuf::overflow( int c ) {
  if ( ! suppressed ) {
    std::cerr.put( c );
  }
  if ( log_file != NULL ) {
    log_file->put( c );
  }
  return c;
}

//////////////////////////////////////////////////////////////////////

LogStream LogStream::DefaultLogStream;

LogStream::LogStream() : std::ostream( NULL ), log_buf() {
  init( &log_buf );
  setf( std::ios::unitbuf );
}
LogStream::LogStream( const std::string& name ) : std::ostream( NULL ), log_buf() {
  init( &log_buf );
  open( name );
  setf( std::ios::unitbuf );
}
LogStream::~LogStream() {
}

void LogStream::open( const std::string& name ) {
  if ( ! log_buf.open( name ) ) {
    setstate( std::ios::failbit );
  }
}
void LogStream::close() {
  if ( ! log_buf.close() ) {
    setstate( std::ios::failbit );
  }
}

AMIS_NAMESPACE_END
// end of LogStream.cc

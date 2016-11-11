//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: ArgumentParser.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_ArgumentParser_h_

#define Amis_ArgumentParser_h_

#include <amis/configure.h>
#include <amis/ErrorBase.h>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/**
 * The error is thrown when an illegal argument is specified to the program.
 */

class IllegalArgumentError : public ErrorBase {
public:
  /// Initialize with an error message
  IllegalArgumentError( const std::string& m ) : ErrorBase( m ) {}
  /// Initialize with an error message
  IllegalArgumentError( const char* m ) : ErrorBase( m ) {}
  /// Destructor
  virtual ~IllegalArgumentError() {}
};

//////////////////////////////////////////////////////////////////////

/**
 * A parser for command-line arguments
 */

class ArgumentParser {
private:
  // kyoshida: don't know why they should be const
  std::vector<std::string>::const_iterator it;
  std::vector<std::string>::const_iterator args_begin;
  std::vector<std::string>::const_iterator args_end;

public:
  /// Constructor: args should not contain the program name
  ArgumentParser(const std::vector<std::string>& a)
    : it(a.begin()), args_begin(a.begin()), args_end(a.end())
  { }

  /// Destructor
  virtual ~ArgumentParser() {}

  /// All arguments are already processed?
  bool empty( void ) {
    return it == args_end;
  }

  /// Get the next argument
  std::string nextArgument( const std::string& message ) {
    if ( it != args_end ) {
      return *(it++);
    } else {
      throw IllegalArgumentError( message );
    }
  }

  /// Put back one argument
  void windBack() {
    if (it != args_begin) {
      --it;
    }
  }

  /// Get the next integer argument
  int nextArgumentInteger( const std::string& message ) {
    if (it != args_end) {
      char* end_ptr;
      int num = static_cast< int >( strtol((it++)->c_str(), &end_ptr, 0 ) );
      if ( *end_ptr != '\0' ) {
	throw IllegalArgumentError( message );
      }
      return num;
    } else {
      throw IllegalArgumentError( message );
    }
  }

  /// Get the next double argument
  double nextArgumentDouble( const std::string& message ) {
    if (it != args_end) {
      char* end_ptr;
      double num = static_cast< double >( strtod((it++)->c_str(), &end_ptr) );
      if ( *end_ptr != '\0' ) {
	throw IllegalArgumentError( message );
      }
      return num;
    } else {
      throw IllegalArgumentError( message );
    }
  }
};

AMIS_NAMESPACE_END

#endif // Amis_ArgumentParser_h_
// end of ArgumentParser.h

//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: ErrorBase.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_ErrorBase_h_
#define Amis_ErrorBase_h_

#include <amis/configure.h>

#include <string>
#include <iostream>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/**
 * The class is a base class for exception classes in this project.
 */

class ErrorBase {
protected:
  /// Error message
  std::string m;

public:
  /// Default constructor
  ErrorBase() : m() {
    //std::cerr << "making error\n" << std::flush;
  }
  /// Initialize with an error message
  explicit ErrorBase( const std::string& s ) : m( s ) { 
    //std::cerr << "making error: " << s << "\n" << std::flush;
  }
  /// Initialize with an error message
  explicit ErrorBase( const char* s ) : m( s ) {
    //std::cerr << "making error: " << s << "\n" << std::flush;
  }
  /// Destructor
  virtual ~ErrorBase() {}

  /// Get an error message
  virtual const std::string& message() const { return m; }
};


AMIS_NAMESPACE_END

#endif


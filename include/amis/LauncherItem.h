//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: LauncherItem.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_LauncherItem_h_

#define Amis_LauncherItem_h_

#include <amis/configure.h>
#include <amis/Initializer.h>
#include <amis/ErrorBase.h>
#include <string>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/// <classdef>
/// <name>IllegalLauncherItemError</name>
/// <overview>Exception for illegal launcher items</overview>
/// <body>

class IllegalLauncherItemError : public ErrorBase {
 public:
  IllegalLauncherItemError( const std::string& m ) : ErrorBase( m ) {}
  /// Initialize with an error message
  IllegalLauncherItemError( const char* m ) : ErrorBase( m ) {}
  /// Initialize with an error message
  virtual ~IllegalLauncherItemError() {}
};

/// </body>
/// </classdef>

//////////////////////////////////////////////////////////////////////

template < class T, class Arg, class Name > class Launcher;

template < class T, class Arg, class Name = std::string >
class LauncherItem : public Initializer< Launcher< T, Arg, Name >* > {
protected:
  Name name;
  std::string description;

public:
  LauncherItem( Initializer< Launcher< T, Arg, Name >* >*& queue,
		const Name& n,
		const std::string& desc = "" )
    : Initializer< Launcher< T, Arg, Name >* >( queue ), name( n ) {
    description = desc;
  }
  virtual ~LauncherItem() {}

  const Name& getName() const {
    return name;
  }
  const std::string& getDescription() const {
    return description;
  }

public:
  void start( Launcher< T, Arg, Name >* launcher ) {
    launcher->setLauncherItem( this );
  }

public:
  virtual T launch( Arg ) = 0;
};

AMIS_NAMESPACE_END

#endif // Amis_LauncherItem_h_
// end of LauncherItem.h

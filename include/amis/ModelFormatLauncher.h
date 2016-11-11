//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: ModelFormatLauncher.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#ifndef Amis_ModelFormatLauncher_h_

#define Amis_ModelFormatLauncher_h_

#include "amis/Launcher.h"
#include "amis/LauncherItem.h"
#include "amis/ModelFormat.h"

AMIS_NAMESPACE_BEGIN

class Property;

class ModelFormatLauncher : public Launcher< ModelFormatPtr, Property* > {
private:
  static Initializer< Launcher< ModelFormatPtr, Property* >* >* queue;

public:
  ModelFormatLauncher( void ) : Launcher< ModelFormatPtr, Property* >( "ModelFormatLauncher", queue ) {}
  virtual ~ModelFormatLauncher() {}

  friend class ModelFormatLauncherItem;
};

class ModelFormatLauncherItem : public LauncherItem< ModelFormatPtr, Property* > {
public:
  ModelFormatLauncherItem( const std::string& n, const std::string& desc = "" )
    : LauncherItem< ModelFormatPtr, Property* >( ModelFormatLauncher::queue, n, desc ) {}
  virtual ~ModelFormatLauncherItem() {}
  ModelFormatPtr launch( Property* property ) = 0;
};

AMIS_NAMESPACE_END

#endif // Amis_ModelLauncher_h_
// end of ModelFormatLauncher.h

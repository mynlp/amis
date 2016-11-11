//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: ModelLauncher.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#ifndef Amis_ModelLauncher_h_

#define Amis_ModelLauncher_h_

#include <amis/configure.h>
#include <amis/Launcher.h>
#include <amis/LauncherItem.h>
#include <amis/ModelBase.h>

AMIS_NAMESPACE_BEGIN

class ModelIdentifier {
private:
  std::string model;
  std::string parameter;

public:
  ModelIdentifier(const std::string& m,
		  const std::string& p)
    : model( m ), parameter( p ) { }
  virtual ~ModelIdentifier() {}

  const std::string& getModel() const { return model; }
  const std::string& getParameter() const { return parameter; }

  bool operator<( const ModelIdentifier& id ) const {
    return (model < id.model ||
	    (model == id.model &&
	     (parameter < id.parameter)));
  }
};

inline std::ostream& operator<<( std::ostream& os, const ModelIdentifier& id ) {
  os << '('
     << id.getModel() << ", "
     << id.getParameter();
  return os << ')';
}


class Property;

class ModelLauncher : public Launcher< ModelPtr, Property* , ModelIdentifier > {
private:
  static Initializer< Launcher< ModelPtr, Property*, ModelIdentifier >* >* queue;

public:
  ModelLauncher( void ) : Launcher< ModelPtr, Property* , ModelIdentifier >( "ModelLauncher", queue ) {}
  virtual ~ModelLauncher() {}

  template < class Model, class ParameterValue >
  friend class ModelLauncherItem;
};

template < class Model, class ParameterValue >
class ModelLauncherItem : public LauncherItem< ModelPtr, Property*, ModelIdentifier > {
public:
  ModelLauncherItem( const ModelIdentifier& id, const std::string& desc = "" )
    : LauncherItem< ModelPtr, Property*, ModelIdentifier >( ModelLauncher::queue, id, desc ) {}
  virtual ~ModelLauncherItem() {}
  ModelPtr launch( Property* property ) {
    return ModelPtr( new Model(new ParameterSpace<ParameterValue>) );
  }
};

AMIS_NAMESPACE_END

#endif // Amis_ModelLauncher_h_
// end of ModelLauncher.h

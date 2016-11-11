//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: DataFormat.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_DataFormat_h_

#define Amis_DataFormat_h_

#include <amis/configure.h>
#include <amis/Launcher.h>
#include <amis/LauncherItem.h>
#include <string>

AMIS_NAMESPACE_BEGIN

class Property;
class DataFormat;

/**
 * Launcher for data formats (model format and event format)
 */
class DataFormatLauncher : public Launcher< const DataFormat&, const Property* > {
private:
  static Initializer< Launcher< const DataFormat&, const Property* >* >* queue;

public:
  /// Constructor
  DataFormatLauncher() : Launcher< const DataFormat&, const Property* >( "DataFormatLauncher", queue ) {}
  /// Destructor
  virtual ~DataFormatLauncher() {}

  friend class DataFormat;
};

/**
 * Launcher item for data formats
 */
class DataFormat : public LauncherItem< const DataFormat&, const Property* > {
private:
  std::string name_table_type;
  std::string model_type;
  std::string event_type;

public:
  /// Constructor
  DataFormat( const std::string& name,
              const std::string& t, const std::string& m, const std::string& e,
              const std::string& desc = "" )
    : LauncherItem< const DataFormat&, const Property* >( DataFormatLauncher::queue, name, desc ),
      name_table_type( t ), model_type( m ), event_type( e ) {
  }
  /// Destructor
  virtual ~DataFormat() {}

  /// Get the type of NameTable
  const std::string& nameTableType() const {
    return name_table_type;
  }
  /// Get the type of Model
  const std::string& modelType() const {
    return model_type;
  }
  /// Get the type of EventSpace
  const std::string& eventType() const {
    return event_type;
  }

  /// Interface for LauncherItem
  const DataFormat& launch( const Property* ) {
    return *this;
  }
};

AMIS_NAMESPACE_END

#endif // Amis_DataFormat_h_
// end of DataFormat.h

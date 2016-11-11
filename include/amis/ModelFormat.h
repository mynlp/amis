//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: ModelFormat.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_ModelFormat_h_

#define Amis_ModelFormat_h_

#include <amis/configure.h>
#include <amis/NameTable.h>
#include <amis/ModelBase.h>
#include <amis/ErrorBase.h>
#include <amis/StringStream.h>
#include <amis/FeatureWeightType.h>
#include <memory>
#include <string>


AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/// <classdef>
/// <name>IllegalModelFormatError</name>
/// <overview>Error exception for illegal model format</overview>
/// <desc>
/// The exception class is thrown by the ModelFormat class to signal
/// an invalid data in a model file.
/// </desc>
/// <see>ModelFormat</see>
/// <body>

class IllegalModelFormatError : public ErrorBase {
public:
  IllegalModelFormatError( const std::string& s, int num ) : ErrorBase() {
    OStringStream os;
    os << s << " at line " << num + 1;
    m = os.str();
  }
  /// Initialize with an error message
  IllegalModelFormatError( const char* s, int num ) : ErrorBase() {
    OStringStream os;
    os << s << " at line " << num + 1;
    m = os.str();
  }
  /// Initialize with an error message
};

/// </body>
/// </classdef>

//////////////////////////////////////////////////////////////////////

/// <classdef>
/// <name>ModelFormat</name>
/// <overview>Base class for reading model files</overview>
/// <desc>
/// The class provides a basic interface for importing model
/// data from specified data files.
/// Derived classes should handle format-specific data.
/// </desc>
/// <see>Model, Event</see>
/// <body>

class ModelFormat {
  FeatureWeightType feature_weight_type;
public:
  ModelFormat(FeatureWeightType fwt)
    : feature_weight_type(fwt)
  { }

  /// Constructor
  virtual ~ModelFormat() {}
  /// Destructor

  virtual std::string modelFormatName() const = 0;
  /// Name of the model format
  
  virtual void modelInitialize( NameTable& name_table_base, ModelBase& model_base ) = 0;
  // Perform table-model interactive initialization ?
  
  virtual void inputModel( std::istream& s, NameTable& name_table_base, ModelBase& model ) = 0;
  /// Input model data from an input stream

  virtual void outputModel( const NameTable& name_table_base, const ModelBase& model, std::ostream& s, int precision = DEFAULT_PRECISION , bool do_filter = false) const = 0;
  /// Output model data into an output stream

  static const int DEFAULT_PRECISION = 6;
  /// Default value for a precision of an output

public:
  Real inputLambda( Real x ) const
  {
    switch (feature_weight_type) {
    case LAMBDA: return x;
    case ALPHA: return log(x);
    default: return x;
    }
  }
  Real outputLambda( Real x ) const
  {
    switch (feature_weight_type) {
    case LAMBDA: return x;
    case ALPHA: return exp(x);
    default: return x;
    }
  }
};

/// </body>
/// </classdef>

typedef std::auto_ptr< ModelFormat > ModelFormatPtr;

AMIS_NAMESPACE_END

#endif // DataFile_h_
// end of DataFile.h

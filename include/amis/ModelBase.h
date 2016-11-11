//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: ModelBase.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_ModelBase_h_
#define Amis_ModelBase_h_

#include <amis/configure.h>
#include <amis/Real.h>
#include <amis/Feature.h>
#include <amis/NameTable.h>
#include <amis/StringStream.h>
#include <amis/ParameterValue.h>

#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <memory>
#include <assert.h>

AMIS_NAMESPACE_BEGIN

class ParameterSpaceBase {
public:
  ParameterSpaceBase() { }
  virtual ~ParameterSpaceBase() { }

  virtual void clear() = 0;
  virtual Real getLambda(size_t ) const = 0;
  virtual Real getAlpha(size_t ) const = 0;
  virtual void setLambda(size_t , Real ) = 0;
  virtual void incLambda(size_t , Real ) = 0;
  virtual void multLambda(size_t , Real ) = 0;
  virtual size_t size() const = 0;
  virtual void push_back(Real ) = 0;
  virtual ParameterSpaceBase* clone() const = 0;
};

template<class ParameterValue >
class ParameterSpace : public ParameterSpaceBase {
  std::vector<ParameterValue> parameter;
public:
  ParameterSpace() { }
  ~ParameterSpace() { }

  ParameterSpaceBase* clone() const
  {
    ParameterSpace* ret = new ParameterSpace<ParameterValue>();
    ret->parameter = parameter;
    return ret;
  }
  void clear()
  {
    parameter.clear();
  }
  ParameterValue getParameter(size_t i) const
  {
    return parameter[i];
  }
  const std::vector<ParameterValue>& getParameters() const
  {
    return parameter;
  }
  Real getLambda(size_t i) const
  {
    return parameter[i].getLambda();
  }
  Real getAlpha(size_t i) const
  {
    return parameter[i].getAlpha();
  }
  void setLambda(size_t i, Real l)
  {
    parameter[i].initLambda(l);
  }
  void incLambda(size_t i, Real l)
  {
    parameter[i].incLambda(l);
  }
  void multLambda(size_t i, Real l)
  {
    parameter[i].multLambda(l);
  }
  size_t size() const
  {
    return parameter.size();
  }
  void push_back(Real v)
  {
    ParameterValue p(v);
    parameter.push_back(p);
  }
};

//////////////////////////////////////////////////////////////////////

/// <classdef>
/// <name>ModelBase</name>
/// <overview>Base class of Model</overview>
/// <body>

class StatisticsBase;

class ModelBase {
  private:
  const NameTable* name_table;
  
  Real  default_lambda;

protected:  
  ParameterSpaceBase* parameterspace;
  
  /// Constructor
public:
  ModelBase()
  {
    name_table = NULL;
    default_lambda = 0.0;
  }
  ModelBase(ParameterSpaceBase* ps)
    : name_table(NULL), default_lambda(0.0), parameterspace(ps)
  { }
  /// Destructor
  virtual ~ModelBase()
  {
    delete parameterspace;
  }
  
  virtual void empty() {
    parameterspace->clear();
  }
  
  /// Set name table
  void setNameTable( const NameTable& table ) {
    name_table = &table;
  }

  /// Name of a feature
  std::string featureNameString( FeatureID id ) const {
    assert( name_table != NULL );
    return name_table->featureNameString( id );
  }

  /// Name of this class
  virtual std::string modelName() const = 0;

  /// Output the debug information
  virtual void debugInfo( std::ostream& ostr ) const = 0;
  
  Real defaultLambda() const { 
    return default_lambda;
  }
  
  void setDefaultLambda( Real dl ) {
    default_lambda = dl;
  }
  
  virtual void setAllParametersToDefault() {
    for ( FeatureID i = 0; i < numFeatures(); i++ ) {
      parameterspace->setLambda(i, default_lambda); 
    }
  }
  
  virtual void copyParametersFrom( const ModelBase& m ) {
    if ( numFeatures() != m.numFeatures() ) {
      AMIS_ABORT( "In: " << __PRETTY_FUNCTION__ << ", Model mismatch\n" ); 
    }
    for ( FeatureID i = 0; i < numFeatures(); i++ ) {
      parameterspace->setLambda(i, m.getLambda( i ));
    }
  }
  
  FeatureID numNonzeroLambdas() const {
    FeatureID num = 0;
    for ( FeatureID i = 0; i < numFeatures(); i++ ) {
      if ( fabs( getLambda( i ) ) > REAL_EPSILON ) {
	++num;
      }
    }
    return num;
  }
  
  virtual void importFeature( const ModelBase& from, const std::vector< FeatureID >& fids ) = 0;
  
  virtual void writeStatToStream( std::ostream& os, const StatisticsBase& stat ) const = 0;
  virtual void writeHeavyStatToStream( std::ostream& os, const StatisticsBase& stat ) const = 0;
  
public:

  static Real safe_log( Real x ) {
    Real l = log( x );
    return ( finite( l ) ? l : Limits::LOG_PROB_ZERO );
  }

  //////////////////////////////////////////////////////////////////////

public:
  FeatureID numFeatures() const {
    return parameterspace->size();
  }

  const ParameterSpaceBase* getParameterSpace() const {
    return parameterspace;
  }
  Real getLambda( FeatureID id ) const {
    return parameterspace->getLambda(id);
  }
  Real getAlpha( FeatureID id ) const {
    return parameterspace->getAlpha(id);
  }
  void setLambda( FeatureID id, Real p ) {
    parameterspace->setLambda(id, p);
  }

  void incLambda( FeatureID id, Real a ) {
    parameterspace->incLambda( id, a );
  }
  
  void multLambda( FeatureID id, Real a ) {
    parameterspace->multLambda( id, a );
  }
  
  FeatureID newFeature( Real l ) {
    FeatureID id = parameterspace->size();
    parameterspace->push_back( l );
    return id;
  }
  
};

//////////////////////////////////////////////////////////////////////

inline std::ostream& operator<<( std::ostream& os, const ModelBase& model ) {
  model.debugInfo( os );
  return os;
}

/// </body>
/// </classdef>

typedef std::auto_ptr< ModelBase > ModelPtr;

AMIS_NAMESPACE_END

#endif // Amis_ModelBase_h_
// end of ModelBase.h

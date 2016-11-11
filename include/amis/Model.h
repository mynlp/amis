//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: Model.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_Model_h_
#define Amis_Model_h_

#include <amis/configure.h>
#include <amis/Real.h>
#include <amis/ModelBase.h>
#include <amis/NameTable.h>
#include <amis/DynamicCast.h>
#include <amis/Sort.h>

#include <string>
#include <iostream>
#include <utility>
#include <algorithm>
#include <cmath>
#include <vector>
#include <numeric>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/// <classdef>
/// <name>Model</name>
/// <overview>Model</overview>
/// <body>

class Model : public virtual ModelBase {

public:
  Model(ParameterSpaceBase* ps) : ModelBase(ps) {
    AMIS_DEBUG_MESSAGE( 3, "IN " << __PRETTY_FUNCTION__ <<  "\n" );
  }
  virtual ~Model() {}
  
  virtual void empty() {
    ModelBase::empty(); 
  }
  
  virtual std::string modelName() const { 
    return "Model";
  }
  
  virtual FeatureID newFeature( Real l ) {
    return ModelBase::newFeature( l );
  }
  
  // add model_base's fid-th feature as the new feature
  virtual FeatureID newFeature( const ModelBase& model_base, FeatureID fid )
  {
    const Model* model = DynamicCast< const Model* >( &model_base, __PRETTY_FUNCTION__ );
    return newFeature( model->getLambda( fid ) );
  }
  
  virtual void importFeature( const ModelBase& model_base_from, const std::vector< FeatureID >& fids ) {
    
    const Model* from = DynamicCast< const Model* >( &model_base_from , __PRETTY_FUNCTION__ );
    
    empty();
    
    for ( FeatureID i = 0; i < fids.size(); i++ ) {
      newFeature( *from, fids[ i ] );
    }
  }
  
  virtual void writeStatToStream( std::ostream& os, const StatisticsBase& stat ) const {
    os << "Model:NUM_FEATURES " << numFeatures() << "\n";
    os << "Model:NUM_NONZERO_LAMBDAS " << numNonzeroLambdas() << "\n";
    os << std::flush;
  }
  
  virtual void writeHeavyStatToStream( std::ostream& os, const StatisticsBase& stat ) const {
    os << "Model:FEATURE_SORT_LAMBDA_ABS\n";
    std::vector< std::pair<FeatureID, Real> > sort_vec;
    for ( FeatureID i = 0; i < numFeatures(); i++ ) {
      sort_vec.push_back( std::pair<FeatureID, Real>( i,  fabs( getLambda(i) ) ) );
    }
    
    sort( sort_vec.begin(), sort_vec.end(), IndexValuePairCmpByValue<FeatureID, Real>() );
    
    // into descreasing order
    reverse( sort_vec.begin(), sort_vec.end() );
    
    for ( FeatureID i = 0; i < numFeatures(); i++ ) {
      FeatureID fid = sort_vec[i].first;
      os << fid << " " << featureNameString( fid ) << " " << getLambda( fid ) << "\n";
    }
    os << "\n";
    os << std::flush;
  }
  
  
  public:
  // Debugging
  virtual void debugInfo( std::ostream& ostr ) const {
    ostr << "====================\n";
    ostr << "Feature\tLambda\n";
    for ( FeatureID i = 0; i < numFeatures(); ++i )
      ostr << featureNameString( i ) << '\t' << getLambda( i ) << '\n';
    ostr << "====================\n";
  }
};

AMIS_NAMESPACE_END

#endif // Model_h_
// end of Model.h

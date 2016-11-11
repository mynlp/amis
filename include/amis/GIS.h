//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: GIS.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_GIS_h_

#define Amis_GIS_h_

#include <amis/configure.h>
#include <amis/EventSpace.h>
#include <amis/Real.h>
#include <amis/IS.h>

AMIS_NAMESPACE_BEGIN

class ModelBase;

class GIS : public IS {
protected:
  Real count;

  //////////////////////////////////////////////////////////////////////

public:
  typedef IS BaseClass;
  GIS() : IS() {
    count = 1.0;
  }
  GIS( ModelBase* init_model, EventSpace* init_event )
    : IS( init_model, init_event ) {
    count = 1.0;
  }
  virtual ~GIS() {}

  //////////////////////////////////////////////////////////////////////

public:
  void initialize() {
    AMIS_DEBUG_MESSAGE( 3, "Set internal data...\n" );
    count = static_cast< Real >( BaseClass::maxFeatureCount() );
    BaseClass::initialize();
  }
  /// Initialize the estimator
  Real solveEquation( FeatureID i ) {
    return ( log( BaseClass::empiricalExpectation(i) / BaseClass::modelExpectation(i) ) ) / count;
  }

  const std::string estimatorName() const {
    return "GIS";
  }
};

AMIS_NAMESPACE_END

#endif // Amis_GIS_h_
// end of GIS.h

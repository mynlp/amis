//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: IS.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_IS_h_

#define Amis_IS_h_

#include <amis/configure.h>
#include <amis/LogStream.h>
#include <amis/BatchEstimator.h>

#include <vector>

AMIS_NAMESPACE_BEGIN


//////////////////////////////////////////////////////////////////////

/// <classdef>
/// <name>IS</name>
/// <overview>Implementation of Iterative Scaling Algorithm</overview>
/// <desc>
/// This class implements an iterative scaling algorithm.
/// </desc>
/// <body>

class IS : public BatchEstimator {
public:
  IS() { }
  IS( ModelBase* init_model, EventSpace* init_event )
    : BatchEstimator( init_model, init_event )
  { }
  /// Initialize with the initial model and the events
  virtual ~IS() {}

  //////////////////////////////////////////////////////////////////////

public:
  void initialize() {
    this->initNormalizers();
  }
  /// Initialization of the internal data before estimation

  void finalize() {}
  /// Finalization of the internal data after estimation

  void iteration() {
	  
    this->log_likelihood = this->setModelExpectation();
	
    setLogLikelihood( this->log_likelihood, this->sumEventCount() );
    this->setObjectiveFunctionValue( this->log_likelihood );
	
    /*    AMIS_DEBUG_MESSAGE( 3, "\nEmpirical Expectation:\n" );
    AMIS_DEBUG_MESSAGE( 3, empirical_expectation );
    AMIS_DEBUG_MESSAGE( 3, "\nModel Expectation:\n" );
    AMIS_DEBUG_MESSAGE( 3, model_expectation );
    AMIS_DEBUG_MESSAGE( 3, "\nModel:\n" );
    AMIS_DEBUG_MESSAGE( 3, *model );*/

    // Update a model according to model expectations
    // not parallelized yet
    AMIS_DEBUG_MESSAGE( 3, "\t\t\t\t>>>>>>>>>>>>>>>>>>>>>>>>\n" );
    AMIS_DEBUG_MESSAGE( 3, "\t\t\t\tFeature\tAlpha\tUpdate\n" );
    for ( size_t i = 0; i < this->numFeatures(); ++i ) {
      if ( this->empiricalExpectation(i) != 0.0 ) {
        Real model_update = solveEquation( i );
        AMIS_DEBUG_MESSAGE( 3, "\t\t\t\tFeature " << i << '\t' << featureNameString(i) << '\t' << getLambda( i ) <<
                            '\t' << model_update << '\n' );
        if ( ! finite( model_update ) ) {
          AMIS_PROF_MESSAGE( "Infinite!: " << this->featureNameString( i ) << '\n' );
          //if ( ! isnan( model_update ) ) { tmp (kazama): isnan is fragile since it is not defined in all platforms? 
            if ( model_update > 0.0 ) {
              this->setLambda( i, this->lambdaMax() );
            } else {
              this->setLambda( i, this->lambdaMin() );
            }
          //}
          //else{
            // do not change
          //}
        } else { // ! finite( model_update )
          this->setMinMaxUpdate( model_update );
          this->incLambda( i, model_update );
        }
      }
    }
    AMIS_DEBUG_MESSAGE( 5, "\t\t\t\t<<<<<<<<<<<<<<<<<<<<<<<<\n" );
  }
  /// Each iteration for estimation

  bool isConverged() {
    return fabs( this->min_update ) < this->machineEpsilon() &&
           fabs( this->max_update ) < this->machineEpsilon();
  }
  
  virtual Real solveEquation( FeatureID index ) = 0;
};

/// </body>
/// </classdef>

AMIS_NAMESPACE_END

#endif // IS_h_
// end of IS.h

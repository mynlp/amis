//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: GaussianPrior.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#ifndef Amis_GaussianPrior_h_
#define Amis_GaussianPrior_h_

#include <amis/configure.h>
#include <amis/Real.h>
//#include <amis/StatisticsBase.h>
#include <string>
#include <vector>
#include <memory>

AMIS_NAMESPACE_BEGIN

class GaussianPrior
{
public:
  GaussianPrior() {}
  virtual ~GaussianPrior() {}
  virtual Real operator[](size_t i) const = 0;

  /*  Real sigma( int i ) const {
    return sqrt( variance( i ) );
  }
  Real variance( int i ) const {
    return 1.0 / (*this)[ i ];
  }
  Real invVariance( int i ) const {
    return (*this)[ i ];
  }

  void setSigma( int i, Real s ) {
    (*this)[ i ] = 1.0 / ( s * s );
  }
  void setVariance( int i, Real s ) {
    (*this)[ i ] = 1.0 / s;
  }
  void setInvVariance( int i, Real s ) {
    (*this)[ i ] = s;
  }
  */
public:
  virtual void initialize() = 0;
  
  virtual std::string getName() const = 0;
};

typedef std::auto_ptr< GaussianPrior > GaussianPriorPtr;

//////////////////////////////////////////////////////////////////////

class GaussianPriorSingle : public GaussianPrior {
private:
  Real default_sigma;
  Real sigma;

public:
  Real operator[](size_t ) const
  {
    return sigma;
  }

  GaussianPriorSingle( Real s ) : GaussianPrior() {
    default_sigma = s;
  }
  virtual ~GaussianPriorSingle() {}

  Real defaultSigma() const { return default_sigma; }
  void setDefaultSigma( Real s ) { default_sigma = s; }

  public:
  
  void initialize() {
    Real inv_variance = 1.0 / ( default_sigma * default_sigma );
    sigma = inv_variance;
  }

  std::string getName() const { return "GaussianPriorSingle"; }
};

//////////////////////////////////////////////////////////////////////

class GaussianPriorInvEmp : public GaussianPrior {
private:
  Real default_sigma;
  std::vector<Real> sigma;

public:
  virtual Real operator[](size_t i) const
  {
    return sigma[i];
  }
  
  GaussianPriorInvEmp( Real s ) : GaussianPrior() {
    default_sigma = s;
  }
  virtual ~GaussianPriorInvEmp() {}

  Real defaultSigma() const { return default_sigma; }
  void setDefaultSigma( Real s ) { default_sigma = s; }

public:
  void initialize() {
    //sigma.resize( statistics.numFeatures() );
    //Real inv_variance = 1.0 / ( default_sigma * default_sigma );
    /*for ( size_t i = 0; i < sigma.size(); ++i ) {
      sigma[ i ] = inv_variance * statistics.empiricalExpectation( i ) * statistics.empiricalExpectation( i );
      }*/
  }

  std::string getName() const { return "GaussianPriorInvEmp"; }
};

/*	
	std::string getVarianceString() const {
		OStringStream ss;
		ss << "[sigma=" << sigma << ",";
		if ( variance_type == SINGLE ) {
			ss << "vtype=SINGLE]";
		}
		else if ( variance_type == INV_EMP ) {
			ss << "vtype=INV_EMP]";
		}
		else if ( variance_type == BAYES_BIN ) {
			ss << "vtype=BAYES_BIN]";
		}
		else if ( variance_type == BAYES_BIN_RAW_SIGMA ) {
			ss << "vtype=BAYES_BIN_RAW_SIGMA]";
		}
		else if ( variance_type == BAYES_BIN_HIST ) {
			ss << "vtype=BAYES_BIN_HIST]";
		}
		else if ( variance_type == BAYES_BIN_HIST_RAW_SIGMA ) {
			ss << "vtype=BAYES_BIN_HIST_RAW_SIGMA]";
		}
		return ss.str();
	}
*/

AMIS_NAMESPACE_END

#endif // Amis_GaussianPrior_h_
// end of GaussianPrior.h

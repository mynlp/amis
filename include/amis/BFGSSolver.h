//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: BFGSSolver.h,v 1.3 2008-11-04 03:58:43 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_BFGSSolver_h_

#define Amis_BFGSSolver_h_

#include <amis/configure.h>
#include <amis/Real.h>
#include <amis/ErrorBase.h>

#include <vector>
#include <deque>
#include <valarray>
#include <string>
#include <numeric>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/**
 * Signals an error during a BFGS algorithm
 */

class BFGSError : public ErrorBase {
public:
  /// Constructor
  explicit BFGSError( const std::string& s ) : ErrorBase( s ) {}
  /// Constructor
  explicit BFGSError( const char* s ) : ErrorBase( s ) {}
  /// Destructor
  virtual ~BFGSError() {}
};

//////////////////////////////////////////////////////////////////////

/// Tolerance of decrease of function value in line search
//const Real LINE_SEARCH_TOLERANCE = 0.0001;
const Real LINE_SEARCH_TOLERANCE = 0.0001;
/// Tolerance of decrease of gradient in line search
//const Real GRADIENT_TOLERANCE = 0.9;
const Real GRADIENT_TOLERANCE = 0.9;
//static const Real GRADIENT_TOLERANCE = 0.00001;
/// Maximum number of iterations of line search
const int LINE_SEARCH_ITERATIONS = 100;

/**
 * An implementation of limited-memory BFGS method (general-purpose function
 * optimizer).
 */



template < class Function, bool do_orthantwise_search = false >
class BFGSSolver {
private:
  /// Objective function to minimize
  Function* function;
  /// Size of vector of the function
  size_t vector_size;
  /// Number of previous vectors to memorize
  size_t memory_size;
  /// Gradient of the function
  std::valarray< Real > grad;
  std::valarray< Real > p_grad;
  /// Previous gradient of the function
  std::valarray< Real > old_grad;
  /// Direction to minimize
  std::valarray< Real > direction;
  /// Set of differences of input vectors
  std::deque< std::valarray< Real > > S;
  /// Set of differences of gradients
  std::deque< std::valarray< Real > > Y;
  /// Set of product of s and y
  std::vector< Real > rho;
  /// Temporal storage
  std::valarray< Real > tmp_factor;
  /// Is the beginning of optimization?
  bool is_beginning;
  /// Is the vector converged?
  bool is_converged;
  /// Current functionvalue
  Real func_val;
  /// Machine epsilon
  Real machine_epsilon;

  template<bool b>
  class CheckBool { };

  void testVector(Real scale, CheckBool<true>)
  {
    function->testVector( direction, scale );
  }
  
  void testVector(Real scale, CheckBool<false>)
  {
    function->testVector( direction, scale );
  }


  Real computeFunctionDerivative(CheckBool<true>)
  {
    return function->computeFunctionDerivative(grad, p_grad);
  }

  Real computeFunctionDerivative(CheckBool<false>)
  {
    return function->computeFunctionDerivative(grad);
  }


public:
  /// Constructor
  BFGSSolver( Function* f, size_t v, size_t m, Real e )
    : grad( v ), old_grad( v ), direction( v ),
      S(), Y(), rho(), tmp_factor( m ) {
    if ( do_orthantwise_search ) {
      p_grad.resize(v);
    }
    function = f;
    vector_size = v;
    memory_size = m;
    //S.reserve( m );
    //Y.reserve( m );
    rho.reserve( m );
    is_beginning = true;
    is_converged = true;
    func_val = 0.0;
    machine_epsilon = e;
  }
  /// Destructor
  ~BFGSSolver() {}

protected:
  /// Push new value v to limited-size vector vec
  template < class Val >
  void push( std::vector< Val >& vec, const Val& v ) const {
    if ( vec.size() == vec.capacity() ) {
      for ( typename std::vector< Val >::iterator it = vec.begin();
            it + 1 != vec.end();
            ++it ) {
        std::swap( *it, *( it + 1 ) );
      }
      vec.pop_back();
    }
    vec.push_back( v );
  }
  template < class Val >
  void push( std::deque< Val >& vec, const Val& v ) const {
    if ( vec.size() == memory_size ) {
      vec.pop_front();
    }
    vec.push_back( v );
  }
  /// Compute the norm of a vector
  Real norm( std::valarray< Real >& vec ) const {
    Real sum = 0.0;
    for ( size_t i = 0; i < vec.size(); ++i ) {
      sum += fabs( vec[ i ] );
    }
    return sum;
  }
  /// Compute an inner product
  Real inner_product( std::valarray< Real >& x, std::valarray< Real >& y, Real init = 0.0 ) const {
    //return ::inner_product( &x[ 0 ], &x[ x.size() ], &y[ 0 ], init );
    return std::inner_product( &x[ 0 ], &x[ x.size() ], &y[ 0 ], init );
  }

  std::valarray<Real>& gradient()
  {
    return grad;
  }

  std::valarray<Real>& pseudo_gradient()
  {
    if ( do_orthantwise_search ) {
      return p_grad;
    }
    else {
      return grad;
    }
  }

  /// Routine for line minimization
  void lineSearch() {
    function->backUpVector();
    AMIS_DEBUG_MESSAGE( 3, "BFGSSolver::lineSearch" );
    //std::cerr << "dir = " << direction << std::endl;
    //std::cerr << "grad = " << pseudo_gradient() << std::endl;
    Real slope_base = inner_product( pseudo_gradient(), direction );
    if ( slope_base >= 0.0 ) throw BFGSError( "direction is not descendent" );
    Real FUNC_TOLERANCE = LINE_SEARCH_TOLERANCE * slope_base;
    Real GRAD_TOLERANCE = fabs( GRADIENT_TOLERANCE * slope_base );
    AMIS_DEBUG_MESSAGE( 4, "\tslope_base=" << slope_base << "  FUNC_TOLERANCE=" << FUNC_TOLERANCE << "  GRAD_TOLERANCE=" << GRAD_TOLERANCE << "\n" );
    Real scale = 1.0;
    Real old_scale = 0.0;
    Real old_func = func_val;
    testVector(scale, CheckBool<do_orthantwise_search>());
    Real func = this->computeFunctionDerivative(CheckBool<do_orthantwise_search>());
    Real old_slope = slope_base;
    Real slope = inner_product( pseudo_gradient(), direction );
    // Bracketing the minimum
    AMIS_DEBUG_MESSAGE( 4, "\tbracketing start\n" );
    for ( int i = 0; i < LINE_SEARCH_ITERATIONS; ++i ) {
      //cerr << "  direction=" << direction << std::endl;
      AMIS_DEBUG_MESSAGE( 4, "\t\tscale=" << scale << "\n" );
      AMIS_DEBUG_MESSAGE( 4, "\t\tfunc=" << func << "  orig=" << func_val << "\n" );
      AMIS_DEBUG_MESSAGE( 4, "\t\tslope=" << slope << "  slope_base=" << slope_base << "\n" );
      if ( func > func_val + scale * FUNC_TOLERANCE ) {
        AMIS_DEBUG_MESSAGE( 4, "\tCondition 1 violated\n" );
        break;
      }
      // Condition 1 holds
      if ( fabs( slope ) <= GRAD_TOLERANCE ) {
        AMIS_DEBUG_MESSAGE( 4, "\tCondition 1 & 2 hold\n" );
        if ( fabs( 1.0 - fabs( func / func_val ) ) <= machine_epsilon ) {
          AMIS_DEBUG_MESSAGE( 3, "\tFunction value converged  " << func << "  " << func_val << "\n" );
          is_converged = true;
        }
        func_val = func;
        if ( scale != 1.0 ) direction *= scale;
        return;
      }
      AMIS_DEBUG_MESSAGE( 4, "\tCondition 2 violated\n" );
      if ( slope >= 0.0 ) break;
      old_scale = scale;
      scale *= 2.0;
      old_func = func;
      testVector(scale, CheckBool<do_orthantwise_search>());
      func = this->computeFunctionDerivative(CheckBool<do_orthantwise_search>());
      old_slope = slope;
      slope = inner_product( pseudo_gradient(), direction );
    }
    // Quadratic minimization
    AMIS_DEBUG_MESSAGE( 4, "\tSwitch to quadratic minimization\n" );
    Real min_scale = old_scale;
    Real max_scale = scale;
    Real min_func = old_func;
    Real max_func = func;
    Real min_slope = old_slope;
    Real max_slope = slope;
    Real new_scale = scale;
    for ( int i = 0; i < LINE_SEARCH_ITERATIONS; ++i ) {
      // Compute the next trial point
      old_scale = new_scale;
      Real scale_step = max_scale - min_scale;
      Real constant =
	( max_func - min_func - scale_step * min_slope )
        /
	( scale_step * scale_step );

      // Quadratic minimization
      new_scale = min_scale - 0.5 * min_slope / constant;
      if ( constant <= 0.0 || new_scale <= min_scale || new_scale >= max_scale ) {
	// Bisection
        new_scale = ( min_scale + max_scale ) * 0.5;
      }

      //cerr << "scale-step=" << new_scale - old_scale << std::endl;
      testVector(new_scale, CheckBool<do_orthantwise_search>());
      Real new_func = this->computeFunctionDerivative(CheckBool<do_orthantwise_search>());
      // Check!
      Real new_slope = inner_product( pseudo_gradient(), direction );
      AMIS_DEBUG_MESSAGE( 4, "\t\tscale=" << new_scale << "\n" );
      AMIS_DEBUG_MESSAGE( 4, "\t\tfunc=" << new_func << "  orig=" << func_val << "\n" );
      AMIS_DEBUG_MESSAGE( 4, "\t\tslope=" << new_slope << "  slope_base=" << slope_base << "\n" );

      if ( new_func > func_val + new_scale * FUNC_TOLERANCE ) {
        // Condition 1 does not hold
        AMIS_DEBUG_MESSAGE( 4, "\tCondition 1 violated\n" );
        max_scale = new_scale;
        max_func = new_func;
        max_slope = new_slope;
        continue;
      }

      // Condition 1 holds
      if ( fabs( new_slope ) <= GRAD_TOLERANCE ) {
        AMIS_DEBUG_MESSAGE( 4, "\tCondition 1 & 2 hold\n" );
        if ( fabs( 1.0 - fabs( new_func / func_val ) ) <= machine_epsilon ) {
          AMIS_DEBUG_MESSAGE( 3, "\tFunction value converged  " << new_func << "  " << func_val << "\n" );
          is_converged = true;
        }
        func_val = new_func;
        direction *= new_scale;
        //cerr << "    step " << direction << std::endl;
        return;
      }
      AMIS_DEBUG_MESSAGE( 4, "\tCondition 2 violated\n" );
      if ( new_slope * scale_step >= 0.0 ) {
        // new point is ascendent -> new values are replaced
        AMIS_DEBUG_MESSAGE( 4, "\t\tNew point it ascendent\n" );
        max_scale = min_scale;
        max_func = min_func;
        max_slope = min_slope;
        min_scale = new_scale;
        min_func = new_func;
        min_slope = new_slope;
      }
      else {
        // new point is descendent -> old values are replaced
        AMIS_DEBUG_MESSAGE( 4, "\t\tNew point is descendent\n" );
        min_scale = new_scale;
        min_func = new_func;
        min_slope = new_slope;
      }
    }
    AMIS_ERROR_MESSAGE( "Line search in BFGS algorithm not terminated\n" );
    is_converged = true;
    return;
  }

  /// Check whether the gradient is sufficiently close to a zero vector
  bool checkConvergence() {
    for ( size_t i = 0; i < pseudo_gradient().size(); ++i ) {
      if ( fabs( pseudo_gradient()[ i ] ) > machine_epsilon ) return false;
    }
    AMIS_DEBUG_MESSAGE( 3, "\tGradient converged!\n" );
    is_converged = true;
    return true;
  }

public:
  /// Whether the vector is converged
  bool isConverged() {
    return is_converged;
  }

  /// Initialize the optimizer
  void initialize() {
    AMIS_DEBUG_MESSAGE( 3, "start BFGSSolver::initialize\n" );
    is_beginning = true;
    is_converged = false;
    AMIS_DEBUG_MESSAGE( 3, "end BFGSSolver::initialize\n" );
  }

  /// An iteration of the limited-memory BFGS method
  void iteration() {
    AMIS_DEBUG_MESSAGE( 3, "start BFGSSolver::iteration\n" );
    if ( is_beginning ) {
      func_val = this->computeFunctionDerivative(CheckBool<do_orthantwise_search>());
      //direction = grad * ( -vector_size / norm( grad ) );
      direction = pseudo_gradient() / ( -norm( pseudo_gradient() ) );
      is_beginning = false;
    }
    else {
      if ( checkConvergence() ) return;
      //cerr << "grad=" << grad << std::endl;
      push( S, direction );
      push( Y, gradient() );
      Y.back() -= old_grad;
      Real sy = inner_product( S.back(), Y.back() );
      Real gamma = sy / inner_product( Y.back(), Y.back() );
      push( rho, 1.0 / sy );
      //cerr << "  gamma=" << gamma << std::endl;
      direction = -pseudo_gradient();
      for ( int i = S.size() - 1; i >= 0; --i ) {
        tmp_factor[ i ] = rho[ i ] * ( inner_product( S[ i ], direction ) );
        direction -= Y[ i ] * tmp_factor[ i ];
      }
      direction *= gamma;
      for ( size_t i = 0; i < S.size(); ++i ) {
        Real tmp = rho[ i ] * ( inner_product( Y[ i ], direction ) );
        direction += S[ i ] * ( tmp_factor[ i ] - tmp );
      }
    }
    if ( do_orthantwise_search ) {
      for ( size_t i = 0; i < direction.size(); ++i ) {
	if ( ( direction[i] > 0.0 && pseudo_gradient()[i] > 0.0 ) ||
	     ( direction[i] < 0.0 && pseudo_gradient()[i] < 0.0 ) ||
	     pseudo_gradient()[i] == 0.0 ) {
	  direction[i] = 0.0;
	}
      }
    }
    //cerr << "direction=" << direction << std::endl;
    old_grad = gradient();
    lineSearch();
    AMIS_DEBUG_MESSAGE( 3, "end BFGSSolver::iteration\n" );
  }

  /// Get the function value
  Real functionValue() const {
    return func_val;
  }

  //const std::valarray< Real >& updateVector( void ) const {
  //  return direction;
  //}
  /// Get the direction of update
};

AMIS_NAMESPACE_END
#endif // BFGSSolver_h_
// end of BFGSSolver.h

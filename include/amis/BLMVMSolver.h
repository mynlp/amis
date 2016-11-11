//
// This file is a modified version of the program originaly written by
// Steven Benson, Jorge More', Jim Adduci, Matthew Beauregard.
// The original version was obtained from
// http://www-unix.mcs.anl.gov/BLMVM/
//


/* 
COPYRIGHT NOTIFICATION

(C) COPYRIGHT 2004 UNIVERSITY OF CHICAGO

This program discloses material protectable under copyright laws of the United States.
Permission to copy and modify this software and its documentation is
hereby granted, provided that this notice is retained thereon and on all copies or
modifications. The University of Chicago makes no representations as to the suitability
and operability of this software for any purpose. It is provided "as is"; without
express or implied warranty. Permission is hereby granted to use, reproduce, prepare
derivative works, and to redistribute to others, so long as this original copyright notice
is retained.  

Any publication resulting from research that made use of this software 
should cite the document:  Steven J. Benson and Jorge Mor\'{e}, 
"A Limited-Memory Variable-Metric Algorithm for Bound-Constrained Minimization",
Mathematics and Computer Science Division, Argonne National Laboratory,
ANL/MCS-P909-0901, 2001.




    Argonne National Laboratory with facilities in the states of Illinois and Idaho, is
    owned by The United States Government, and operated by the University of Chicago under
    provision of a contract with the Department of Energy.

                                    DISCLAIMER
    THIS PROGRAM WAS PREPARED AS AN ACCOUNT OF WORK SPONSORED BY AN AGENCY OF THE UNITED
    STATES GOVERNMENT. NEITHER THE UNITED STATES GOVERNMENT NOR ANY AGENCY THEREOF, NOR THE
    UNIVERSITY OF CHICAGO, NOR ANY OF THEIR EMPLOYEES OR OFFICERS, MAKES ANY WARRANTY, EXPRESS
    OR IMPLIED, OR ASSUMES ANY LEGAL LIABILITY OR RESPONSIBILITY FOR THE ACCURACY,
    COMPLETENESS, OR USEFULNESS OF ANY INFORMATION, APPARATUS, PRODUCT, OR PROCESS DISCLOSED,
    OR REPRESENTS THAT ITS USE WOULD NOT INFRINGE PRIVATELY OWNED RIGHTS. REFERENCE HEREIN TO
    ANY SPECIFIC COMMERCIAL PRODUCT, PROCESS, OR SERVICE BY TRADE NAME, TRADEMARK,
    MANUFACTURER, OR OTHERWISE, DOES NOT NECESSARILY CONSTITUTE OR IMPLY ITS ENDORSEMENT,
    RECOMMENDATION, OR FAVORING BY THE UNITED STATES GOVERNMENT OR ANY AGENCY THEREOF. THE
    VIEW AND OPINIONS OF AUTHORS EXPRESSED HEREIN DO NOT NECESSARILY STATE OR REFLECT THOSE OF
    THE UNITED STATES GOVERNMENT OR ANY AGENCY THEREOF.
*/ 


#ifndef __BLMVMSolver_H_
#define __BLMVMSolver_H_

#include <cmath>
#include <cfloat>
#include <vector>

#include <iostream>



namespace {
  //double eps 2.2e-16;
  const double BLMVMSolver_eps = 2.2e-11;
}


class BLMVMSolver {

public:
  typedef std::vector<double> Vector;

  template<typename T>
  static inline T min(T a, T b)
  {
    return ((a <= b)? (a) : (b));
  }

  template<typename T>
  static inline T max(T a, T b)
  {
    return ((a >= b)? (a) : (b));
  }

  int lm;
  int lmnow;

  bool converge;


  std::vector< Vector* > S;
  std::vector< Vector* > Y;
  Vector* Gprev;
  Vector* Xprev;

  double fun_val;
  double prev_val;
  double y0normsquared;
  Vector rho;
  Vector beta;

  Vector X;
  Vector W;
  Vector DX;
  Vector GP;
  Vector G;
  Vector DXL;
  Vector DXU;

  int pgits;

  double maxdx;
  double alpha;
  double dxnorm;

  bool initialized;


  virtual double upperBound(int i) = 0;
  virtual double lowerBound(int i) = 0;

  virtual double computeFunctionGradient(Vector& params,
					 Vector* ret) = 0;
  virtual bool checkConvergence(double step_size, double dxnorm,
				const Vector& GP) = 0;

  BLMVMSolver(int dim, int num_mem)
    : lm(num_mem),
      X(dim), W(dim), DX(dim), GP(dim), G(dim),
      DXL(dim), DXU(dim),
      initialized(false)
  {
    S = std::vector< Vector* >(lm+1);
    Y = std::vector< Vector* >(lm+1);

    rho = Vector(lm+1);
    beta = Vector(lm+1);
  
    lmnow = 0;

    converge = false;

    for (int i = 0; i < lm+1; ++i) {
      S[i] = new Vector(dim);
      Y[i] = new Vector(dim);
    }

    Gprev = 0;
    Xprev = 0;

    maxdx = 1.0e200;
  }

  virtual ~BLMVMSolver()
  {
    for (int i=0; i < lm+1; ++i){
      delete S[i];
      delete Y[i];
    }
  }

  void init()
  {
    for (size_t i = 0; i < X.size(); ++i) {
      X[i] = 0.0;
    }
    VecApplyBounds(&X);
  }

  void bound()
  {
    VecApplyBounds(&X);
  }


  void setX(int i, double d)
  {
    X[i] = d;
  }

  double getX(int i)
  {
    return X[i];
  }


  Vector& getX()
  {
    return X;
  }

  bool converged()
  {
    return converge;
  }

  void LMVMMatUpdate()
  {
    if (Gprev == 0 || Xprev == 0) {
      Gprev = Y[lm];
      Xprev = S[lm];
      rho[0] = 1.0;
      y0normsquared = 1.0;
    }
    else {
      VecYeqAYpX(-1.0, G, Gprev);
      VecYeqAYpX(-1.0, X, Xprev);
      double rhotemp = VecDot(*Xprev, *Gprev);
      double y0temp = VecDot(*Gprev, *Gprev);

      double rhotol = BLMVMSolver_eps * y0temp;
      if (rhotemp > rhotol){
	lmnow = min(lmnow+1, lm);
	for (int i = lm - 1; i >= 0 ; i--){
	  S[i+1] = S[i];
	  Y[i+1] = Y[i];
	  rho[i+1] = rho[i];
	}
	S[0] = Xprev;
	Y[0] = Gprev;
	rho[0] = 1.0 / rhotemp;
	y0normsquared = y0temp;
	Xprev = S[lm];
	Gprev = Y[lm];
      }
    }
    *Xprev = X;
    *Gprev = G;
  }

  void LMVMMatSolve()
  {
    if (lmnow < 1) {
      rho[0]=1.0;
      y0normsquared = 1.0;
    }

    DX = G;
    for (int ll = 0; ll < lmnow; ++ll) {
      double sq = VecDot(DX, *S[ll]);
      beta[ll] = sq * rho[ll];
      VecYp_eqAX(-beta[ll], *Y[ll], &DX);
    }
    VecScale(1.0 / (rho[0] * y0normsquared), &DX);
    for (int ll = lmnow - 1; ll >= 0; ll--){
      double yq = VecDot(DX, *Y[ll]);
      VecYp_eqAX(beta[ll]-yq*rho[ll], *S[ll], &DX);
    }
  }



  int iter()
  {
    if (!initialized) {
      fun_val = computeFunctionGradient(X, &G);
      pgits = 0;
      converge = false;

      pgits = 0;
      dxnorm = 0;
      alpha = 0;
      initialized = true;
      VecProjectGradient(G, X, &DXL, &GP, &DXU);
    }
	
    prev_val = fun_val;

    LMVMMatUpdate();
    LMVMMatSolve();

    VecProjectGradient(DX, X, &DXL, &W, &DXU);

    dxnorm = sqrt(VecDot(W, W));
    double gdx = VecDot(G, W);
    if (gdx <= 0){
      DX = G;
      dxnorm = sqrt(VecDot(GP, GP));
    }

    alpha = -1.0;
    if (dxnorm > maxdx) {
      alpha = -maxdx / dxnorm;
    }

    ApplyLineSearch();
    dxnorm = fabs(dxnorm * alpha);
    alpha = fabs(alpha);

    VecProjectGradient(G, X, &DXL, &GP, &DXU);
    if (checkConvergence(alpha, dxnorm, GP)) {
      converge = true;
    }
    return 0;
  }

  int ApplyLineSearch()
  {
  
    double zero = 0.0;
    double p5 = 0.5;
    double p66 = 0.66;
    double two = 2.0;
    double xtrapf = 4.0;

    double ftest1=0.0;
    double ftest2=0.0;
    int infoc = 1;

    double stepmin  = 1.0e-30; /* lower bound for step */
    double stepmax  = 1.0e+20; /* upper bound for step */
    double rtol     = 1.0e-10; /* relative tolerance for an acceptable step */
    double ftol     = .05;     /* tolerance for sufficient decrease condition */
    double gtol     = .9;      /* tolerance for curvature condition */
    int maxfev   = 30;      /* maximum number of function evaluations */

    double step = -alpha;

    /* Check input parameters for errors */
    if (step < zero) {
      std::cerr << "ERROR: step < 0....Exiting \n";
      converge = true;
      return 0;
    } 

    /* Compute the smallest steplength that will make one nonbinding variable equal the bound */ 
    VecProjectGradient(DX, X, &DXL, &DX, &DXU);

    double bstepmin1, bstepmin2, bstepmax;
    stepBound(X, DX, &bstepmin1, &bstepmin2, &bstepmax);

    if(stepmin > bstepmin1) {
      std::cerr << "BLMVMApplyLineSearch: min step too big (solution non-feasible)\n";
      converge = true;
      return 0;
    }

    stepmax = min(bstepmax, 1.0e+15); 
  
    /* Check that search direction is a descent direction */
    double dginit = -VecDot(G, DX);
    if (dginit >= zero) {
      std::cerr << "BLMVMApplyLineSearch: converged (Not a descent direction)" << std::endl;
      converge = true;
      return 0;
    }

    /* Initialization */
    double bracket = 0;
    int stage1 = 1;
    double finit = fun_val;
    double dgtest = ftol*dginit;
    double width = stepmax-stepmin;
    double width1 = width*two;

    W = X;

    /* Variable dictionary:  
       stx, fx, dgx - the step, function, and derivative at the best step
       sty, fy, dgy - the step, function, and derivative at the other endpoint 
       of the interval of uncertainty
       step, f, dg - the step, function, and derivative at the current step */
  
    double stx = zero;
    double fx = finit;
    double dgx = dginit;
    double sty = zero;
    double fy  = finit;
    double dgy = dginit;
    int nfev = 0;
  
    for (int i = 0; i < maxfev; ++i) {
      /* Set min and max steps to correspond to the interval of uncertainty */
      if (bracket) {
	stepmin = min(stx,sty);
	stepmax = max(stx,sty);
      } 
      else {
	stepmin = stx;
	stepmax = step + xtrapf * (step - stx);
      }

      /* Force the step to be within the bounds */
      step = max(step,stepmin);
      step = min(step,stepmax);
    
      /* If an unusual termination is to occur, then let step be the lowest
	 point obtained thus far */
      if (((bracket) && (step <= stepmin || step >= stepmax)) ||
	  ((bracket) && (stepmax - stepmin <= rtol * stepmax)) ||
	  (nfev >= maxfev - 1) || (infoc == 0)) {
	step = stx;
      }
    
      VecWeqAXpY(-step, DX, W, &X); /* X = W + step*DX */
    
      /* This if statement added by S. Benson.   --  Project the solution, if necessary, to keep feasible */
      if (step >= bstepmin1){
	VecApplyBounds(&X);
      }

      fun_val = computeFunctionGradient(X, &G);
      nfev++;
    
      /*    info = G->Dot(S,&dg);CHKERRQ(info);	        / * dg = G^T S */
      /* Next 3 lines  added by S. Benson.   to compute an estimate of decrease in the actual direction and curvature */
      double dg1 = VecDot(G, W);   /* dg = G^T S */
      double dg2 = VecDot(G, X);  /* dg = G^T S */
      double dg = dg2-dg1;
      ftest1 = finit + step * dgtest;

      /* This test added by S. Benson.   --  Be satisfied with Armijo condition if the Projection X->Median(XL,X,XU) changed X. */
      ftest2 = finit + step * dgtest * ftol;
    
      /* Convergence testing */
      if (((bracket) && (step <= stepmin||step >= stepmax)) || (!infoc)) {
	std::cerr << "BLMVMApply_BoundLineSearch: converged (Possible Rounding errors)" << std::endl;
	converge = true;
	break;
      }
      if ((step == stepmax) && (fun_val <= ftest1) && (dg <= dgtest)) {
	std::cerr << "BLMVMApply_BoundLineSearch: converged (Step is at the upper bound, "
		  << "stepmax " << stepmax << ")" << std::endl;
	converge = true;
	break;
      }
      if ((step == stepmin) && (fun_val >= ftest1) && (dg >= dgtest)) {
	std::cerr << "BLMVMApply_BoundLineSearch: converged (Step is at the lower bound, "
		  << "stepmin " << stepmin << ")" << std::endl;
	converge = true;
	break;
      }
      if (nfev >= maxfev) {
	std::cerr << "BLMVMApply_BoundLineSearch: converged (Number of line search function evals  > maximum)" << std::endl;
	converge = true;
	break;
      }
      if ((bracket) && (stepmax - stepmin <= rtol*stepmax)) {
	std::cerr << "BLMVMApply_BoundLineSearch: converged (Relative width of interval of uncertainty is at most rtol " << rtol << ")" << std::endl;
	converge = true;
	break;
      }
      if ((fun_val <= ftest1) && (fabs(dg) <= gtol*(-dginit))) break;

      /* This test added by S. Benson.   --  Be satisfied with Armijo condition if the Projection X->Median(XL,X,XU) changed X. */
      if ( (fun_val <= ftest2) && ((step) < bstepmin2) ) {
	break;
      }

      /* In the first stage, we seek a step for which the modified function
	 has a nonpositive value and nonnegative derivative */
      if ((stage1) && (fun_val <= ftest1) && (dg >= dginit * min(ftol, gtol))) {
	stage1 = 0;
      }

      /* A modified function is used to predict the step only if we
	 have not obtained a step for which the modified function has a 
	 nonpositive function value and nonnegative derivative, and if a
	 lower function value has been obtained but the decrease is not
	 sufficient */

      if ((stage1) && (fun_val <= fx) && (fun_val > ftest1)) {
	double fm   = fun_val - step * dgtest;	/* Define modified function */
	double fxm  = fx - stx * dgtest;	        /* and derivatives */
	double fym  = fy - sty * dgtest;
	double dgm  = dg - dgtest;
	double dgxm = dgx - dgtest;
	double dgym = dgy - dgtest;

	/* Update the interval of uncertainty and compute the new step */
	Step_LineSearch(&stepmin, &stepmax,
			&bracket, &stx, &fxm, &dgxm, &sty, 
			&fym, &dgym, &step, &fm, &dgm);

	fx  = fxm + stx * dgtest;	/* Reset the function and */
	fy  = fym + sty * dgtest;	/* gradient values */
	dgx = dgxm + dgtest; 
	dgy = dgym + dgtest; 
      }
      else {
	/* Update the interval of uncertainty and compute the new step */
	Step_LineSearch(&stepmin, &stepmax,
			&bracket, &stx,&fx,
			&dgx, &sty, &fy, &dgy, &step, &fun_val, &dg);
      }

      /* Force a sufficient decrease in the interval of uncertainty */
      if (bracket) {
	if (fabs(sty - stx) >= p66 * width1) {
	  step = stx + p5*(sty - stx);
	}
	width1 = width;
	width = fabs(sty - stx);
      }
    }

    /* Finish computations */
    /* printf("BLMVM_BoundLineSearch: step = %4.4e\n",step); */
    /*
      info = G->Norm2(dginitt);CHKERRQ(info);
    */

    /*reset to original standard*/ 
    alpha = (-1)*(step);
  
    return 0;
  }


  int Step_LineSearch(double *stepmin, double *stepmax, 
		      double *bracket,double *stx,double *fx,double *dx,
		      double *sty,double *fy,double *dy,double *stp,
		      double *fp,double *dp)
  {
    double zero = 0.0;

    /* Check the input parameters for errors */
    if (*bracket && (*stp <= min(*stx,*sty) || (*stp >= max(*stx,*sty)))) {
      std::cerr << "BLMVMStep_LineSearch: bad step in bracket"
		<< std::endl;
    }
    if (*dx * (*stp-*stx) >= zero) {
      std::cerr << "BLMVMStep_LineSearch: dx * (stp-stx) >= 0"
		<< std::endl;
    }
    if (*stepmax < *stepmin) {
      std::cerr << "BLMVMStep_LineSearch: stepmax > stepmin"
		<< std::endl;
    }

  /* Determine if the derivatives have opposite sign */
    double sgnd = *dp * (*dx/fabs(*dx));

  /* Case 1: a higher function value.
     the minimum is bracketed. if the cubic step is closer
     to stx than the quadratic step, the cubic step is taken,
     else the average of the cubic and quadratic steps is taken.
   */

    int bound;
    double stpf;
    if (*fp > *fx) {
      bound = 1;
      double theta = 3 * (*fx - *fp) / (*stp - *stx) + *dx + *dp;
      double s = max(fabs(theta), fabs(*dx));
      s = max(s, fabs(*dp));
      double gamma1 = s * sqrt(pow(theta/s, 2) - (*dx/s)*(*dp/s));
      if (*stp < *stx) {
	gamma1 = -gamma1;
      }
    /* Can p be 0?  Check */
      double p = (gamma1 - *dx) + theta;
      double q = ((gamma1 - *dx) + gamma1) + *dp;
      double r = p/q;
      double stpc = *stx + r*(*stp - *stx);
      double stpq = *stx + ((*dx/((*fx-*fp)/(*stp-*stx)+*dx))*0.5) * (*stp - *stx);
      if (fabs(stpc-*stx) < fabs(stpq-*stx)) {
	stpf = stpc;
      } 
      else {
	stpf = stpc + 0.5*(stpq - stpc);
      }
      *bracket = 1;
    }
    /* 
       Case 2: A lower function value and derivatives of
       opposite sign. the minimum is bracketed. if the cubic
       step is closer to stx than the quadratic (secant) step,
       the cubic step is taken, else the quadratic step is taken.
    */
    else if (sgnd < zero) {
      bound = 0;
      double theta = 3*(*fx - *fp)/(*stp - *stx) + *dx + *dp;
      double s = max(fabs(theta), fabs(*dx));
      s = max(s, fabs(*dp));
      double gamma1 = s * sqrt(pow(theta/s,2) - (*dx/s)*(*dp/s));
      if (*stp > *stx) {
	gamma1 = -gamma1;
      }
      double p = (gamma1 - *dp) + theta;
      double q = ((gamma1 - *dp) + gamma1) + *dx;
      double r = p/q;
      double stpc = *stp + r*(*stx - *stp);
      double stpq = *stp + (*dp/(*dp-*dx))*(*stx - *stp);
      if (fabs(stpc-*stp) > fabs(stpq-*stp)) {
	stpf = stpc;
      }
      else {
	stpf = stpq;
      }
      *bracket = 1;
    }

    /*   Case 3: A lower function value, derivatives of the
	 same sign, and the magnitude of the derivative decreases.
	 the cubic step is only used if the cubic tends to infinity
	 in the direction of the step or if the minimum of the cubic
	 is beyond stp. otherwise the cubic step is defined to be
	 either stepmin or stepmax. the quadratic (secant) step is also
	 computed and if the minimum is bracketed then the the step
	 closest to stx is taken, else the step farthest away is taken.
    */

    else if (fabs(*dp) < fabs(*dx)) {
      bound = 1;
      double theta = 3 * (*fx - *fp)/(*stp - *stx) + *dx + *dp;
      double s = max(fabs(theta), fabs(*dx));
      s = max(s, fabs(*dp));

      /* The case gamma1 = 0 only arises if the cubic does not tend
	 to infinity in the direction of the step. */
      double gamma1 = s*sqrt(max(zero,pow(theta/s,2) - (*dx/s)*(*dp/s)));
      if (*stp > *stx) {
	gamma1 = -gamma1;
      }
      double p = (gamma1 - *dp) + theta;
      double q = (gamma1 + (*dx - *dp)) + gamma1;
      double r = p/q;
      double stpc;
      if (r < zero && gamma1 != zero) {
	stpc = *stp + r*(*stx - *stp);
      }
      else if (*stp > *stx) {
	stpc = *stepmax;
      }
      else {
	stpc = *stepmin;
      }
      double stpq = *stp + (*dp/(*dp-*dx)) * (*stx - *stp);
      if (*bracket) {
	if (fabs(*stp-stpc) < fabs(*stp-stpq)) {
	  stpf = stpc;
	}
	else {
	  stpf = stpq;
	}
      }
      else {
	if (fabs(*stp-stpc) > fabs(*stp-stpq)) {
	  stpf = stpc;
	}
	else {
	  stpf = stpq;
	}
      }
    }

    /*   Case 4: A lower function value, derivatives of the
	 same sign, and the magnitude of the derivative does
	 not decrease. if the minimum is not bracketed, the step
	 is either stpmin or stpmax, else the cubic step is taken.
    */
    else {
      bound = 0;
      if (*bracket) {
	double theta = 3*(*fp - *fy)/(*sty - *stp) + *dy + *dp;
	double s = max(fabs(theta), fabs(*dy));
	s = max(s, fabs(*dp));
	double gamma1 = s*sqrt(pow(theta/s,2) - (*dy/s)*(*dp/s));
	if (*stp > *sty) {
	  gamma1 = -gamma1;
	}
	double p = (gamma1 - *dp) + theta;
	double q = ((gamma1 - *dp) + gamma1) + *dy;
	double r = p/q;
	double stpc = *stp + r*(*sty - *stp);
	stpf = stpc;
      } 
      else if (*stp > *stx) {
	stpf = *stepmax;
      } 
      else {
	stpf = *stepmin;
      }
    }

    /* Update the interval of uncertainty.  This update does not
       depend on the new step or the case analysis above. */

    if (*fp > *fx) {
      *sty = *stp;
      *fy = *fp;
      *dy = *dp;
    } 
    else {
      if (sgnd < zero) {
	*sty = *stx;
	*fy = *fx;
	*dy = *dx;
      }
      *stx = *stp;
      *fx = *fp;
      *dx = *dp;
    }
    
    /* Compute the new step and safeguard it */
    stpf = min(*stepmax,stpf);
    stpf = max(*stepmin,stpf);
    *stp = stpf;
    if (*bracket && bound) {
      if (*sty > *stx) {
	*stp = min(*stx+0.66*(*sty-*stx),*stp);
      }
      else {
	*stp = max(*stx+0.66*(*sty-*stx),*stp);
      }
    }
    return 0;

  }


private:
  static void VecScale(double alpha,
		       Vector* x)
  {
    for (size_t i = 0; i < x->size(); ++i) {
      (*x)[i] *= alpha;
    }
  }

  static void VecYp_eqAX(double alpha,
			 const Vector& x,
			 Vector* y)
  {
    for (size_t i = 0; i < x.size(); ++i) {
      (*y)[i] += alpha * x[i];
    }
  }

  static void VecWeqAXpY(double alpha,
			 const Vector& x,
			 const Vector& y,
			 Vector* w)
  {
    for (size_t i = 0; i < x.size(); ++i) {
      (*w)[i] = y[i] + alpha * x[i];
    }
  }


  static void VecYeqAYpX(double alpha,
			 const Vector& x,
			 Vector* y) {
    for (size_t i = 0; i < x.size(); ++i) {
      (*y)[i] = x[i] + alpha * (*y)[i];
    }
  }

  static void VecSet(double alpha,
		     Vector* x)
  {
    for (size_t i = 0; i < x->size(); ++i) {
      (*x)[i] = alpha;
    }
  }

  static double VecDot(const Vector& v1,
		       const Vector& v2)
  {
    double ret = 0.0;
    for (size_t i = 0; i < v1.size(); ++i) {
      ret += v1[i] * v2[i];
    }
    return ret;
  }

  static void VecPointwiseMin(const Vector& v1,
			      const Vector& v2,
			      Vector* v3)
  {
    for (size_t i = 0; i < v1.size(); ++i) {
      (*v3)[i] = min(v1[i], v2[i]);
    }
  }

  static void VecPointwiseMax(const Vector& v1,
			      Vector& v2,
			      Vector* v3)
  {
    for (size_t i = 0; i < v1.size(); ++i) {
      (*v3)[i] = max(v1[i], v2[i]);
    }
  }

  void VecApplyBounds(Vector* v)
  {
    for (unsigned i = 0; i < v->size(); ++i) { 
      (*v)[i] = max((*v)[i], lowerBound(i));
      (*v)[i] = min(upperBound(i), (*v)[i]); 
    }
  }

  void VecProjectGradient(const Vector& g,
			  const Vector& x,
			  Vector* dxl,
			  Vector* gp,
			  Vector* dxu)
  {
    for (size_t i = 0; i < x.size(); ++i) {
      if ((*gp)[i] > 0 && x[i] <= lowerBound(i)) {
	(*gp)[i] = 0;
	(*dxl)[i] = g[i];
	(*dxu)[i] = 0;
      }
      else if ((*gp)[i] < 0 && x[i] >= upperBound(i)) {
	(*gp)[i] = 0;
	(*dxl)[i] = 0;
	(*dxu)[i] = g[i];
      }
      else {
	(*gp)[i] = g[i];
	(*dxl)[i] = 0;
	(*dxu)[i] = 0;
      }
    }
  }

  static void outputVector(std::ostream& os,
			   const Vector& v)
  {
    for (Vector::const_iterator it = v.begin();
	 it != v.end();
	 ++it) {
      os << (*it) << " ";
    }
  }

  void stepBound(const Vector& x,
		 const Vector& dx,
		 double *boundmin,
		 double *wolfemin,
		 double *boundmax)
  {
    double t1 = 1.0e+20;
    double t2 = 1.0e+20;
    double t3=0;
      
    for (unsigned i = 0; i < x.size(); ++i) {
      if (-dx[i] > 0){
	double tt = (upperBound(i)-x[i]) / -dx[i];
	t1 = min(t1, tt);
	if (t1 > 0) {
	  t2 = min(t2, tt);
	}
	t3 = max(t3, tt);
      }
      else if (-dx[i] < 0) {
	double tt = (lowerBound(i)-x[i]) / -dx[i];
	t1 = min(t1, tt);
	if (t1 > 0) {
	  t2 = min(t2, tt);
	}
	t3 = max(t3, tt);
      }
    }
    *boundmin = t1;
    *wolfemin = t2;
    *boundmax = t3;
  }

};

#endif

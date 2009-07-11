// Copyright (C) 2008 Benjamin Kehlet.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2008-10-11
// Last changed: 2008-02-03

#include <dolfin/la/uBLASDenseMatrix.h>
#include "SORSolver.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
void SORSolver::SOR(uint n, 
		    const real* A, 
		    real* x, 
		    const real* b, 
		    const real& tol) 
{
  real prev[n];

  uint iterations = 0;
  real diff = 1000.0; //some big number

  while ( diff > tol ) 
  {
    if ( iterations > SOR_MAX_ITERATIONS )
    {
      error("SOR: System seems not to converge");
    }

    real_set(n, prev, x);
    
    SOR_iteration(n, A, b, x, prev);

    
    // Check precision
    real_sub(n, prev, x);
    diff = real_max_abs(n, prev);

    ++iterations;
  }
}
//-----------------------------------------------------------------------------
void SORSolver::SOR_iteration(uint n, 
                               const real* A, 
                               const real* b, 
                               real* x_new, 
                               const real* x_prev)
{
  for (uint i = 0; i < n; ++i)
  {
    x_new[i] = b[i];
    
    // j < i
    for (uint j = 0; j < i; ++j) 
    { x_new[i] -= (A[i*n+j] * x_new[j]); }
    
    // j > i
    for (uint j = i+1; j < n; ++j) 
    { x_new[i] -= (A[i*n+j]* x_prev[j]); }

    x_new[i] /= A[i*n + i];
  }
}
//-----------------------------------------------------------------------------
void SORSolver::SOR_precond(uint n, 
			    const real* A, 
			    real *x,
			    const real* b,
			    const uBLASDenseMatrix& Precond,
			    const real& tol)
{
  real A_precond[n*n];
  real b_precond[n];

  //Compute Precond*A and Precond*b with extended precision
  for (uint i=0; i < n; ++i)
  {
    b_precond[i] = 0.0;
    for (uint j = 0; j < n; ++j)
    {
      b_precond[i] += Precond(i,j)*b[j];

      A_precond[i*n+j] = 0.0;
      for (uint k = 0; k < n; ++k)
      {
	A_precond[i*n+j] += Precond(i, k)* A[k*n+j];
      }
    }
  }

  // use Precond*b as initial guess
  real_set(n, x, b_precond);

  // solve the system
  SOR(n, A_precond, x, b_precond, tol);
}
//-----------------------------------------------------------------------------
real SORSolver::err(uint n, const real* A, const real* x, const real* b) {
  real _err[n];

  //compute Ax and compare with b
  for (uint i=0; i < n; ++i)
  {
    _err[i] = 0.0;
    for (uint j = 0; j < n; ++j)
    {
      _err[i] += A[i*n+j]*x[j];
    }
    _err[i] -= b[i];
  }
  real e =  real_max_abs(n, _err);

  return e;
}

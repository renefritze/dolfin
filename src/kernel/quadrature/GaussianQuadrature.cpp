// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin/DenseMatrix.h>
#include <dolfin/Vector.h>
#include <dolfin/Legendre.h>
#include <dolfin/GaussianRules.h>

using namespace dolfin

//-----------------------------------------------------------------------------
GaussianRules:GaussianRule

//-----------------------------------------------------------------------------
void GaussQuadrature::computeWeights()
{
  // Compute the quadrature weights by solving a linear system of equations
  // for exact integration of polynomials. We compute the integrals over
  // [-1,1] of the Legendre polynomials of degree <= n - 1; These integrals
  // are all zero, except for the integral of P0 which is 2.
  //
  // This requires that the n-point quadrature rule is exact at least for
  // polynomials of degree n-1.

  // Special case n = 0
  if ( n == 0 ) {
    weights[0] = 2.0;
    return;
  }
 
  DenseMatrix A(n, n);
  Vector x(n), b(n);
   
  // Compute the matrix coefficients
  for (int i = 0; i < n; i++) {
    Legendre p(i);
    for (int j = 0; j < n; j++)
      A(i,j) = p(points[j]);
    b(i) = 0.0;
  }
  b(0) = 2.0;
    
  // Solve the system of equations
  A.solve(x, b);
  
  // Save the weights
  for (int i = 0; i < n; i++)
    weights[i] = x(i);
}
//-----------------------------------------------------------------------------

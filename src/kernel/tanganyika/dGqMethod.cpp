// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin/Lagrange.h>
#include <dolfin/RadauQuadrature.h>
#include <dolfin/Vector.h>
#include <dolfin/Matrix.h>
#include <dolfin/dGqMethod.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
dGqMethod::dGqMethod(int q) : Method(q)
{
  init();
}
//-----------------------------------------------------------------------------
void dGqMethod::show() const
{
  dolfin_info("Data for the dG(%d) method", q);
  dolfin_info("==========================");
  dolfin_info("");

  dolfin_info("Radau quadrature points and weights on [0,1]:");
  dolfin_info("");
  dolfin_info(" i   points                   weights");
  dolfin_info("----------------------------------------------------");
  
  for (int i = 0; i < n; i++)
    dolfin_info("%2d   %.16e   %.16e", i, points[i], qweights[i]);

  for (int i = 0; i < n; i++) {
    dolfin_info("");
    dolfin_info("dG(%d) weights for degree of freedom %d:", q, i);
    dolfin_info("");
    dolfin_info(" i   weights");
    dolfin_info("---------------------------");
    for (int j = 0; j < n; j++)
      dolfin_info("%2d   %.16e", j, weights[i][j]);
  }
}
//-----------------------------------------------------------------------------
void dGqMethod::computeQuadrature()
{
  // Use Radau quadrature
  RadauQuadrature quadrature(n);

  // Get points, rescale from [-1,1] to [0,1], and reverse the points
  for (int i = 0; i < n; i++)
    points[i] = 1.0 - (quadrature.point(n-1-i) + 1.0) / 2.0;

  // Get points, rescale from [-1,1] to [0,1], and reverse the points
  for (int i = 0; i < n; i++)
    qweights[i] = 0.5 * quadrature.weight(n-1-i);
}
//-----------------------------------------------------------------------------
void dGqMethod::computeBasis()
{
  dolfin_assert(!trial);
  dolfin_assert(!test);

  // Compute Lagrange basis for trial space
  trial = new Lagrange(q);
  for (int i = 0; i < n; i++)
    trial->set(i, points[i]);

  // Compute Lagrange basis for test space
  test = new Lagrange(q);
  for (int i = 0; i < n; i++)
    test->set(i, points[i]);
}
//-----------------------------------------------------------------------------
void dGqMethod::computeWeights()
{
  Matrix A(n, n, Matrix::DENSE);
  
  // Compute matrix coefficients
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
  
      // Use Radau quadrature which is exact for the order we need, 2q
      real integral = 0.0;
      for (int k = 0; k < n; k++) {
	real x = points[k];
	integral += qweights[k] * trial->dx(j,x) * test->eval(i,x);
      }
      
      A[i][j] = integral + trial->eval(j,0.0) * test->eval(i,0.0);
      
    }
  }

  Vector b(n);
  Vector w(n);

  // Compute nodal weights for each degree of freedom (loop over points)
  for (int i = 0; i < n; i++) {
    
    // Get nodal point
    real x = points[i];
    
    // Evaluate test functions at current nodal point
    for (int j = 0; j < n; j++)
      b(j) = test->eval(j,x);
    
    // Solve for the weight functions at the nodal point
    A.hpsolve(w,b);

    // Save weights including quadrature
    for (int j = 0; j < n; j++)
      weights[j][i] = qweights[i] * w(j);
    
  }
}
//-----------------------------------------------------------------------------

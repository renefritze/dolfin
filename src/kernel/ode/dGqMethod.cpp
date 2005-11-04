// Copyright (C) 2003-2005 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2005-05-02
// Last changed: 2005-11-02

#include <dolfin/dolfin_log.h>
#include <dolfin/dolfin_math.h>
#include <dolfin/Lagrange.h>
#include <dolfin/RadauQuadrature.h>
#include <dolfin/Vector.h>
#include <dolfin/Matrix.h>
#include <dolfin/LU.h>
#include <dolfin/dGqMethod.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
dGqMethod::dGqMethod(unsigned int q) : Method(q, q + 1, q + 1)
{
  dolfin_info("Initializing discontinous Galerkin method dG(%d).", q);
  
  init();

  _type = Method::dG;

  p = 2*q + 1;
}
//-----------------------------------------------------------------------------
real dGqMethod::ueval(real x0, real values[], real tau) const
{
  // Note: x0 is not used, maybe this can be done differently

  real sum = 0.0;
  for (unsigned int i = 0; i < nn; i++)
    sum += values[i] * trial->eval(i, tau);
  
  return sum;
}
//-----------------------------------------------------------------------------
real dGqMethod::residual(real x0, real values[], real f, real k) const
{
  // FIXME: Include jump term in residual
  real sum = 0.0;
  for (uint i = 0; i < nn; i++)
    sum += values[i] * derivatives[i];

  return sum / k - f;
}
//-----------------------------------------------------------------------------
real dGqMethod::timestep(real r, real tol, real k0, real kmax) const
{
  // FIXME: Missing stability factor and interpolation constant
  // FIXME: Missing jump term
  
  if ( fabs(r) < DOLFIN_EPS )
    return kmax;

  //return pow(tol / fabs(r), 1.0 / static_cast<real>(q+1));

  const real qq = static_cast<real>(q);
  return pow(tol * pow(k0, qq) / fabs(r), 1.0 / (2.0*qq + 1.0));
}
//-----------------------------------------------------------------------------
real dGqMethod::error(real k, real r) const
{
  // FIXME: Missing jump term and interpolation constant
  return pow(k, static_cast<real>(q + 1)) * fabs(r);
}
//-----------------------------------------------------------------------------
void dGqMethod::disp() const
{
  dolfin_info("Data for the dG(%d) method", q);
  dolfin_info("==========================");
  dolfin_info("");

  dolfin_info("Radau quadrature points and weights on [0,1]:");
  dolfin_info("");
  dolfin_info(" i   points                   weights");
  dolfin_info("----------------------------------------------------");
  
  for (unsigned int i = 0; i < nq; i++)
    dolfin_info("%2d   %.15e   %.15e", i, qpoints[i], qweights[i]);
  dolfin_info("");

  for (unsigned int i = 0; i < nn; i++)
  {
    dolfin_info("");
    dolfin_info("dG(%d) weights for degree of freedom %d:", q, i);
    dolfin_info("");
    dolfin_info(" i   weights");
    dolfin_info("---------------------------");
    for (unsigned int j = 0; j < nq; j++)
      dolfin_info("%2d   %.15e", j, nweights[i][j]);
  }
  dolfin_info("");

  dolfin_info("dG(%d) weights in matrix format:", q);
  if ( q < 10 )
    dolfin_info("-------------------------------");
  else
    dolfin_info("--------------------------------");
  for (unsigned int i = 0; i < nn; i++)
  {
    for (unsigned int j = 0; j < nq; j++)
      cout << nweights[i][j] << " ";
    cout << endl;
  }
}
//-----------------------------------------------------------------------------
void dGqMethod::computeQuadrature()
{
  // Use Radau quadrature
  RadauQuadrature quadrature(nq);

  // Get points, rescale from [-1,1] to [0,1], and reverse the points
  for (unsigned int i = 0; i < nq; i++)
  {
    qpoints[i] = 1.0 - (quadrature.point(nq - 1 - i).x + 1.0) / 2.0;
    npoints[i] = qpoints[i];
  }

  // Get points, rescale from [-1,1] to [0,1], and reverse the points
  for (unsigned int i = 0; i < nq; i++)
    qweights[i] = 0.5 * quadrature.weight(nq - 1 - i);
}
//-----------------------------------------------------------------------------
void dGqMethod::computeBasis()
{
  dolfin_assert(!trial);
  dolfin_assert(!test);

  // Compute Lagrange basis for trial space
  trial = new Lagrange(q);
  for (unsigned int i = 0; i < nq; i++)
    trial->set(i, qpoints[i]);

  // Compute Lagrange basis for test space
  test = new Lagrange(q);
  for (unsigned int i = 0; i < nq; i++)
    test->set(i, qpoints[i]);
}
//-----------------------------------------------------------------------------
void dGqMethod::computeWeights()
{
  Matrix A(nn, nn);
  
  // Compute matrix coefficients
  for (unsigned int i = 0; i < nn; i++)
  {
    for (unsigned int j = 0; j < nn; j++)
    {
      // Use Radau quadrature which is exact for the order we need, 2q
      real integral = 0.0;
      for (unsigned int k = 0; k < nq; k++)
      {
	real x = qpoints[k];
	integral += qweights[k] * trial->ddx(j, x) * test->eval(i, x);
      }
      
      A(i, j) = integral + trial->eval(j, 0.0) * test->eval(i, 0.0);
    }
  }

  Vector b(nn);
  Vector w(nn);

  // Compute nodal weights for each degree of freedom (loop over points)
  for (unsigned int i = 0; i < nq; i++)
  {
    // Get nodal point
    real x = qpoints[i];
    
    // Evaluate test functions at current nodal point
    for (unsigned int j = 0; j < nn; j++)
      b(j) = test->eval(j, x);

    // Solve for the weight functions at the nodal point
    // FIXME: Do we get high enough precision?
    LU lu;
    lu.solve(A, w, b);

    // Save weights including quadrature
    for (unsigned int j = 0; j < nn; j++)
      nweights[j][i] = qweights[i] * w(j);
  }
}
//-----------------------------------------------------------------------------

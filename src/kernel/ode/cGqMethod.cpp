// Copyright (C) 2003-2006 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2005-05-02
// Last changed: 2006-05-07

#include <dolfin/dolfin_log.h>
#include <dolfin/dolfin_math.h>
#include <dolfin/Lagrange.h>
#include <dolfin/LobattoQuadrature.h>
#include <dolfin/DenseVector.h>
#include <dolfin/DenseMatrix.h>
#include <dolfin/Matrix.h>
#include <dolfin/Vector.h>
#include <dolfin/LU.h>
#include <dolfin/cGqMethod.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
cGqMethod::cGqMethod(unsigned int q) : Method(q, q + 1, q)
{
  dolfin_info("Initializing continous Galerkin method cG(%d).", q);

  init();

  _type = Method::cG;

  p = 2*q;
}
//-----------------------------------------------------------------------------
real cGqMethod::ueval(real x0, real values[], real tau) const
{
  real sum = x0 * trial->eval(0, tau);
  for (uint i = 0; i < nn; i++)
    sum += values[i] * trial->eval(i + 1, tau);
  
  return sum;
}
//-----------------------------------------------------------------------------
real cGqMethod::residual(real x0, real values[], real f, real k) const
{
  real sum = x0 * derivatives[0];
  for (uint i = 0; i < nn; i++)
    sum += values[i] * derivatives[i + 1];

  return sum / k - f;
}
//-----------------------------------------------------------------------------
real cGqMethod::timestep(real r, real tol, real k0, real kmax) const
{
  // FIXME: Missing stability factor and interpolation constant

  if ( fabs(r) < DOLFIN_EPS )
    return kmax;

  const real qq = static_cast<real>(q);
  return pow(tol * pow(k0, qq) / fabs(r), 0.5 / qq);
}
//-----------------------------------------------------------------------------
real cGqMethod::error(real k, real r) const
{
  // FIXME: Missing interpolation constant
  return pow(k, static_cast<real>(q)) * fabs(r);
}
//-----------------------------------------------------------------------------
void cGqMethod::disp() const
{
  dolfin_info("Data for the cG(%d) method", q);
  dolfin_info("=========================");
  dolfin_info("");

  dolfin_info("Lobatto quadrature points and weights on [0,1]:");
  dolfin_info("");
  dolfin_info(" i   points                   weights");
  dolfin_info("----------------------------------------------------");
  
  for (unsigned int i = 0; i < nq; i++)
    dolfin_info("%2d   %.15e   %.15e", i, 0.0, 0.0);
  //dolfin_info("%2d   %.15e   %.15e", i, qpoints[i], qweights[i]);
  dolfin_info("");

  for (unsigned int i = 0; i < nn; i++)
  {
    dolfin_info("");
    dolfin_info("cG(%d) weights for degree of freedom %d:", q, i);
    dolfin_info("");
    dolfin_info(" i   weights");
    dolfin_info("---------------------------");
    for (unsigned int j = 0; j < nq; j++)
      dolfin_info("%2d   %.15e", j, 0.0);
    //dolfin_info("%2d   %.15e", j, nweights[i][j]);
  }
  dolfin_info("");
  
  dolfin_info("cG(%d) weights in matrix format:", q);
  if ( q < 10 )
    dolfin_info("-------------------------------");
  else
    dolfin_info("--------------------------------");
  for (unsigned int i = 0; i < nn; i++)
  {
    for (unsigned int j = 0; j < nq; j++)
      cout << 0.0 << " ";
    //cout << nweights[i][j] << " ";
    cout << endl;
  }
}
//-----------------------------------------------------------------------------
void cGqMethod::computeQuadrature()
{
  // Use Lobatto quadrature
  LobattoQuadrature quadrature(nq);

  // Get quadrature points and rescale from [-1,1] to [0,1]
  for (unsigned int i = 0; i < nq; i++)
    qpoints[i] = (quadrature.point(i).x + 1.0) / 2.0;

  // Get nodal points and rescale from [-1,1] to [0,1]
  for (unsigned int i = 0; i < nn; i++)
    npoints[i] = (quadrature.point(i + 1).x + 1.0) / 2.0;

  // Get quadrature weights and rescale from [-1,1] to [0,1]
  for (unsigned int i = 0; i < nq; i++)
    qweights[i] = 0.5 * quadrature.weight(i);
}
//-----------------------------------------------------------------------------
void cGqMethod::computeBasis()
{
  dolfin_assert(!trial);
  dolfin_assert(!test);

  // Compute Lagrange basis for trial space
  trial = new Lagrange(q);
  for (unsigned int i = 0; i < nq; i++)
    trial->set(i, qpoints[i]);

  // Compute Lagrange basis for test space using the Lobatto points for q - 1
  test = new Lagrange(q - 1);
  if ( q > 1 )
  {
    LobattoQuadrature lobatto(nq - 1);
    for (unsigned int i = 0; i < (nq - 1); i++)
      test->set(i, (lobatto.point(i).x + 1.0) / 2.0);
  }
  else
    test->set(0, 1.0);
}
//-----------------------------------------------------------------------------
void cGqMethod::computeWeights()
{
  DenseMatrix A(q, q);
  
  // Compute matrix coefficients
  for (unsigned int i = 0; i < nn; i++)
  {
    for (unsigned int j = 0; j < nn; j++)
    {
      // Use Lobatto quadrature which is exact for the order we need, 2q-1
      real integral = 0.0;
      for (unsigned int k = 0; k < nq; k++)
      {
	real x = qpoints[k];
	integral += qweights[k] * trial->ddx(j + 1, x) * test->eval(i, x);
      }
      
      A(i, j) = integral;
    }
  }

  DenseVector b(q);
  DenseVector w(q);

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
    //LU lu;
    //lu.set("LU report", false);
    //lu.solve(A, w, b);
    A.invert();
    A.mult(b, w);

    // Save weights including quadrature
    for (unsigned int j = 0; j < nn; j++)
      nweights[j][i] = qweights[i] * w(j);
  }
}
//-----------------------------------------------------------------------------

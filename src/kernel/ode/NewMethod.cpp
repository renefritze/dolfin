// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin/dolfin_log.h>
#include <dolfin/Lagrange.h>
#include <dolfin/NewMethod.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
NewMethod::NewMethod(unsigned int q, unsigned int nq, unsigned int nn)
{
  dolfin_assert(q >= 0);
  dolfin_assert(nq > 0);
  dolfin_assert(nn > 0);

  this->q = q;
  this->nq = nq;
  this->nn = nn;

  // Allocate quadrature points
  qpoints  = new real[nq];
  for (unsigned int i = 0; i < nq; i++)
    qpoints[i] = 0.0;

  // Allocate nodal points
  npoints  = new real[nn];
  for (unsigned int i = 0; i < nn; i++)
    npoints[i] = 0.0;

  // Allocate quadrature weights
  qweights = new real[nq];
  for (unsigned int i = 0; i < nq; i++)
    qweights[i] = 0.0;

  // Allocate weights
  nweights = new real*[nn];
  for (unsigned int i = 0; i < nn; i++)
  {
    nweights[i] = new real[nq];
    for (unsigned int j = 0; j < nq; j++)
      nweights[i][j] = 0.0;
  }

  // Allocate derivatives
  derivatives = new real[nq];
  for (unsigned int i = 0; i < nq; i++)
    derivatives[i] = 0.0;

  trial = 0;
  test = 0;
}
//-----------------------------------------------------------------------------
NewMethod::~NewMethod()
{
  if ( qpoints ) delete [] qpoints;
  if ( npoints ) delete [] npoints;
  if ( qweights ) delete [] qweights;

  if ( nweights )
  {
    for (unsigned int i = 0; i < nn; i++)
      delete [] nweights[i];
    delete [] nweights;
  }

  if ( derivatives ) delete [] derivatives;

  if ( trial ) delete trial;
  if ( test ) delete test;
}
//-----------------------------------------------------------------------------
void NewMethod::init()
{
  computeQuadrature();
  computeBasis();
  computeWeights();
  computeDerivatives();
}
//-----------------------------------------------------------------------------
real NewMethod::update(real x0, real f[], real k, real values[]) const
{
  // Save old end-time value
  real x1 = values[nn - 1];

  // Update values
  for (uint i = 0; i < nn; i++)
  {
    real sum = 0.0;
    for (uint j = 0; j < nq; j++)
      sum += nweights[i][j] * f[j];
    values[i] = x0 + k*sum;
  }

  // Return increment
  return values[nn - 1] - x1;
}
//-----------------------------------------------------------------------------
void NewMethod::computeDerivatives()
{
  for (unsigned int i = 0; i < nq; i++)
    derivatives[i] = trial->ddx(i, 1.0);
}
//-----------------------------------------------------------------------------

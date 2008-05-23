// Copyright (C) 2007-2008 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Ola Skavhaug 2008.
//
// First added:  2007-04-30
// Last changed: 2008-05-23

#include "LinearSolver.h"
#include "GenericMatrix.h"
#include "GenericVector.h"
#include "LinearAlgebraFactory.h"
#include "solve.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
void dolfin::solve(const GenericMatrix& A, GenericVector& x, const GenericVector& b,
                   SolverType solver_type, PreconditionerType pc_type)
{
  LinearSolver solver(solver_type, pc_type);
  solver.solve(A, x, b);
}
//-----------------------------------------------------------------------------  
real dolfin::residual(const GenericMatrix& A, const GenericVector& x, const GenericVector& b)
{
  GenericVector* y = A.factory().createVector();
  A.mult(x, *y);
  *y -= b;
  const real norm = y->norm();
  delete y;
  return norm;
}
//-----------------------------------------------------------------------------
real dolfin::normalize(GenericVector& x, NormalizationType normalization_type)
{
  switch (normalization_type)
  {
  case norm:
    {
      const real c = x.norm();
      x /= c;
      return c;
    }
    break;
  case average:
    {
      GenericVector* y = x.factory().createVector();
      y->init(x.size());
      (*y) = 1.0 / static_cast<real>(x.size());
      const real c = x.inner(*y);
      (*y) = c;
      x -= (*y);
      delete y;
      return c;
    }
    break;
  default:
    error("Unknown normalization type.");
  }

  return 0.0;
}
//-----------------------------------------------------------------------------
/*
void dolfin::solve(const PETScKrylovMatrix& A,
                   PETScVector& x,
                   const PETScVector& b)
{
  PETScLUSolver solver;
  solver.solve(A, x, b);
}
*/
//-----------------------------------------------------------------------------

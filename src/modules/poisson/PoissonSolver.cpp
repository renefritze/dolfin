// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// Modified by Johan Hoffman, 2005.
// Modified by Anders Logg, 2005.

// FIXME: Should not be needed
#include <dolfin/NewFunction.h>
#include <dolfin/BoundaryCondition.h>

// FIXME: Should not be needed
#include <dolfin/NewGMRES.h>

#include <dolfin/Mesh.h>
#include <dolfin/NewVector.h>
#include <dolfin/NewMatrix.h>
#include <dolfin/NewFEM.h>
#include <dolfin/NewBoundaryCondition.h>
#include <dolfin/File.h>
#include <dolfin/Parameter.h>
#include <dolfin.h>

#include "dolfin/Poisson.h"
#include "dolfin/PoissonSolver.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
PoissonSolver::PoissonSolver(Mesh& mesh, NewFunction& f, NewBoundaryCondition& bc)
  : mesh(mesh), f(f), bc(bc)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void PoissonSolver::solve()
{
  Poisson::FiniteElement element;

  Poisson::BilinearForm a;
  Poisson::LinearForm L(f);

  NewMatrix A;
  NewVector x, b;

  // Discretize
  NewFEM::assemble(a, L, A, b, mesh, element);

  // Set boundary conditions
  NewFEM::setBC(A, b, mesh, bc);
  
  // Solve the linear system
  // FIXME: Make NewGMRES::solve() static
  NewGMRES solver;
  solver.solve(A, x, b);

  // FIXME: Remove this and implement output for NewFunction
  Vector xold(b.size());
  for(uint i = 0; i < x.size(); i++)
    xold(i) = x(i);
  Function uold(mesh, xold);
  uold.rename("u", "temperature");
  File file("poisson.m");
  file << uold;
}
//-----------------------------------------------------------------------------
void PoissonSolver::solve(Mesh& mesh, NewFunction& f, NewBoundaryCondition& bc)
{
  PoissonSolver solver(mesh, f, bc);
  solver.solve();
}
//-----------------------------------------------------------------------------

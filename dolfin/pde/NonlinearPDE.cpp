// Copyright (C) 2005-2008 Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Anders Logg, 2006-2008.
//
// First added:  2005-10-24
// Last changed: 2008-11-15

#include <dolfin/fem/BoundaryCondition.h>
#include <dolfin/function/Function.h>
#include <dolfin/fem/Form.h>
#include <dolfin/log/dolfin_log.h>
#include "NonlinearPDE.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
NonlinearPDE::NonlinearPDE(Form& a, Form& L, BoundaryCondition& bc) : a(a), L(L)
{
  message("Creating nonlinear PDE with %d boundary condition(s).", bcs.size());

  // Check ranks of forms
  if ( a.rank() != 2 )
    error("Expected a bilinear form but rank is %d.", a.rank());
  if ( L.rank() != 1 )
    error("Expected a linear form but rank is %d.", L.rank());

  // Create array with one boundary condition
  bcs.push_back(&bc);
}
//-----------------------------------------------------------------------------
NonlinearPDE::NonlinearPDE(Form& a, Form& L, Array<BoundaryCondition*>& bcs) 
    : a(a), L(L), bcs(bcs)
{
  message("Creating nonlinear PDE with %d boundary condition(s).", bcs.size());

  // Check ranks of forms
  if ( a.rank() != 2 )
    error("Expected a bilinear form but rank is %d.", a.rank());
  if ( L.rank() != 1 )
    error("Expected a linear form but rank is %d.", L.rank());
}
//-----------------------------------------------------------------------------
NonlinearPDE::~NonlinearPDE()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void NonlinearPDE::update(const GenericVector& x)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void NonlinearPDE::F(GenericVector& b, const GenericVector& x)
{
  // Assemble 
  Assembler::assemble(b, L);

  // Apply boundary conditions
  for (uint i = 0; i < bcs.size(); i++)
    bcs[i]->apply(b, x);
}
//-----------------------------------------------------------------------------
void NonlinearPDE::J(GenericMatrix& A, const GenericVector& x)
{
  // Assemble 
  Assembler::assemble(A, a);

  // Apply boundary conditions
  for (uint i = 0; i < bcs.size(); i++)
    bcs[i]->apply(A);
}
//-----------------------------------------------------------------------------
void NonlinearPDE::solve(Function& u, double& t, const double& T, const double& dt)
{
  begin("Solving nonlinear PDE.");  

  // Get vector associated with Function
  GenericVector& x = u.vector();

  // Solve
  while( t < T )
  {
    t += dt;
    newton_solver.solve(*this ,x);
  }

  end();
}
//-----------------------------------------------------------------------------

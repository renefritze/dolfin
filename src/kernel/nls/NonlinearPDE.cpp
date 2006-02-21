// Copyright (C) 2005 Garth N. Wells.
// Licensed under the GNU GPL Version 2.
//
// First added:  2005-10-24
// Last changed: 2005-12-05

#include <dolfin/FEM.h>
#include <dolfin/NonlinearPDE.h>
#include <dolfin/BilinearForm.h>
#include <dolfin/LinearForm.h>
#include <dolfin/NewtonSolver.h>

using namespace dolfin;
NonlinearPDE::NonlinearPDE()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
NonlinearPDE::NonlinearPDE(BilinearForm& a, LinearForm& L, Mesh& mesh) : 
      NonlinearFunction(), _a(&a), _Lf(&L), _mesh(&mesh), _bc(0)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
NonlinearPDE::NonlinearPDE(BilinearForm& a, LinearForm& L, Mesh& mesh, 
      BoundaryCondition& bc) : NonlinearFunction(), _a(&a), _Lf(&L), 
      _mesh(&mesh), _bc(&bc)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
NonlinearPDE::~NonlinearPDE()
{
  // Do nothing 
}
//-----------------------------------------------------------------------------
void NonlinearPDE::form(Matrix& A, Vector& b, const Vector& x)
{
  if(!_a)
  {  
    dolfin_error("Nonlinear function update for F(u) and Jacobian has not been supplied by user.");
  }  
  else
  {
    FEM::assemble(*_a, *_Lf, A, b, *_mesh);
    if(_bc) 
    { 
      FEM::applyBC(A, *_mesh, _a->test(), *_bc);
      FEM::assembleBCresidual(b, x, *_mesh, _Lf->test(), *_bc);
    }
    else
    {
      //FIXME: Deal with zero Neumann condition on entire boundary her. Need to fix FEM::assembleBCresidual 
      // FEM::assembleBCresidual(b, x, *_mesh, _Lf->test());
      dolfin_error("Pure zero Neumann boundary conditions not yet implemented for nonlinear functions.");
    }
  }
  
}
//-----------------------------------------------------------------------------
void NonlinearPDE::F(Vector& b, const Vector& x)
{
  dolfin_error("Nonlinear PDE update for F(u)  has not been supplied by user.");
}
//-----------------------------------------------------------------------------
void NonlinearPDE::J(Matrix& A, const Vector& x)
{
  dolfin_error("Nonlinear PDE update for Jacobian has not been supplied by user.");
}
//-----------------------------------------------------------------------------
dolfin::uint NonlinearPDE::solve(Function& u)
{
  // Create solver
  NewtonSolver newton_solver;

  // Solution vector
  Vector& x = u.vector();

  // Solver nonlinear problem
  uint iterations = newton_solver.solve(*this, x);
  
  return iterations;
}
//-----------------------------------------------------------------------------
Function NonlinearPDE::solve()
{
  Function u;
  solve(u);
  return u;
}
//-----------------------------------------------------------------------------

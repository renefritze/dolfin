// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// Modified by Johan Hoffman, 2005.
// Modified by Anders Logg, 2005.

#include "dolfin/ConvectionDiffusionSolver.h"
#include "dolfin/ConvectionDiffusion.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
ConvectionDiffusionSolver::ConvectionDiffusionSolver(Mesh& mesh, 
						     NewFunction& w,
						     NewFunction& f,
						     NewBoundaryCondition& bc)
  : mesh(mesh), w(w), f(f), bc(bc)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void ConvectionDiffusionSolver::solve()
{
  real t = 0.0;  // current time
  real T = 1.0;  // final time
  real k = 0.01; // time step
  real c = 0.1;  // diffusion

  Matrix A;                 // matrix defining linear system
  Vector x0, x1, b;         // vectors 
  GMRES solver;             // linear system solver
  NewFunction u0(x0, mesh); // function at left end-point
  NewFunction u1(x1, mesh); // function at right end-point
  File file("convdiff.m");  // file for saving solution

  // Create variational forms
  ConvectionDiffusion::BilinearForm a(w, k, c);
  ConvectionDiffusion::LinearForm L(u0, w, f, k, c);

  // Assemble stiffness matrix
  NewFEM::assemble(a, A, mesh);

  // FIXME: Temporary fix
  x1.init(mesh.noNodes());

  // Start time-stepping
  Progress p("Time-stepping");
  while ( t < T )
  {
    // Make time step
    t += k;
    x0 = x1;
    
    // Assemble load vector and set boundary conditions
    NewFEM::assemble(L, b, mesh);
    NewFEM::setBC(A, b, mesh, bc);
    
    // Solve the linear system
    solver.solve(A, x1, b);
    
    // Save the solution
    u1.set(t);
    file << u1;

    // Update progress
    p = t / T;
  }
}
//-----------------------------------------------------------------------------
void ConvectionDiffusionSolver::solve(Mesh& mesh, NewFunction& w, NewFunction& f,
				      NewBoundaryCondition& bc)
{
  ConvectionDiffusionSolver solver(mesh, w, f, bc);
  solver.solve();
}
//-----------------------------------------------------------------------------

// Copyright (C) 2011 Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2011-01-05
// Last changed:
//
// This demo solves the Stokes equations using an iterative linear solver.
// Its also demonstrates how to use a precontioner matrix that is different
// from the matrix being solved.
//
// Note that the sign for the pressure has been flipped for symmetry.
//

#include <dolfin.h>
#include "Stokes.h"
#include "StokesPreconditioner.h"

using namespace dolfin;

int main()
{
  // Sub domain for left-hand side
  class Left : public SubDomain
  {
    bool inside(const Array<double>& x, bool on_boundary) const
    {
      return x[0] < DOLFIN_EPS;
    }
  };

  // Sub domain for right-hand side
  class Right : public SubDomain
  {
    bool inside(const Array<double>& x, bool on_boundary) const
    {
      return std::abs(1.0 - x[0]) < DOLFIN_EPS;
    }
  };

  // Sub domain for top and bottom
  class TopBottom : public SubDomain
  {
    bool inside(const Array<double>& x, bool on_boundary) const
    {
      return std::abs(1.0 - x[1]) < DOLFIN_EPS || std::abs(x[1]) < DOLFIN_EPS;
    }
  };

  // Function for inflow boundary condition for velocity
  class Inflow : public Expression
  {
  public:

    Inflow() : Expression(3) {}

    void eval(Array<double>& values, const Array<double>& x) const
    {
      values[0] = -sin(x[1]*DOLFIN_PI);
      values[1] = 0.0;
      values[2] = 0.0;
    }

  };

  cout << "This demo is unlikely to converge if PETSc is not configured with the AMG preconditioners Hypre or ML." << endl;

  // Create mesh
  UnitCube mesh(16, 16, 16);

  // Create function space and subspaces
  Stokes::FunctionSpace W(mesh);
  SubSpace W0(W, 0);
  SubSpace W1(W, 1);

  // Set-up infow boundary condition
  Inflow inflow_prfofile;
  Right right;
  DirichletBC inflow(W0, inflow_prfofile, right);

  // Set-up outflow pressure boundary condition
  Constant zero(0.0);
  Left left;
  DirichletBC outflow(W1, zero, left);

  // Set-up no-slip boundary condition
  Constant zero_vector(0.0, 0.0, 0.0);
  TopBottom top_bottom;
  DirichletBC noslip(W0, zero_vector, top_bottom);

  // Collect boundary conditions
  std::vector<const DirichletBC*> bcs;
  bcs.push_back(&inflow); bcs.push_back(&outflow); bcs.push_back(&noslip);

  // Create forms for the Stokes problem
  Constant f(0.0, 0.0, 0.0);
  Stokes::BilinearForm a(W, W);
  Stokes::LinearForm L(W);
  L.f = f;

  // Create solution function
  Function w(W);

  // Create form for the Stokes preconditioner
  StokesPreconditioner::BilinearForm a_P(W, W);

  // Assemble precondtioner system (P, b_dummy)
  Matrix P;
  Vector b;
  assemble_system(P, b, a_P, L, bcs);

  // Assemble Stokes system (A, b)
  Matrix A;
  assemble_system(A, b, a, L, bcs);

  // Create Krylov solver with specified method and preconditioner
  KrylovSolver solver("tfqmr", "amg_ml");

  // Set operator (A) and precondtioner matrix (P)
  solver.set_operators(A, P);

  // Solve system
  solver.solve(w.vector(), b);

  // Split solution
  Function u = w[0];
  Function p = w[1];

  // Plot solution
  plot(u);
  plot(p);

  // Save solution in VTK format
  File ufile_pvd("velocity.pvd");
  ufile_pvd << u;
  File pfile_pvd("pressure.pvd");
  pfile_pvd << p;
}

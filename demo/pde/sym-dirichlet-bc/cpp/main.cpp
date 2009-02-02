// Copyright (C) 2006-2007 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Garth N. Wells, 2008.
//
// First added:  2006-02-07
// Last changed: 2008-12-12
//
// This demo program solves Poisson's equation,
//
//     - div grad u(x, y) = f(x, y)
//
// on the unit square with source f given by
//
//     f(x, y) = 500*exp(-((x - 0.5)^2 + (y - 0.5)^2)/0.02)
//
// and boundary conditions given by
//
//     u(x, y)     = 0               for x = 0,
//     du/dn(x, y) = 25 cos(5 pi y)  for x = 1,
//     du/dn(x, y) = 0               otherwise.

#include <dolfin.h>
#include "Poisson.h"

using namespace dolfin;

int main()
{
  // Source term
  class Source : public Function
  {
    void eval(double* values, const double* x) const
    {
      double dx = x[0] - 0.5;
      double dy = x[1] - 0.5;
      values[0] = 500.0*exp(-(dx*dx + dy*dy)/0.02);
    }
  };

  // Neumann boundary condition
  class Flux : public Function
  {
    void eval(double* values, const double* x) const
    {
      if (x[0] > (1.0 - DOLFIN_EPS))
        values[0] = 25.0*cos(5.0*DOLFIN_PI*x[1]);
      else
        values[0] = 0.0;
    }
  };

  // Sub domain for Dirichlet boundary condition
  class DirichletBoundary : public SubDomain
  {
    bool inside(const double* x, bool on_boundary) const
    {
      return x[0] < DOLFIN_EPS;
    }
  };

  // Create mesh
  UnitSquare mesh(128, 128);

  // Create functions
  Source f;
  Flux g;

  // Define forms and attach functions
  PoissonFunctionSpace V(mesh);
  PoissonBilinearForm a(V, V);
  PoissonLinearForm L(V);
  L.f = f; L.g = g;

  // Create boundary condition
  Constant u0(0.0);
  DirichletBoundary boundary;
  DirichletBC bc(V, u0, boundary);

  // Create function
  Function u(V);

  // Create table
  Table table("Assembly and application of bcs");

  // Matrix and vector to assemble
  Matrix A;
  Vector b;

  // Assemble A and b separately
  tic();
  assemble(A, a);
  assemble(b, L);
  bc.apply(A, b);
  table("Standard", "Assembly time") = toc();

  // Assemble A and b together
  tic();
  assemble_system(A, b, a, L, bc);
  table("Symmetric", "Assembly time") = toc();

  // Display summary
  table.disp();
 
  // Solve system
  GenericVector& x = u.vector();
  LUSolver solver(symmetric);
  solver.solve(A, x, b);

  // Plot solution
  plot(u);

  // Save solution in VTK format
  File file("poisson.pvd");
  file << u;

  return 0;
}

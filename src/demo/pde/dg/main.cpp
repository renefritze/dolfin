// Copyright (C) 2006 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2006-12-??
// Last changed: 2006-12-14
//
// This demo program solves Poisson's equation
//
//     - div grad u(x, y) = f(x, y)
//
// on the unit square with source f given by
//
//     f(x, y) = 500*exp(-((x-0.5)^2 + (y-0.5)^2)/0.02)
//
// and boundary conditions given by
//
//     u(x, y)     = 0  for x = 0
//     du/dn(x, y) = 1  for x = 1
//     du/dn(x, y) = 0  otherwise
//
// using a discontinuous Galerkin formulation (interior penalty method)

#include <dolfin.h>
#include "Poisson.h"
#include "Projection.h"

using namespace dolfin;

int main()
{
  // Right-hand side
  class Source : public Function
  {
    real eval(const Point& p, unsigned int i)
    {
      real dx = p.x() - 0.5;
      real dy = p.y() - 0.5;
      return 500.0*exp(-(dx*dx + dy*dy)/0.02);
    }
  };

  // Dirichlet boundary condition
  class DirichletBC : public BoundaryCondition
  {
    void eval(BoundaryValue& value, const Point& p, unsigned int i)
    {
      if ( std::abs(p.x() - 0.0) < DOLFIN_EPS )
        value = 0.0;
    }
  };
  
  // Neumann boundary condition
  class NeumannBC : public Function
  {
    real eval(const Point& p, unsigned int i)
    {
      if ( std::abs(p.x() - 1.0) < DOLFIN_EPS )
        return 1.0;
      else
        return 0.0;  
    }
  };

  // Set up problem
  Source f;
  DirichletBC bc;
  NeumannBC g;
  FacetNormal n;
  UnitSquare mesh(16, 16);
  double alpha(2.0);

  Poisson::BilinearForm a(n,alpha);
  Poisson::LinearForm L(f, g);
  PDE pde(a, L, mesh, bc);
  dolfin_log(true);

  // Compute solution
  Function U = pde.solve();

  // Project solution onto a continuous basis
  Projection::BilinearForm a_p;
  Projection::LinearForm L_p(U);
  PDE pde_p(a_p, L_p, mesh, bc);
  Function U_p = pde_p.solve();

  // Save projected solution to file
//  File file("poisson.pvd");
//  file << U;

  File file_p("poisson_projected.pvd");
  file_p << U_p;
  return 0;
}

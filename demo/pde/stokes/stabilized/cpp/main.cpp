// Copyright (C) 2006-2008 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2006-02-09
// Last changed: 2008-12-12
//
// This demo solves the Stokes equations, using stabilized
// first order elements for the velocity and pressure. The
// sub domains for the different boundary conditions used
// in this simulation are computed by the demo program in
// src/demo/mesh/subdomains.

#include <dolfin.h>
#include "Stokes.h"

using namespace dolfin;

int main()
{
  // Function for no-slip boundary condition for velocity
  class Noslip : public Function
  {
    void eval(double* values, const double* x) const
    {
      values[0] = 0.0;
      values[1] = 0.0;
    }
  };

  // Function for inflow boundary condition for velocity
  class Inflow : public Function
  {
    void eval(double* values, const double* x) const
    {
      values[0] = -sin(x[1]*DOLFIN_PI);
      values[1] = 0.0;
    }
  };

  // Read mesh and sub domain markers
  Mesh mesh("../../../../../data/meshes/dolfin-2.xml.gz");
  MeshFunction<unsigned int> sub_domains(mesh, "../subdomains.xml.gz");

  // Create function space and subspaces
  Stokes::FunctionSpace W(mesh);
  SubSpace W0(W, 0);
  SubSpace W1(W, 1);

  // Create functions for boundary conditions
  Noslip noslip;
  Inflow inflow;
  Constant zero(0.0);

  // No-slip boundary condition for velocity
  DirichletBC bc0(W0, noslip, sub_domains, 0);

  // Inflow boundary condition for velocity
  DirichletBC bc1(W0, inflow, sub_domains, 1);

  // Boundary condition for pressure at outflow
  DirichletBC bc2(W1, zero, sub_domains, 2);

  // Collect boundary conditions
  std::vector<BoundaryCondition*> bcs;
  bcs.push_back(&bc0); bcs.push_back(&bc1); bcs.push_back(&bc2);

  // Set up PDE
  MeshSize h;
  Constant f(2, 0.0);
  Stokes::BilinearForm a(W, W);
  a.h = h;
  Stokes::LinearForm L(W);
  L.f = f; L.h = h;
  VariationalProblem pde(a, L, bcs);

  // Solve PDE
  Function w;
  pde.set("linear solver", "direct");
  pde.solve(w);

  // Extract subfunctions
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

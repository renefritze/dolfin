// Copyright (C) 2009 Harish Narayanyan.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2009-09-29
// Last changed:
//
// This demo program solves a hyperelastic problem

#include <dolfin.h>
#include "HyperElasticity.h"

using namespace dolfin;

// Sub domain for clamp at left end
class Left : public SubDomain
{
  bool inside(const double* x, bool on_boundary) const
  {
    return (std::abs(x[0]) < DOLFIN_EPS) && on_boundary;
  }
};

// Sub domain for rotation at right end
class Right : public SubDomain
{
  bool inside(const double* x, bool on_boundary) const
  {
    return (std::abs(x[0] - 1.0) < DOLFIN_EPS) && on_boundary;
  }
};

// Dirichlet boundary condition for clamp at left end
class Clamp : public Expression
{
public:

  Clamp() : Expression(3, 3) {}

  void eval(double* values, const double* x) const
  {
    values[0] = 0.0;
    values[1] = 0.0;
    values[2] = 0.0;
  }

};

// Dirichlet boundary condition for rotation at right end
class Rotation : public Expression
{
public:

  Rotation() : Expression(3, 3) {}

  void eval(double* values, const double* x) const
  {
    // Center of rotation
    double y0 = 0.5;
    double z0 = 0.5;

    // Large angle of rotation (60 degrees)
    double theta = 1.04719755;

    // New coordinates
    double y = y0 + (x[1] - y0)*cos(theta) - (x[2] - z0)*sin(theta);
    double z = z0 + (x[1] - y0)*sin(theta) + (x[2] - z0)*cos(theta);

    // Rotate at right end
    values[0] = 0.0;
    values[1] = y - x[1];
    values[2] = z - x[2];
  }

};

int main()
{
  // Create mesh and define function space
  UnitCube mesh (8, 8, 8);
  HyperElasticity::FunctionSpace V(mesh);

  // Define Dirichlet boundary condition
  Left left;
  Right right;

  Clamp c;
  Rotation r;

  DirichletBC bcl(V, c, left);
  DirichletBC bcr(V, r, right);

  std::vector<BoundaryCondition*> bcs;
  bcs.push_back(&bcl);
  bcs.push_back(&bcr);

  // Define source and solution functions
  Constant B(3, 0.0);
  Constant T(3, 0.0);
  Function u(V);

  // Set material parameters
  double E  = 10.0;
  double nu = 0.3;
  Constant mu(mesh, E / (2*(1 + nu)));
  Constant lambda(mesh, E*nu / ((1 + nu)*(1 - 2*nu)));

  // Create forms
  HyperElasticity::BilinearForm a(V, V);
  a.mu = mu; a.lmbda = lambda;
  a.u = u;
  HyperElasticity::LinearForm L(V);
  L.mu = mu; L.lmbda = lambda; L.B = B; L.T = T;
  L.u = u;

  // Solve nonlinear variational problem
  VariationalProblem problem(a, L, bcs, true);
  problem.solve(u);

  // Save solution in VTK format
  File file("displacement.pvd");
  file << u;

  // Plot solution
  plot(u);

  return 0;
}

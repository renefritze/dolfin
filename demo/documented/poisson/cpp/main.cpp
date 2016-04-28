// Copyright (C) 2006-2011 Anders Logg
//
// This file is part of DOLFIN.
//
// DOLFIN is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DOLFIN is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with DOLFIN. If not, see <http://www.gnu.org/licenses/>.
//
// First added:  2006-02-07
// Last changed: 2013-03-11
//
// This demo program solves Poisson's equation
//
//     - div grad u(x, y) = f(x, y)
//
// on the unit square with source f given by
//
//     f(x, y) = 10*exp(-((x - 0.5)^2 + (y - 0.5)^2) / 0.02)
//
// and boundary conditions given by
//
//     u(x, y) = 0        for x = 0 or x = 1
// du/dn(x, y) = sin(5*x) for y = 0 or y = 1

#include <dolfin.h>
#include <dolfin/mesh/TopologyComputation.h>
#include "Poisson.h"

using namespace dolfin;

// Source term (right-hand side)
class Source : public Expression
{
  void eval(Array<double>& values, const Array<double>& x) const
  {
    double dx = x[0] - 0.5;
    double dy = x[1] - 0.5;
    values[0] = 100000.0*10*exp(-(dx*dx + dy*dy) / 0.02);
  }
};

// Normal derivative (Neumann boundary condition)
class dUdN : public Expression
{
  void eval(Array<double>& values, const Array<double>& x) const
  {
    values[0] = sin(5*x[0]);
  }
};

// Sub domain for Dirichlet boundary condition
class DirichletBoundary : public SubDomain
{
  bool inside(const Array<double>& x, bool on_boundary) const
  {
    return x[0] < DOLFIN_EPS or x[0] > 1.0 - DOLFIN_EPS;
  }
};

int main()
{
  parameters["reorder_dofs_serial"] = false;


  // Create mesh and function space
  auto mesh = std::make_shared<UnitSquareMesh>(2, 1);

  //TopologyComputation::compute_entities(*mesh, 1);
  //TopologyComputation::compute_entities_new(*mesh, 1);
  //return 0;

  std::cout << mesh->topology().str(true) << std::endl;

  mesh->init(1, 2);

  std::cout << "Facet iterator" << std::endl;
  int tmp_count = 0;
  for (FacetIterator f(*mesh); !f.end(); ++f)
  {
    std::cout << "  index: " << f->index() << std::endl;
    Point p = f->midpoint();
    std::cout << "      x: " << p[0] << ", " << p[1] << std::endl;
    std::cout << "      e: " << f->entities(0)[0]
              << ", " << f->entities(0)[1] << std::endl;
    if (f->num_entities(2) == 2)
    {
      std::cout << "      c2: " << f->entities(2)[0]
                << ", " << f->entities(2)[1] << std::endl;
    }
    else
      std::cout << "      c1: " << f->entities(2)[0] << std::endl;

    ++tmp_count;
  }
  std::cout << "Num facets in facet iterator: " << tmp_count << std::endl;

  //TopologyComputation::compute_entities_new(*mesh, 1);
  //return 0;

  auto V = std::make_shared<Poisson::FunctionSpace>(mesh);

  // Define boundary condition
  auto u0 = std::make_shared<Constant>(0.0);
  auto boundary = std::make_shared<DirichletBoundary>();
  DirichletBC bc(V, u0, boundary);

  // Define variational forms
  Poisson::BilinearForm a(V, V);
  Poisson::LinearForm L(V);

  auto f = std::make_shared<Source>();
  auto g = std::make_shared<dUdN>();
  L.f = f;
  L.g = g;

  // Compute solution
  Function u(V);
  solve(a == L, u, bc);

  // Save solution in VTK format
  File file("poisson.pvd");
  file << u;

  std::cout << "Soln norm: " << u.vector()->norm("l2") << std::endl;

  // Plot solution
  plot(u);
  interactive();

  return 0;
}

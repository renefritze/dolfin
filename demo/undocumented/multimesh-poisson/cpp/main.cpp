// Copyright (C) 2013-2015 Anders Logg
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
// First added:  2013-06-26
// Last changed: 2016-03-02
//
// This demo program solves Poisson's equation on a domain defined by
// three overlapping and non-matching meshes. The solution is computed
// on a sequence of rotating meshes to test the multimesh
// functionality.

#include <cmath>
#include <dolfin.h>
#include "MultiMeshPoisson.h"

using namespace dolfin;
using std::make_shared;

// Sub domain for Dirichlet boundary condition
class DirichletBoundary : public SubDomain
{
  bool inside(const Array<double>& x, bool on_boundary) const
  {
    return on_boundary;
  }
};

// Compute solution for given mesh configuration
std::shared_ptr<MultiMeshFunction> solve_poisson(double t,
                                                 double x1, double y1,
                                                 double x2, double y2)
{
  // Create meshes
  double r = 0.5;
  auto mesh_0 = make_shared<RectangleMesh>(Point(-r, -r), Point(r, r), 16, 16);
  auto mesh_1 = make_shared<RectangleMesh>(Point(x1 - r, y1 - r), Point(x1 + r, y1 + r), 8, 8);
  auto mesh_2 = make_shared<RectangleMesh>(Point(x2 - r, y2 - r), Point(x2 + r, y2 + r), 8, 8);
  mesh_1->rotate(70*t);
  mesh_2->rotate(-70*t);

  // Build multimesh
  auto multimesh = make_shared<MultiMesh>();
  multimesh->add(mesh_0);
  multimesh->add(mesh_1);
  multimesh->add(mesh_2);
  multimesh->build();

  // Create function space
  auto V = make_shared<MultiMeshPoisson::MultiMeshFunctionSpace>(multimesh);

  // Create forms
  auto a = make_shared<MultiMeshPoisson::MultiMeshBilinearForm>(V, V);
  auto L = make_shared<MultiMeshPoisson::MultiMeshLinearForm>(V);

  // Attach coefficients
  auto f = make_shared<Constant>(1);
  L->f = f;

  // Assemble linear system
  auto A = make_shared<Matrix>();
  auto b = make_shared<Vector>();
  assemble_multimesh(*A, *a);
  assemble_multimesh(*b, *L);

  // Apply boundary condition
  auto zero = make_shared<Constant>(0);
  auto boundary = make_shared<DirichletBoundary>();
  auto bc = make_shared<MultiMeshDirichletBC>(V, zero, boundary);
  bc->apply(*A, *b);

  // Remove inactive dofs
  V->lock_inactive_dofs(*A, *b);

  // Compute solution
  auto u = make_shared<MultiMeshFunction>(V);
  solve(*A, *u->vector(), *b);

  return u;
}

int main(int argc, char* argv[])
{
  if (dolfin::MPI::size(MPI_COMM_WORLD) > 1)
  {
    info("Sorry, this demo does not (yet) run in parallel.");
    return 0;
  }

  // Parameters
  const double T = 10.0;
  const std::size_t N = 100;
  const double dt = T / N;

  // Create files for output
  XDMFFile f0("output/u0.xdmf");
  XDMFFile f1("output/u1.xdmf");
  XDMFFile f2("output/u2.xdmf");

  // Iterate over configurations
  for (std::size_t n = 0; n < N; n++)
  {
    info("Computing solution, step %d / %d.", n + 1, N);

    // Compute coordinates for meshes
    const double t = dt*n;
    const double x1 = sin(t)*cos(2*t);
    const double y1 = cos(t)*cos(2*t);
    const double x2 = cos(t)*cos(2*t);
    const double y2 = sin(t)*cos(2*t);

    // Compute solution
    std::shared_ptr<MultiMeshFunction> u = solve_poisson(t, x1, y1, x2, y2);

    // Save to file
    f0.write(*u->part(0), t);
    f1.write(*u->part(1), t);
    f2.write(*u->part(2), t);
  }

  // Close files
  f0.close();
  f1.close();
  f2.close();

  return 0;
}

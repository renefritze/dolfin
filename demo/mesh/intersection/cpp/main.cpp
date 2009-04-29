// Copyright (C) 2008 Kristoffer Selim.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2008-10-08
// Last changed: 2008-12-18
//
// Modified by Anders Logg, 2008.

#include <dolfin.h>
#include <math.h>

using namespace dolfin;

int main()
{
  // Create meshes (omega0 overlapped by omega1)
  UnitCircle omega0(20);
  UnitSquare omega1(20, 20);

  // Access mesh geometry
  MeshGeometry& geometry = omega0.geometry();

  // Move and scale circle
  for (VertexIterator vertex(omega0); !vertex.end(); ++vertex)
  {
    double* x = geometry.x(vertex->index());
    x[0] = 0.5*x[0] + 1.0;
    x[1] = 0.5*x[1] + 1.0;
  }

  // Iterate over angle
  const double dtheta = 0.1*DOLFIN_PI;
  for (double theta = 0; theta < 2*DOLFIN_PI; theta += dtheta)
  {
    // Compute intersection with boundary of square
    BoundaryMesh boundary(omega1);
    std::vector<unsigned int> cells;
    omega0.intersection(boundary, cells, false);

    // Copy values to mesh function for plotting
    MeshFunction<unsigned int> intersection(omega0, omega0.topology().dim());
    intersection = 0;
    for (unsigned int i = 0; i < cells.size(); i++)
      intersection.set(cells[i], 1);

    // Plot intersection
    plot(intersection);

    // Rotate circle around (0.5, 0.5)
    for (VertexIterator vertex(omega0); !vertex.end(); ++vertex)
    {
      double* x = geometry.x(vertex->index());
      const double xr = x[0] - 0.5;
      const double yr = x[1] - 0.5;
      x[0] = 0.5 + (cos(dtheta)*xr - sin(dtheta)*yr);
      x[1] = 0.5 + (sin(dtheta)*xr + cos(dtheta)*yr);
    }
  }
}

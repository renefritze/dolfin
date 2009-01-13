// Copyright (C) 2008-2009 Solveig Bruvoll and Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2008-05-02
// Last changed: 2009-01-12
//
// This demo demonstrates how to move the vertex coordinates
// of a boundary mesh and then updating the interior vertex
// coordinates of the original mesh by suitably interpolating
// the vertex coordinates (useful for implementation of ALE
// methods).

#include <dolfin.h>

using namespace dolfin;

int main()
{
  // Create mesh
  UnitSquare mesh(20, 20);

  // Create boundary mesh
  BoundaryMesh boundary(mesh);

  // Move vertices in boundary
  MeshGeometry& geometry = boundary.geometry();
  for (VertexIterator v(boundary); !v.end(); ++v)
  {
    double* x = geometry.x(v->index());
    x[0] *= 3.0;
    x[1] += 0.1*sin(5.0*x[0]);
  }
  
  // Move mesh
  mesh.move(boundary);

  // Plot mesh
  plot(mesh);

  return 0;
}

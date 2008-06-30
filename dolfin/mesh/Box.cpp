// Copyright (C) 2005-2006 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Garth N. Wells, 2007.
// Modified by Nuno Lopes, 2008
//
// First added:  2005-12-02
// Last changed: 2008-06-19

#include "MeshEditor.h"
#include "Box.h"
#include <dolfin/main/MPI.h>
#include "MPIMeshCommunicator.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
Box::Box(const real a,const  real b,const real c,const real d,const real e,const real f, uint nx, uint ny, uint nz) : Mesh()
{
  // Receive mesh according to parallel policy
  if (MPI::receive()) { MPIMeshCommunicator::receive(*this); return; }

  if ( nx < 1 || ny < 1 || nz < 1 )
    error("Size of unit cube must be at least 1 in each dimension.");

  rename("mesh", "Mesh of the cuboid (a,b)x(c,d)x(e,f)");

  // Open mesh for editing
  MeshEditor editor;
  editor.open(*this, CellType::tetrahedron, 3, 3);

  // Create vertices
  editor.initVertices((nx+1)*(ny+1)*(nz+1));
  uint vertex = 0;
  for (uint iz = 0; iz <= nz; iz++)
  {
    const real z = e+ (static_cast<real>(iz))*(f-e) / static_cast<real>(nz);
    for (uint iy = 0; iy <= ny; iy++)
    {
      const real y =c+ (static_cast<real>(iy))*(d-e) / static_cast<real>(ny);
      for (uint ix = 0; ix <= nx; ix++)
      {
        const real x = a+(static_cast<real>(ix))*(b-a) / static_cast<real>(nx);
        editor.addVertex(vertex++, x, y, z);
      }
    }
  }

  // Create tetrahedra
  editor.initCells(6*nx*ny*nz);
  uint cell = 0;
  for (uint iz = 0; iz < nz; iz++)
  {
    for (uint iy = 0; iy < ny; iy++)
    {
      for (uint ix = 0; ix < nx; ix++)
      {
        const uint v0 = iz*(nx + 1)*(ny + 1) + iy*(nx + 1) + ix;
        const uint v1 = v0 + 1;
        const uint v2 = v0 + (nx + 1);
        const uint v3 = v1 + (nx + 1);
        const uint v4 = v0 + (nx + 1)*(ny + 1);
        const uint v5 = v1 + (nx + 1)*(ny + 1);
        const uint v6 = v2 + (nx + 1)*(ny + 1);
        const uint v7 = v3 + (nx + 1)*(ny + 1);

        editor.addCell(cell++, v0, v1, v3, v7);
        editor.addCell(cell++, v0, v1, v7, v5);
        editor.addCell(cell++, v0, v5, v7, v4);
        editor.addCell(cell++, v0, v3, v2, v7);
        editor.addCell(cell++, v0, v6, v4, v7);
        editor.addCell(cell++, v0, v2, v6, v7);
      }
    }
  }

  // Close mesh editor
  editor.close();

  // Broadcast mesh according to parallel policy
  if (MPI::broadcast()) { MPIMeshCommunicator::broadcast(*this); }
}
//-----------------------------------------------------------------------------

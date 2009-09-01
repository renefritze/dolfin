// Copyright (C) 2008 Kristian B. Oelgaard.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2007-11-23
// Last changed: 2008-11-13

#include <dolfin/main/MPI.h>
#include "MeshPartitioning.h"
#include "MeshEditor.h"
#include "UnitInterval.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
UnitInterval::UnitInterval(uint nx) : Mesh()
{
  // Receive mesh according to parallel policy
  if (MPI::is_receiver()) { MeshPartitioning::partition(*this); return; }

  if ( nx < 1 )
    error("Size of unit interval must be at least 1.");

  rename("mesh", "Mesh of the unit interval (0,1)");

  // Open mesh for editing
  MeshEditor editor;
  editor.open(*this, CellType::interval, 1, 1);

  // Create vertices and cells:
  editor.init_vertices((nx+1));
  editor.init_cells(nx);

  // Create main vertices:
  for (uint ix = 0; ix <= nx; ix++)
  {
    const double x = static_cast<double>(ix) / static_cast<double>(nx);
    editor.add_vertex(ix, x);
  }

  // Create intervals
  for (uint ix = 0; ix < nx; ix++) {
  	const uint v0 = ix;
  	const uint v1 = v0 + 1;
  	editor.add_cell(ix, v0, v1);
  }

  // Close mesh editor
  editor.close();

  // Broadcast mesh according to parallel policy
  if (MPI::is_broadcaster()) { MeshPartitioning::partition(*this); return; }
}
//-----------------------------------------------------------------------------

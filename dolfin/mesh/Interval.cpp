// Copyright (C) 2007 Kristian B. Oelgaard.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by N. Lopes, 2008.
//
// First added:  2007-11-23
// Last changed: 2008-10-13

#include "MeshEditor.h"
#include "Interval.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
Interval::Interval(uint nx,double a,double b) : Mesh()
{
  if ( nx < 1 )
    error("Size of unit interval must be at least 1.");

  rename("mesh", "Mesh of the unit interval (0,1)");

  // Open mesh for editing
  MeshEditor editor;
  editor.open(*this, CellType::interval, 1, 1);

  // Create vertices and cells:
  editor.initVertices((nx+1));
  editor.initCells(nx);

  // Create main vertices:
  for (uint ix = 0; ix <= nx; ix++)
  {
    const double x = a+(static_cast<double>(ix)*(b-a) / static_cast<double>(nx));
    editor.addVertex(ix, x);
  }

  // Create intervals
  for (uint ix = 0; ix < nx; ix++)
  {
    const uint v0 = ix;
    const uint v1 = v0 + 1;
    editor.addCell(ix, v0, v1);
  }

  // Close mesh editor
  editor.close();
}
//-----------------------------------------------------------------------------

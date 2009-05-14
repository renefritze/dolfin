// Copyright (C) 2005-2008 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Garth N. Wells 2007.
// Modified by Nuno Lopes 2008
//
// First added:  2005-12-02
// Last changed: 2008-11-13

#include "MeshEditor.h"
#include "UnitCircle.h"
#include <dolfin/main/MPI.h>
#include "MPIMeshCommunicator.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
UnitCircle::UnitCircle(uint nx, Type type, Transformation transformation) : Mesh()
{
  warning("UnitCircle is Experimental: It may be of poor quality.");

  uint ny=nx;
  // Receive mesh according to parallel policy
  if (MPI::receive()) { MPIMeshCommunicator::receive(*this); return; }

  if ( nx < 1 || ny < 1 )
    error("Size of unit square must be at least 1 in each dimension.");

  rename("mesh", "Mesh of the unit square (0,1) x (0,1)");

  // Open mesh for editing
  MeshEditor editor;
  editor.open(*this, CellType::triangle, 2, 2);

  // Create vertices and cells:
  if (type == crisscross)
  {
    editor.init_vertices((nx+1)*(ny+1) + nx*ny);
    editor.init_cells(4*nx*ny);
  }
  else
  {
    editor.init_vertices((nx+1)*(ny+1));
    editor.init_cells(2*nx*ny);
  }

  // Create main vertices:
  // variables for transformation
  double trns_x = 0.0;
  double trns_y = 0.0;
  uint vertex = 0;
  for (uint iy = 0; iy <= ny; iy++)
  {
    const double y = -1.0 + static_cast<double>(iy)*2.0 / static_cast<double>(ny);
    for (uint ix = 0; ix <= nx; ix++)
    {
      const double x =-1.+ static_cast<double>(ix)*2.0 / static_cast<double>(nx);
      trns_x = transformx(x, y, transformation);
      trns_y = transformy(x, y, transformation);
      editor.add_vertex(vertex++, trns_x, trns_y);
    }
  }

  // Create midpoint vertices if the mesh type is crisscross
  if (type == crisscross)
  {
    for (uint iy = 0; iy < ny; iy++)
    {
      const double y = -1.0 + (static_cast<double>(iy) + 0.5)*2.0 / static_cast<double>(ny);
      for (uint ix = 0; ix < nx; ix++)
      {
        const double x = -1.0 + (static_cast<double>(ix) + 0.5)*2.0 / static_cast<double>(nx);
        trns_x = transformx(x, y, transformation);
        trns_y = transformy(x, y, transformation);
        editor.add_vertex(vertex++, trns_x, trns_y);
      }
    }
  }

  // Create triangles
  uint cell = 0;
  if (type == crisscross)
  {
    for (uint iy = 0; iy < ny; iy++)
    {
      for (uint ix = 0; ix < nx; ix++)
      {
        const uint v0 = iy*(nx + 1) + ix;
        const uint v1 = v0 + 1;
        const uint v2 = v0 + (nx + 1);
        const uint v3 = v1 + (nx + 1);
        const uint vmid = (nx + 1)*(ny + 1) + iy*nx + ix;

        // Note that v0 < v1 < v2 < v3 < vmid.
        editor.add_cell(cell++, v0, v1, vmid);
        editor.add_cell(cell++, v0, v2, vmid);
        editor.add_cell(cell++, v1, v3, vmid);
        editor.add_cell(cell++, v2, v3, vmid);
      }
    }
  }
  else if (type == left )
  {
    for (uint iy = 0; iy < ny; iy++)
    {
      for (uint ix = 0; ix < nx; ix++)
      {
        const uint v0 = iy*(nx + 1) + ix;
        const uint v1 = v0 + 1;
        const uint v2 = v0 + (nx + 1);
        const uint v3 = v1 + (nx + 1);

        editor.add_cell(cell++, v0, v1, v2);
        editor.add_cell(cell++, v1, v2, v3);
      }
    }
  }
  else
  {
    for (uint iy = 0; iy < ny; iy++)
    {
      for (uint ix = 0; ix < nx; ix++)
      {
        const uint v0 = iy*(nx + 1) + ix;
        const uint v1 = v0 + 1;
        const uint v2 = v0 + (nx + 1);
        const uint v3 = v1 + (nx + 1);

        editor.add_cell(cell++, v0, v1, v3);
        editor.add_cell(cell++, v0, v2, v3);
      }
    }
  }

  // Close mesh editor
  editor.close();

  // Broadcast mesh according to parallel policy
  if (MPI::broadcast()) { MPIMeshCommunicator::broadcast(*this); }
}
//-----------------------------------------------------------------------------
double UnitCircle::transformx(double x, double y, Transformation transformation)
{
  //maxn transformation
  if(transformation == maxn)
  {
    if (x||y) //in (0,0) (trns_x,trans_y)=(nan,nan)
      return x*max(fabs(x),fabs(y))/sqrt(x*x+y*y);
    else
      return x;
  }
  //sumn transformation
  else if(transformation == sumn)
  {
    if (x||y) //in (0,0) (trns_x,trans_y)=(nan,nan)
      return x*(fabs(x)+fabs(y))/sqrt(x*x+y*y);
    else
      return x;
  }
  else
  {
    // FIXME: Use easier to understand check
    if((transformation != maxn)*(transformation != sumn)*(transformation != rotsumn))
    {
      info("Implemented  transformations are: maxn,sumn and rotsumn");
      info("Using rotsumn transformation");
    }
    if(x||y) //in (0,0) (trns_x,trans_y)=(nan,nan)
    {
      double xx = 0.5*(x+y);
      double yy = 0.5*(-x+y);
      return xx*(fabs(xx)+fabs(yy))/sqrt(xx*xx+yy*yy);
    }
    else
      return y;
  }
}
//-----------------------------------------------------------------------------
double UnitCircle::transformy(double x, double y, Transformation transformation)
{
  //maxn transformation
  if(transformation == maxn)
  {
    if (x||y) //in (0,0) (trns_x,trans_y)=(nan,nan)
      return y*max(fabs(x),fabs(y))/sqrt(x*x+y*y);
    else
      return y;
  }
  //sumn transformation
  else if (transformation == sumn)
  {
    if (x||y) //in (0,0) (trns_x,trans_y)=(nan,nan)
      return y*(fabs(x)+fabs(y))/sqrt(x*x+y*y);
    else
      return y;
  }
  else
  {
    if ((transformation != maxn)*(transformation != sumn)*(transformation != rotsumn))
    {
      info("Implemented  transformations for are: maxn, sumn and rotsumn");
      info("Using rotsumn transformation");
    }
    if (x||y) //in (0,0) (trns_x,trans_y)=(nan,nan)
    {
      double xx = 0.5*(x+y);
      double yy = 0.5*(-x+y);
      return yy*(fabs(xx)+fabs(yy))/sqrt(xx*xx+yy*yy);
    }
    else
      return y;
  }
}
//-----------------------------------------------------------------------------

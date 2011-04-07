// Copyright (C) 2006-2010 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Garth N. Wells, 2010
//
// First added:  2006-06-08
// Last changed: 2011-04-07

#include <dolfin/math/dolfin_math.h>
#include <dolfin/log/dolfin_log.h>
#include <dolfin/mesh/Mesh.h>
#include <dolfin/mesh/MeshTopology.h>
#include <dolfin/mesh/MeshGeometry.h>
#include <dolfin/mesh/MeshConnectivity.h>
#include <dolfin/mesh/MeshEditor.h>
#include <dolfin/mesh/Vertex.h>
#include <dolfin/mesh/Edge.h>
#include <dolfin/mesh/Cell.h>
#include "UniformMeshRefinement.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
void UniformMeshRefinement::refine(Mesh& refined_mesh,
                                   const Mesh& mesh)
{
  log(TRACE, "Refining simplicial mesh uniformly.");

  // Check that refined_mesh and mesh are not the same
  if (&refined_mesh == &mesh)
    error("UniformMeshRefinement::refine: refined_mesh and mesh point to the same object");

  // Generate cell - edge connectivity if not generated
  mesh.init(mesh.topology().dim(), 1);

  // Generate edge - vertex connectivity if not generated
  mesh.init(1, 0);

  // Mesh needs to be ordered (so we can pick right combination of vertices/edges)
  if (!mesh.ordered())
    error("Unable to refine mesh. Mesh is not ordered according to the UFC numbering convention, consider calling mesh.order().");

  // Get cell type
  const CellType& cell_type = mesh.type();

  // Open new mesh for editing
  MeshEditor editor;
  editor.open(refined_mesh, cell_type.cell_type(),
              mesh.topology().dim(), mesh.geometry().dim());

  // Get size of mesh
  const uint num_vertices = mesh.size(0);
  const uint num_edges = mesh.size(1);
  const uint num_cells = mesh.size(mesh.topology().dim());

  // Specify number of vertices and cells
  editor.init_vertices(num_vertices + num_edges);
  editor.init_cells(ipow(2, mesh.topology().dim())*num_cells);

  // Add old vertices
  uint vertex = 0;
  for (VertexIterator v(mesh); !v.end(); ++v)
    editor.add_vertex(vertex++, v->point());

  // Add new vertices
  for (EdgeIterator e(mesh); !e.end(); ++e)
    editor.add_vertex(vertex++, e->midpoint());

  // Add cells
  uint current_cell = 0;
  for (CellIterator c(mesh); !c.end(); ++c)
    cell_type.refine_cell(*c, editor, current_cell);

  // Close editor
  editor.close();

  // Make sure that mesh is ordered after refinement
  //refined_mesh.order();
}
//-----------------------------------------------------------------------------

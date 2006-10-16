// Copyright (C) 2006 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2006-06-08
// Last changed: 2006-06-22

#include <dolfin/dolfin_math.h>
#include <dolfin/dolfin_log.h>
#include <dolfin/Mesh.h>
#include <dolfin/MeshTopology.h>
#include <dolfin/MeshGeometry.h>
#include <dolfin/MeshConnectivity.h>
#include <dolfin/MeshEditor.h>
#include <dolfin/Vertex.h>
#include <dolfin/Edge.h>
#include <dolfin/Cell.h>
#include <dolfin/UniformMeshRefinement.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
void UniformMeshRefinement::refine(Mesh& mesh)
{
  // Only know how to refine simplicial meshes
  refineSimplex(mesh);
}
//-----------------------------------------------------------------------------
void UniformMeshRefinement::refineSimplex(Mesh& mesh)
{
  dolfin_info("Refining simplicial mesh uniformly.");
  
  // Generate cell - edge connectivity if not generated
  mesh.init(mesh.topology().dim(), 1);
  
  // Generate edge - vertex connectivity if not generated
  mesh.init(1, 0);

  // Get cell type
  const CellType& cell_type = mesh.type();
  
  // Create new mesh and open for editing
  Mesh refined_mesh;
  MeshEditor editor;
  editor.open(refined_mesh, cell_type.cellType(),
	      mesh.topology().dim(), mesh.geometry().dim());
  
  // Get size of mesh
  const uint num_vertices = mesh.size(0);
  const uint num_edges = mesh.size(1);
  const uint num_cells = mesh.size(mesh.topology().dim());

  // Specify number of vertices and cells
  editor.initVertices(num_vertices + num_edges);
  editor.initCells(ipow(2, mesh.topology().dim())*num_cells);

  // Add old vertices
  uint vertex = 0;
  for (VertexIterator v(mesh); !v.end(); ++v)
    editor.addVertex(vertex++, v->point());

  // Add new vertices
  for (EdgeIterator e(mesh); !e.end(); ++e)
    editor.addVertex(vertex++, e->midpoint());

  // Add cells
  uint current_cell = 0;
  for (CellIterator c(mesh); !c.end(); ++c)
    cell_type.refineCell(*c, editor, current_cell);

  // Overwrite old mesh with refined mesh
  editor.close();
  mesh = refined_mesh;

  cout << "Refined mesh: " << mesh << endl;
}
//-----------------------------------------------------------------------------

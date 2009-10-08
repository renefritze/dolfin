// Copyright (C) 2009 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2009-02-11
// Last changed: 2009-10-08

#include <map>
#include <vector>

#include "Cell.h"
#include "Vertex.h"
#include "MeshEditor.h"
#include "SubDomain.h"
#include "SubMesh.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
SubMesh::SubMesh(const Mesh& mesh, const SubDomain& sub_domain)
{
  // Create mesh function and mark sub domain
  MeshFunction<uint> sub_domains(mesh, mesh.topology().dim());
  sub_domains = 0;
  sub_domain.mark(sub_domains, 1);

  // Create sub mesh
  init(mesh, sub_domains, 1);
}
//-----------------------------------------------------------------------------
SubMesh::SubMesh(const Mesh& mesh,
                 const MeshFunction<uint>& sub_domains, uint sub_domain)
{
  // Create sub mesh
  init(mesh, sub_domains, sub_domain);
}
//-----------------------------------------------------------------------------
SubMesh::~SubMesh()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void SubMesh::init(const Mesh& mesh,
                   const MeshFunction<uint>& sub_domains, uint sub_domain)
{
  // Open mesh for editing
  MeshEditor editor;
  editor.open(*this, mesh.type().cell_type(),
              mesh.topology().dim(), mesh.geometry().dim());

  // Extract cells
  std::set<uint> cells;
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    if (sub_domains[*cell] == sub_domain)
      cells.insert(cell->index());
  }

  // Map to keep track of new local indices for vertices
  std::map<uint, uint> local_vertex_indices;

  // Add cells
  editor.init_cells(cells.size());
  uint current_cell = 0;
  uint current_vertex = 0;
  for (std::set<uint>::iterator cell_it = cells.begin(); cell_it != cells.end(); ++cell_it)
  {
    std::vector<uint> cell_vertices;
    Cell cell(mesh, *cell_it);
    for (VertexIterator vertex(cell); !vertex.end(); ++vertex)
    {
      const uint global_vertex_index = vertex->index();
      uint local_vertex_index = 0;
      std::map<uint, uint>::iterator vertex_it = local_vertex_indices.find(global_vertex_index);
      if (vertex_it != local_vertex_indices.end())
      {
        local_vertex_index = vertex_it->second;
      }
      else
      {
        local_vertex_index = current_vertex++;
        local_vertex_indices[global_vertex_index] = local_vertex_index;
      }
      cell_vertices.push_back(local_vertex_index);
    }
    editor.add_cell(current_cell++, cell_vertices);
  }

  // Add vertices
  editor.init_vertices(local_vertex_indices.size());
  for (std::map<uint, uint>::iterator it = local_vertex_indices.begin(); it != local_vertex_indices.end(); ++it)
  {
    Vertex vertex(mesh, it->first);
    editor.add_vertex(it->second, vertex.point());
  }

  // Close editor
  editor.close();

  // Build local-to-global mapping for vertices
  MeshFunction<uint>* global_vertex_indices = data().create_mesh_function("global vertex indices", 0);
  for (std::map<uint, uint>::iterator it = local_vertex_indices.begin(); it != local_vertex_indices.end(); ++it)
    (*global_vertex_indices)[it->second] = it->first;
}
//-----------------------------------------------------------------------------

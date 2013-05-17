// Copyright (C) 2009-2011 Anders Logg
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
// First added:  2009-02-11
// Last changed: 2013-02-11

#include <map>
#include <vector>

#include "Cell.h"
#include "Mesh.h"
#include "MeshEditor.h"
#include "MeshEntityIterator.h"
#include "MeshFunction.h"
#include "SubDomain.h"
#include "SubMesh.h"
#include "Vertex.h"
#include "MeshValueCollection.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
SubMesh::SubMesh(const Mesh& mesh, const SubDomain& sub_domain)
{
  // Create mesh function and mark sub domain
  MeshFunction<std::size_t> sub_domains(mesh, mesh.topology().dim());
  sub_domains = 0;
  sub_domain.mark(sub_domains, 1);

  // Create sub mesh
  init(mesh, sub_domains, 1);
}
//-----------------------------------------------------------------------------
SubMesh::SubMesh(const Mesh& mesh,
                 const MeshFunction<std::size_t>& sub_domains, std::size_t sub_domain)
{
  // Create sub mesh
  init(mesh, sub_domains, sub_domain);
}
//----------------------------------------------------------------------------
SubMesh::SubMesh(const Mesh& mesh, std::size_t sub_domain)
{
  if (mesh.domains().num_marked(mesh.topology().dim()) == 0)
  {
    dolfin_error("SubMesh.cpp",
                 "construct SubMesh",
                 "Mesh does not include a MeshValueCollection the cell dimension of the mesh");
  }

  // Get MeshFunction from MeshValueCollection
  boost::shared_ptr<const MeshFunction<std::size_t> > sub_domains
    = mesh.domains().cell_domains();

  // Create sub mesh
  cout << "Create sub mesh" << endl;
  init(mesh, *sub_domains, sub_domain);
  cout << "End create sub mesh" << endl;
}
//-----------------------------------------------------------------------------
SubMesh::~SubMesh()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void SubMesh::init(const Mesh& mesh,
                   const MeshFunction<std::size_t>& sub_domains,
                   std::size_t sub_domain)
{
  // Open mesh for editing
  MeshEditor editor;
  const std::size_t cell_dim_t = mesh.topology().dim();
  editor.open(*this, mesh.type().cell_type(), cell_dim_t,
              mesh.geometry().dim());

  // Build set of cells that are in sub-mesh
  std::set<std::size_t> submesh_cells;
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    if (sub_domains[*cell] == sub_domain)
      submesh_cells.insert(cell->index());
  }

  // Map from parent vertex index to submesh vertex index
  std::map<std::size_t, std::size_t> parent_to_submesh_vertex_indices;

  // Map from submesh cell to parent cell
  std::vector<std::size_t> submesh_cell_parent_indices;
  submesh_cell_parent_indices.reserve(submesh_cells.size());

  // Vector from parent cell index to submesh cell index
  std::vector<std::size_t> parent_to_submesh_cell_indices(mesh.num_cells(), 0);

  // Add sub-mesh cells
  editor.init_cells(submesh_cells.size());
  std::size_t current_cell = 0;
  std::size_t current_vertex = 0;
  for (std::set<std::size_t>::iterator cell_it = submesh_cells.begin();
       cell_it != submesh_cells.end(); ++cell_it)
  {
    // Data structure to hold new vertex indices for cell
    std::vector<std::size_t> cell_vertices;

    // Create cell
    Cell cell(mesh, *cell_it);

    // Iterate over cell vertices
    for (VertexIterator vertex(cell); !vertex.end(); ++vertex)
    {
      const std::size_t parent_vertex_index = vertex->index();

      // Look for parent vertex in map
      std::map<std::size_t, std::size_t>::iterator vertex_it
        = parent_to_submesh_vertex_indices.find(parent_vertex_index);

      // If vertex has been inserted, get new index, otherwise
      // increment and insert
      std::size_t submesh_vertex_index = 0;
      if (vertex_it != parent_to_submesh_vertex_indices.end())
        submesh_vertex_index = vertex_it->second;
      else
      {
        submesh_vertex_index = current_vertex++;
        parent_to_submesh_vertex_indices[parent_vertex_index]
          = submesh_vertex_index;
      }

      // Add vertex to list of cell vertices (new indexing)
      cell_vertices.push_back(submesh_vertex_index);
    }

    // Add parent cell index to list
    submesh_cell_parent_indices.push_back(cell.index());

    // Store parent cell -> submesh cell indices
    parent_to_submesh_cell_indices[cell.index()] = current_cell;

    // Add cell to mesh
    editor.add_cell(current_cell++, cell_vertices);
  }

  // Vector to hold submesh vertex -> parent vertex
  std::vector<std::size_t> parent_vertex_indices;
  parent_vertex_indices.reserve((parent_to_submesh_vertex_indices.size()));

  // Initialise mesh editor
  editor.init_vertices(parent_to_submesh_vertex_indices.size());

  // Add vertices
  for (std::map<std::size_t, std::size_t>::iterator it
         = parent_to_submesh_vertex_indices.begin();
       it != parent_to_submesh_vertex_indices.end(); ++it)
  {
    Vertex vertex(mesh, it->first);
    if (MPI::num_processes() > 1)
      error("SubMesh::init not working in parallel");

    // FIXME: Get global vertex index
    editor.add_vertex(it->second, vertex.point());
    parent_vertex_indices.push_back(it->second);
  }

  // Close editor
  editor.close();

  // Build submesh-to-parent map for vertices
  boost::shared_ptr<MeshFunction<std::size_t> > parent_vertex_indices_mf
    = data().create_mesh_function("parent_vertex_indices", 0);
  for (std::map<std::size_t, std::size_t>::iterator it
         = parent_to_submesh_vertex_indices.begin();
       it != parent_to_submesh_vertex_indices.end(); ++it)
  {
    (*parent_vertex_indices_mf)[it->second] = it->first;
  }


  // Build submesh-to-parent map for cells
  boost::shared_ptr<MeshFunction<std::size_t> > parent_cell_indices
    = data().create_mesh_function("parent_cell_indices", cell_dim_t);
  current_cell = 0;
  for (std::vector<std::size_t>::iterator it = submesh_cell_parent_indices.begin();
       it != submesh_cell_parent_indices.end(); ++it)
  {
    (*parent_cell_indices)[current_cell++] = *it;
  }

  // Init present MeshDomain
  const MeshDomains& parent_domains = mesh.domains();
  this->domains().init(parent_domains.max_dim());

  // Collect MeshValueCollections from parent mesh
  for (std::size_t dim_t = 0; dim_t <= parent_domains.max_dim(); dim_t++)
  {
    // If parent mesh does not has a data for dim_t
    if (parent_domains.num_marked(dim_t) == 0)
      continue;

    // FIXME: Cam avoid building this map for cell and vertices

    // Build map from submesh entity (gloabl vertex list) -> (submesh index)
    std::map<std::vector<std::size_t>, std::size_t> entity_map;
    for (MeshEntityIterator e(mesh, dim_t); !e.end(); ++e)
    {
      // Build list of entity vertex indices and sort
      std::vector<std::size_t> vertex_list;
      for (VertexIterator v(*e); !v.end(); ++v)
        vertex_list.push_back(parent_vertex_indices[v->index()]);
      std::sort(vertex_list.begin(), vertex_list.end());

      entity_map[vertex_list] = e->index();
    }

    // Get submesh marker map
    std::map<std::size_t, std::size_t>& submesh_markers
      = this->domains().markers(dim_t);

    // Get values map from parent MeshValueCollection
    const std::map<std::size_t, std::size_t>& parent_markers
      = parent_domains.markers(dim_t);

    // Iterate over all parents marker values
    std::map<std::size_t, std::size_t>::const_iterator itt;
    for (itt = parent_markers.begin(); itt != parent_markers.end(); itt++)
    {
      // Create parent entity
      MeshEntity parent_entity(mesh, dim_t, itt->first);

      // FIXME: Need to check all attached cells

      std::size_t parent_cell_index;
      if (dim_t == cell_dim_t)
        parent_cell_index = itt->first;
      else
      {
        // Get first parent cell index attached to parent entity
        parent_cell_index = parent_entity.entities(cell_dim_t)[0];
      }

      // Get submesh cell index
      const std::size_t submesh_cell_index
        = parent_to_submesh_cell_indices[parent_cell_index];

      // Check if the cell is included in the submesh
      if (sub_domains[parent_cell_index] == sub_domain)
      {
        // Map markers from parent mesh to submesh
	if (dim_t == cell_dim_t)
	  submesh_markers[submesh_cell_index] = itt->second;
	else
	{
          std::vector<std::size_t> parent_vertex_list;
          for (VertexIterator v(parent_entity); !v.end(); ++v)
            parent_vertex_list.push_back(v->index());
          std::sort(parent_vertex_list.begin(), parent_vertex_list.end());

          // Get submesh entity index
          std::map<std::vector<std::size_t>, std::size_t>::const_iterator submesh_it;
          submesh_it = entity_map.find(parent_vertex_list);
          dolfin_assert(submesh_it != entity_map.end());

          submesh_markers[submesh_it->second] = itt->second;
	}
      }
    }
  }

}
//-----------------------------------------------------------------------------

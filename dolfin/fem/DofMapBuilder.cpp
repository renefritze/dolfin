// Copyright (C) 2008-2013 Anders Logg, Ola Skavhaug and Garth N. Wells
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
// Modified by Niclas Jansson 2009
// Modified by Garth N. Wells 2010-2012
// Modified by Mikael Mortensen, 2012.
// Modified by Joachim B Haga, 2012
// Modified by Martin Alnaes, 2013
//
// First added:  2008-08-12
// Last changed: 2013-01-08

#include <ufc.h>
#include <boost/random.hpp>
#include <boost/unordered_map.hpp>

#include <dolfin/common/Timer.h>
#include <dolfin/graph/BoostGraphOrdering.h>
#include <dolfin/graph/GraphBuilder.h>
#include <dolfin/log/log.h>
#include <dolfin/mesh/BoundaryMesh.h>
#include <dolfin/mesh/DistributedMeshTools.h>
#include <dolfin/mesh/Facet.h>
#include <dolfin/mesh/Mesh.h>
#include <dolfin/mesh/MeshEntityIterator.h>
#include <dolfin/mesh/Restriction.h>
#include <dolfin/mesh/Vertex.h>
#include "DofMap.h"
#include "UFCCell.h"
#include "DofMapBuilder.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
void DofMapBuilder::build(DofMap& dofmap, const Mesh& mesh,
  boost::shared_ptr<const Restriction> restriction)
{
  // Start timer for dofmap initialization
  Timer t0("Init dofmap");

  // Check that mesh has been ordered
  if (!mesh.ordered())
  {
     dolfin_error("DofMapBuiler.cpp",
                  "create mapping of degrees of freedom",
                  "Mesh is not ordered according to the UFC numbering convention. "
                  "Consider calling mesh.order()");
  }

  // Build dofmap based on UFC-provided map. This function does not
  // set local_range
  map restricted_dofs_inverse;
  build_ufc(dofmap, restricted_dofs_inverse, mesh, restriction);

  // Check if dofmap is distributed
  const bool distributed = MPI::num_processes() > 1;

  // Re-order dofmap when distributed for process locality and set
  // local_range
  if (distributed)
    reorder_distributed(dofmap, mesh, restriction, restricted_dofs_inverse);
  else
  {
    // Optionally re-order local dofmap for spatial locality
    const bool reorder = dolfin::parameters["reorder_dofs_serial"];
    if (reorder)
      reorder_local(dofmap, mesh);

    // Set local dof ownbership range
    dofmap._ownership_range = std::make_pair(0, dofmap.global_dimension());
  }
}
//-----------------------------------------------------------------------------
void DofMapBuilder::build_sub_map(DofMap& sub_dofmap,
                                  const DofMap& parent_dofmap,
                                  const std::vector<std::size_t>& component,
                                  const Mesh& mesh)
{
  // Note: Ownership range is set to zero since dofmap is a view

  dolfin_assert(!component.empty());

  // Collect global mesh entity dimensions in a vector
  std::vector<std::size_t> num_global_mesh_entities(mesh.topology().dim() + 1);
  for (std::size_t d = 0; d < num_global_mesh_entities.size(); d++)
    num_global_mesh_entities[d] = mesh.size_global(d);

  // Initialise offset from parent
  std::size_t offset = parent_dofmap._ufc_offset;

  // Get parent UFC dof map
  dolfin_assert(parent_dofmap._ufc_dofmap);
  const ufc::dofmap& parent_ufc_dofmap = *(parent_dofmap._ufc_dofmap);

  // Extract ufc sub-dofmap from parent and get offset
  sub_dofmap._ufc_dofmap = extract_ufc_sub_dofmap(parent_ufc_dofmap,
                                                  offset, component, mesh);
  dolfin_assert(sub_dofmap._ufc_dofmap);

  // Check for dimensional consistency between the dofmap and mesh
  //check_dimensional_consistency(*_ufc_dofmap, mesh);

  // Set UFC offset
  sub_dofmap._ufc_offset = offset;

  // Check dimensional consistency between UFC dofmap and the mesh
  //check_provided_entities(*_ufc_dofmap, mesh);

  // Resize dofmap data structure
  sub_dofmap._dofmap.resize(mesh.num_cells());

  // Holder for copying UFC std::size_t dof maps into the a dof map that
  // is consistent with the linear algebra backend
  std::vector<std::size_t> dof_holder;

  // Build sub-map based on UFC dofmap
  UFCCell ufc_cell(mesh);
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    const std::size_t cell_index = cell->index();

    // Update to current cell
    ufc_cell.update(*cell);

    // Resize list for cell
    sub_dofmap._dofmap[cell_index].resize(sub_dofmap._ufc_dofmap->local_dimension(ufc_cell));
    dof_holder.resize(sub_dofmap._ufc_dofmap->local_dimension(ufc_cell));

    // Tabulate sub-dofs on cell (using UFC map)
    sub_dofmap._ufc_dofmap->tabulate_dofs(&dof_holder[0],
                                          num_global_mesh_entities,
                                          ufc_cell);

    // Add UFC offset
    for (unsigned int i = 0; i < dof_holder.size(); i++)
      dof_holder[i] += offset;

    std::copy(dof_holder.begin(), dof_holder.end(),
              sub_dofmap._dofmap[cell_index].begin());
  }

  // Set global dimension
  sub_dofmap._global_dimension
    = sub_dofmap._ufc_dofmap->global_dimension(num_global_mesh_entities);

  // Modify dofmap for non-UFC numbering
  sub_dofmap.ufc_map_to_dofmap.clear();
  sub_dofmap._off_process_owner.clear();
  sub_dofmap._shared_dofs.clear();
  sub_dofmap._neighbours.clear();
  if (!parent_dofmap.ufc_map_to_dofmap.empty())
  {
    boost::unordered_map<std::size_t, std::size_t>::const_iterator ufc_to_current_dof;
    std::vector<std::vector<dolfin::la_index> >::iterator cell_map;
    std::vector<dolfin::la_index>::iterator dof;
    for (cell_map = sub_dofmap._dofmap.begin();
        cell_map != sub_dofmap._dofmap.end(); ++cell_map)
    {
      for (dof = cell_map->begin(); dof != cell_map->end(); ++dof)
      {
        // Get dof index
        ufc_to_current_dof = parent_dofmap.ufc_map_to_dofmap.find(*dof);
        dolfin_assert(ufc_to_current_dof != parent_dofmap.ufc_map_to_dofmap.end());

        // Add to ufc-to-current dof map
        sub_dofmap.ufc_map_to_dofmap.insert(*ufc_to_current_dof);

        // Set dof index
        *dof = ufc_to_current_dof->second;

        // Add to off-process dof owner map
        boost::unordered_map<std::size_t, std::size_t>::const_iterator
          parent_off_proc = parent_dofmap._off_process_owner.find(*dof);
        if (parent_off_proc != parent_dofmap._off_process_owner.end())
          sub_dofmap._off_process_owner.insert(*parent_off_proc);

        // Add to shared-dof process map, and update the set of neighbours
        boost::unordered_map<std::size_t, std::vector<std::size_t> >::const_iterator parent_shared
          = parent_dofmap._shared_dofs.find(*dof);
        if (parent_shared != parent_dofmap._shared_dofs.end())
        {
          sub_dofmap._shared_dofs.insert(*parent_shared);
          sub_dofmap._neighbours.insert(parent_shared->second.begin(), parent_shared->second.end());
        }
      }
    }
  }
}
//-----------------------------------------------------------------------------
std::size_t DofMapBuilder::build_constrained_vertex_indices(const Mesh& mesh,
 const std::map<std::size_t, std::pair<std::size_t, std::size_t> >& slave_to_master_vertices,
 std::vector<std::size_t>& modified_global_indices)
{
  // Mark shared vertices
  std::vector<bool> shared_vertex(mesh.num_vertices(), false);
  //const std::map<std::size_t, std::set<std::size_t> > shared_entities
  //  = mesh.topology().shared_entities(0);
  //std::map<std::size_t, std::set<std::size_t> >::const_iterator shared_entity;
  //for (shared_entity = shared_entities.begin(); shared_entity != shared_entities.end(); ++shared_entity)
  //{
  //  dolfin_assert(shared_entity->first < shared_vertex.size());
  //  shared_vertex[shared_entity->first] = true;
  //}

  // Must check if slave vertex is shared

  // Mark slave vertices
  std::vector<bool> slave_vertex(mesh.num_vertices(), false);
  std::map<std::size_t, std::pair<std::size_t, std::size_t> >::const_iterator slave;
  for (slave = slave_to_master_vertices.begin(); slave != slave_to_master_vertices.end(); ++slave)
  {
    dolfin_assert(slave->first < slave_vertex.size());
    slave_vertex[slave->first] = true;
  }

  // Compute modified global vertex indices
  std::size_t new_index = 0;
  modified_global_indices.resize(mesh.num_vertices());
  for (VertexIterator vertex(mesh); !vertex.end(); ++vertex)
  {
    const std::size_t local_index = vertex->index();
    if (slave_vertex[local_index])
    {
      // Slave, need to find master vertex index and need to communicate
      // to any sharing processes
    }
    else if (shared_vertex[local_index])
    {
      // Shared, decide if I should number this index or not
    }
    else
      modified_global_indices[vertex->index()] = new_index++;
  }

  // Send number of owned entities to compute offeset
  std::size_t offset = MPI::global_offset(new_index, true);

  // Add process offset
  for (std::size_t i = 0; i < modified_global_indices.size(); ++i)
    modified_global_indices[i] += offset;

  // Serial hack
  for (slave = slave_to_master_vertices.begin(); slave != slave_to_master_vertices.end(); ++slave)
  {
    dolfin_assert(slave->first < modified_global_indices.size());
    modified_global_indices[slave->first] = modified_global_indices[slave->second.second];
  }

  // Send new indices to process that share a vertex but were not
  // responsible for re-numbering
  return MPI::sum(new_index);
}
//-----------------------------------------------------------------------------
void DofMapBuilder::reorder_local(DofMap& dofmap, const Mesh& mesh)
{
  // Build graph
  Graph graph(dofmap.global_dimension());
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    const std::vector<dolfin::la_index>& dofs0 = dofmap.cell_dofs(cell->index());
    const std::vector<dolfin::la_index>& dofs1 = dofmap.cell_dofs(cell->index());
    std::vector<dolfin::la_index>::const_iterator node;
    for (node = dofs0.begin(); node != dofs0.end(); ++node)
      graph[*node].insert(dofs1.begin(), dofs1.end());
  }

  // Reorder graph (reverse Cuthill-McKee)
  const std::vector<std::size_t> dof_remap
      = BoostGraphOrdering::compute_cuthill_mckee(graph, true);

  // Store re-ordering map (from UFC dofmap)
  dolfin_assert(dofmap.ufc_map_to_dofmap.empty());
  for (std::size_t i = 0; i < dofmap.global_dimension(); ++i)
    dofmap.ufc_map_to_dofmap[i] = dof_remap[i];

  // Re-number dofs for each cell
  std::vector<std::vector<dolfin::la_index> >::iterator cell_map;
  std::vector<dolfin::la_index>::iterator dof;
  for (cell_map = dofmap._dofmap.begin(); cell_map != dofmap._dofmap.end(); ++cell_map)
    for (dof = cell_map->begin(); dof != cell_map->end(); ++dof)
      *dof = dof_remap[*dof];
}
//-----------------------------------------------------------------------------
void DofMapBuilder::build_ufc(DofMap& dofmap,
                             DofMapBuilder::map& restricted_dofs_inverse,
                             const Mesh& mesh,
                             boost::shared_ptr<const Restriction> restriction)
{
  // Start timer for dofmap initialization
  Timer t0("Init dofmap from UFC dofmap");

  // Sanity checks on UFC dofmap
  dolfin_assert(dofmap._ufc_dofmap);
  dolfin_assert(dofmap._ufc_dofmap->geometric_dimension() == mesh.geometry().dim());
  dolfin_assert(dofmap._ufc_dofmap->topological_dimension() == mesh.topology().dim());

  // Check for periodic constraints
  const bool periodic = MPI::sum(mesh.periodic_vertex_map.size()) > 0;

  // Global enity indices
  std::vector<std::vector<std::size_t> > global_entity_indices(mesh.topology().dim() + 1);

  // Generate and number required mesh entities. Mesh indices are modified
  // for periodic bcs
  const std::size_t D = mesh.topology().dim();
  std::vector<std::size_t> num_global_mesh_entities(mesh.topology().dim() + 1, 0);
  if (!periodic)
  {
    // Compute number of mesh entities
    for (std::size_t d = 0; d <= D; ++d)
    {
      if (dofmap._ufc_dofmap->needs_mesh_entities(d))
      {
        // Number entities globally
        DistributedMeshTools::number_entities(mesh, d);

        // Store entity indices
        global_entity_indices[d].resize(mesh.size(d));
        for (MeshEntityIterator e(mesh, d); !e.end(); ++e)
          global_entity_indices[d][e->index()] = e->global_index();

        // Store number of global entities
        num_global_mesh_entities[d] = mesh.size_global(d);
      }
    }
  }
  else
  {
    // Get master-slave vertex map
    const std::map<std::size_t, std::pair<std::size_t, std::size_t> >&
      slave_to_master_vertices = mesh.periodic_vertex_map;

    // Compute modified global vertex indices
    const std::size_t num_vertices = build_constrained_vertex_indices(mesh,
          slave_to_master_vertices, global_entity_indices[0]);

    cout << "Num vertices: " << num_vertices << ", " << mesh.num_vertices() << endl;
    cout << "Num slave vertices: " << slave_to_master_vertices.size() << endl;

    // Compute number of mesh entities
    num_global_mesh_entities[0] = num_vertices;
    for (std::size_t d = 1; d <= D; ++d)
    {
      if (dofmap._ufc_dofmap->needs_mesh_entities(d))
      {
        // Initialise local entities
        std::map<std::size_t, std::set<std::size_t> > shared_entities;
        const std::size_t num_entities
          = DistributedMeshTools::number_entities(mesh, global_entity_indices[0],
                                       global_entity_indices[d], shared_entities, d);

        num_global_mesh_entities[d] = num_entities;
      }
    }
  }

  // Allocate space for dof map
  dofmap._dofmap.resize(mesh.num_cells());
  dofmap._off_process_owner.clear();
  dolfin_assert(dofmap._ufc_dofmap);

  // Maps used to renumber dofs for restricted meshes
  map restricted_dofs;         // map from old to new dof

  // Holder for UFC 64-bit dofmap integers
  std::vector<std::size_t> ufc_dofs;

  // Build dofmap from ufc::dofmap
  UFCCell ufc_cell(mesh);
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    // Skip cells not included in restriction
    if (restriction && !restriction->contains(*cell))
      continue;

    // Update UFC cell data
    ufc_cell.orientation = cell->mesh().cell_orientations()[cell->index()];
    for (std::size_t d = 0; d < D; ++d)
    {
      if (!global_entity_indices[d].empty())
      {
        //cout << "Testing: " << d << ", " << global_entity_indices.size() << ", " << mesh.num_entities(d) << endl;
        for (std::size_t i = 0; i < ufc_cell.num_cell_entities[d]; ++i)
          ufc_cell.entity_indices[d][i] = global_entity_indices[d][cell->entities(d)[i]];
      }
    }

    // FIXME: Check the below two for local vs global
    ufc_cell.entity_indices[D][0] = cell->index();
    ufc_cell.index = cell->index();

    // Get standard local element dimension
    const std::size_t local_dim = dofmap._ufc_dofmap->local_dimension(ufc_cell);

    // Get container for cell dofs
    std::vector<dolfin::la_index>& cell_dofs = dofmap._dofmap[cell->index()];
    cell_dofs.resize(local_dim);

    // Tabulate standard UFC dof map
    ufc_dofs.resize(local_dim);
    dofmap._ufc_dofmap->tabulate_dofs(&ufc_dofs[0],
                                      num_global_mesh_entities, ufc_cell);
    std::copy(ufc_dofs.begin(), ufc_dofs.end(), cell_dofs.begin());

    // Renumber dofs if mesh is restricted
    if (restriction)
    {
      for (std::size_t i = 0; i < cell_dofs.size(); i++)
      {
        map_iterator it = restricted_dofs.find(cell_dofs[i]);
        if (it == restricted_dofs.end())
        {
          const std::size_t dof = restricted_dofs.size();
          restricted_dofs[cell_dofs[i]] = dof;
          restricted_dofs_inverse[dof] = cell_dofs[i];
          cell_dofs[i] = dof;
        }
        else
          cell_dofs[i] = it->second;
      }
    }
  }

  // Set global dimension
  if (restriction)
    dofmap._global_dimension = restricted_dofs.size();
  else
  {
    dofmap._global_dimension
      = dofmap._ufc_dofmap->global_dimension(num_global_mesh_entities);
  }

  //dofmap._ownership_range = std::make_pair(0, dofmap.global_dimension());
}
//-----------------------------------------------------------------------------
void DofMapBuilder::reorder_distributed(DofMap& dofmap,
                                      const Mesh& mesh,
                                      boost::shared_ptr<const Restriction> restriction,
                                      const map& restricted_dofs_inverse)
{
  // Build set of dofs that are not associated with a mesh entity
  // (global dofs)
  set global_dofs = compute_global_dofs(dofmap, mesh);

  // Allocate data structure to hold dof ownership
  boost::array<DofMapBuilder::set, 3> dof_ownership;

  // Allocate map data structure from a shared dof to the processes that
  // share it
  DofMapBuilder::vec_map shared_dof_processes;

  // Computed owned and shared dofs (and owned and un-owned)
  compute_dof_ownership(dof_ownership, shared_dof_processes, dofmap,
                        global_dofs, mesh, restriction,
                        restricted_dofs_inverse);

  // Renumber owned dofs and receive new numbering for unowned shared dofs
  parallel_renumber(dof_ownership, shared_dof_processes, dofmap, mesh,
                    restriction, restricted_dofs_inverse);
}
//-----------------------------------------------------------------------------
void DofMapBuilder::compute_dof_ownership(boost::array<set, 3>& dof_ownership,
                                      vec_map& shared_dof_processes,
                                      DofMap& dofmap,
                                      const DofMapBuilder::set& global_dofs,
                                      const Mesh& mesh,
                                      boost::shared_ptr<const Restriction> restriction,
                                      const map& restricted_dofs_inverse)
{
  log(TRACE, "Determining dof ownership for parallel dof map");

  // References to dof ownership sets
  set& owned_dofs          = dof_ownership[0];
  set& shared_owned_dofs   = dof_ownership[1];
  set& shared_unowned_dofs = dof_ownership[2];

  // Clear data structures
  owned_dofs.clear();
  shared_owned_dofs.clear();
  shared_unowned_dofs.clear();

  // Data structures for computing ownership
  boost::unordered_map<std::size_t, std::size_t> dof_vote;
  std::vector<std::size_t> facet_dofs(dofmap.num_facet_dofs());

  // Communication buffer
  std::vector<std::size_t> send_buffer;

  // Extract the interior boundary
  BoundaryMesh interior_boundary;
  interior_boundary.init_interior_boundary(mesh);

  // Create a random number generator for ownership 'voting'
  boost::mt19937 engine(MPI::process_number());
  boost::uniform_int<> distribution(0, 100000000);
  boost::variate_generator<boost::mt19937&, boost::uniform_int<> >
    rng(engine, distribution);

  // Build set of dofs on process boundary (first assuming that all are
  // owned by this process)
  const MeshFunction<std::size_t>& cell_map = interior_boundary.entity_map(interior_boundary.topology().dim());
  if (!cell_map.empty())
  {
    for (CellIterator _f(interior_boundary); !_f.end(); ++_f)
    {
      // Get boundary facet
      Facet f(mesh, cell_map[*_f]);

      // Get cell to which facet belongs (pick first)
      Cell c(mesh, f.entities(mesh.topology().dim())[0]);

      // Skip cells not included in restriction
      if (restriction && !restriction->contains(c))
        continue;

      // Tabulate dofs on cell
      const std::vector<dolfin::la_index>& cell_dofs = dofmap.cell_dofs(c.index());

      // Tabulate which dofs are on the facet
      dofmap.tabulate_facet_dofs(facet_dofs, c.index(f));

      // Insert shared dofs into set and assign a 'vote'
      for (std::size_t i = 0; i < dofmap.num_facet_dofs(); i++)
      {
        // Get facet dof
        size_t facet_dof = cell_dofs[facet_dofs[i]];

        // Map back to original (and common) numbering for restricted space
        if (restriction)
        {
          const map_iterator it = restricted_dofs_inverse.find(facet_dof);
          dolfin_assert(it != restricted_dofs_inverse.end());
          facet_dof = it->second;
        }

        // Add to list of shared dofs
        if (shared_owned_dofs.find(facet_dof) == shared_owned_dofs.end())
        {
          shared_owned_dofs.insert(facet_dof);
          dof_vote[facet_dof] = rng();

          send_buffer.push_back(facet_dof);
          send_buffer.push_back(dof_vote[facet_dof]);
        }
      }
    }
  }

  // FIXME: The below algortihm can be improved (made more scalable)
  //        by distributing (dof, process) pairs to 'owner' range owner,
  //        then letting each process get the sharing process list. This
  //        will avoid interleaving communication and computation.

  // Decide ownership of shared dofs
  const std::size_t num_prococesses = MPI::num_processes();
  const std::size_t process_number = MPI::process_number();
  std::vector<std::size_t> recv_buffer;
  for (std::size_t k = 1; k < num_prococesses; ++k)
  {
    const std::size_t src  = (process_number - k + num_prococesses) % num_prococesses;
    const std::size_t dest = (process_number + k) % num_prococesses;
    MPI::send_recv(send_buffer, dest, recv_buffer, src);

    for (std::size_t i = 0; i < recv_buffer.size(); i += 2)
    {
      const std::size_t received_dof  = recv_buffer[i];
      const std::size_t received_vote = recv_buffer[i + 1];

      if (shared_owned_dofs.find(received_dof) != shared_owned_dofs.end())
      {
        // Move dofs with higher ownership votes from shared to shared
        // but not owned
        if (received_vote < dof_vote[received_dof])
        {
          shared_unowned_dofs.insert(received_dof);
          shared_owned_dofs.erase(received_dof);
        }
        else if (received_vote == dof_vote[received_dof] && process_number > src)
        {
          // If votes are equal, let lower rank process take ownership
          shared_unowned_dofs.insert(received_dof);
          shared_owned_dofs.erase(received_dof);
        }

        // Remember the sharing of the dof
        shared_dof_processes[received_dof].push_back(src);
      }
      else if (shared_unowned_dofs.find(received_dof) != shared_unowned_dofs.end())
      {
        // Remember the sharing of the dof
        shared_dof_processes[received_dof].push_back(src);
      }
    }
  }

  // Add/remove global dofs to/from relevant sets (process 0 owns global dofs)
  if (process_number == 0)
  {
    shared_owned_dofs.insert(global_dofs.begin(), global_dofs.end());
    for (set::const_iterator dof = global_dofs.begin(); dof != global_dofs.begin(); ++dof)
    {
      set::const_iterator _dof = shared_unowned_dofs.find(*dof);
      if (_dof != shared_unowned_dofs.end())
        shared_unowned_dofs.erase(_dof);
    }
  }
  else
  {
    shared_unowned_dofs.insert(global_dofs.begin(), global_dofs.end());
    for (set::const_iterator dof = global_dofs.begin(); dof != global_dofs.begin(); ++dof)
    {
      set::const_iterator _dof = shared_owned_dofs.find(*dof);
      if (_dof != shared_owned_dofs.end())
        shared_owned_dofs.erase(_dof);
    }
  }

  // Mark all shared and owned dofs as owned by the processes
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    const std::vector<dolfin::la_index>& cell_dofs = dofmap.cell_dofs(cell->index());
    const std::size_t cell_dimension = dofmap.cell_dimension(cell->index());
    for (std::size_t i = 0; i < cell_dimension; ++i)
    {
      // Get cell dof
      size_t cell_dof = cell_dofs[i];

      // Map back to original (and common) numbering for restricted space
      if (restriction)
      {
        const map_iterator it = restricted_dofs_inverse.find(cell_dof);
        dolfin_assert(it != restricted_dofs_inverse.end());
        cell_dof = it->second;
      }

      // Mark dof as owned if not in unowned set
      if (shared_unowned_dofs.find(cell_dof) == shared_unowned_dofs.end())
        owned_dofs.insert(cell_dof);
    }
  }

  // Check or set global dimension
  if (restriction)
  {
    // Global dimension for restricted space needs to be computed here
    // since it is not know by the UFC dof map.
    const std::size_t _owned_dim = owned_dofs.size();
    const std::size_t _global_dimension = MPI::sum(_owned_dim);
    dofmap._global_dimension = _global_dimension;
  }
  else
  {
    const std::size_t _owned_dim = owned_dofs.size();
    dolfin_assert(MPI::sum(_owned_dim) == dofmap.global_dimension());
  }

  log(TRACE, "Finished determining dof ownership for parallel dof map");
}
//-----------------------------------------------------------------------------
void DofMapBuilder::parallel_renumber(const boost::array<set, 3>& dof_ownership,
                                      const vec_map& shared_dof_processes,
                                      DofMap& dofmap,
                                      const Mesh& mesh,
                                      boost::shared_ptr<const Restriction> restriction,
                                      const map& restricted_dofs_inverse)
{
  log(TRACE, "Renumber dofs for parallel dof map");

  // References to dof ownership sets
  const set& owned_dofs          = dof_ownership[0];
  const set& shared_owned_dofs   = dof_ownership[1];
  const set& shared_unowned_dofs = dof_ownership[2];


  // FIXME: Handle double-renumbered dof map
  if (!dofmap.ufc_map_to_dofmap.empty())
  {
    dolfin_error("DofMapBuilder.cpp",
                 "compute parallel renumbering of degrees of freedom",
                 "The degree of freedom mapping cannot (yet) be renumbered twice");
  }

  const std::vector<std::vector<dolfin::la_index> >& old_dofmap = dofmap._dofmap;
  std::vector<std::vector<dolfin::la_index> > new_dofmap(old_dofmap.size());
  dolfin_assert(old_dofmap.size() == mesh.num_cells());

  // Compute offset for owned and non-shared dofs
  const std::size_t process_offset = MPI::global_offset(owned_dofs.size(), true);

  // Clear some data
  dofmap._off_process_owner.clear();

  // Create graph
  Graph graph(owned_dofs.size());

  // Build graph for re-ordering. Below block is scoped to clear working
  // data structures once graph is constructed.
  {
    // Create contiguous local numbering for locally owned dofs
    std::size_t my_counter = 0;
    boost::unordered_map<std::size_t, std::size_t> my_old_to_new_dof_index;
    for (set_iterator owned_dof = owned_dofs.begin(); owned_dof != owned_dofs.end(); ++owned_dof, my_counter++)
      my_old_to_new_dof_index[*owned_dof] = my_counter;

    // Build local graph based on old dof map with contiguous numbering
    for (std::size_t cell = 0; cell < old_dofmap.size(); ++cell)
    {
      const std::vector<dolfin::la_index>& dofs0 = dofmap.cell_dofs(cell);
      const std::vector<dolfin::la_index>& dofs1 = dofmap.cell_dofs(cell);
      std::vector<dolfin::la_index>::const_iterator node0, node1;
      for (node0 = dofs0.begin(); node0 != dofs0.end(); ++node0)
      {
        boost::unordered_map<std::size_t, std::size_t>::const_iterator _node0
            = my_old_to_new_dof_index.find(*node0);
        if (_node0 != my_old_to_new_dof_index.end())
        {
          const std::size_t local_node0 = _node0->second;
          dolfin_assert(local_node0 < graph.size());
          for (node1 = dofs1.begin(); node1 != dofs1.end(); ++node1)
          {
            boost::unordered_map<std::size_t, std::size_t>::const_iterator
                  _node1 = my_old_to_new_dof_index.find(*node1);
            if (_node1 != my_old_to_new_dof_index.end())
            {
              const std::size_t local_node1 = _node1->second;
              graph[local_node0].insert(local_node1);
            }
          }
        }
      }
    }
  }

  // Reorder dofs locally
  const std::vector<std::size_t> dof_remap
      = BoostGraphOrdering::compute_cuthill_mckee(graph, true);

  // Map from old to new index for dofs
  boost::unordered_map<std::size_t, std::size_t> old_to_new_dof_index;

  // Renumber owned dofs and buffer dofs that are owned but shared with
  // another process
  std::size_t counter = 0;
  std::vector<std::size_t> send_buffer;
  for (set_iterator owned_dof = owned_dofs.begin();
          owned_dof != owned_dofs.end(); ++owned_dof, counter++)
  {
    // Set new dof number
    old_to_new_dof_index[*owned_dof] = process_offset + dof_remap[counter];

    // Update UFC-to-renumbered map for new number
    dofmap.ufc_map_to_dofmap[*owned_dof] = process_offset + dof_remap[counter];

    // If this dof is shared and owned, buffer old and new index for sending
    if (shared_owned_dofs.find(*owned_dof) != shared_owned_dofs.end())
    {
      send_buffer.push_back(*owned_dof);
      send_buffer.push_back(process_offset + dof_remap[counter]);
    }
  }

  // FIXME: The below algortihm can be improved (made more scalable)
  //        by distributing (dof, process) pairs to 'owner' range owner,
  //        then letting each process get the sharing process list. This
  //        will avoid interleaving communication and computation.

  // Exchange new dof numbers for dofs that are shared
  const std::size_t num_processes = MPI::num_processes();
  const std::size_t process_number = MPI::process_number();
  std::vector<std::size_t> recv_buffer;
  for (std::size_t k = 1; k < num_processes; ++k)
  {
    const std::size_t src  = (process_number - k + num_processes) % num_processes;
    const std::size_t dest = (process_number + k) % num_processes;
    MPI::send_recv(send_buffer, dest, recv_buffer, src);

    // Add dofs renumbered by another process to the old-to-new map
    for (std::size_t i = 0; i < recv_buffer.size(); i += 2)
    {
      const std::size_t received_old_dof_index = recv_buffer[i];
      const std::size_t received_new_dof_index = recv_buffer[i + 1];

      // Check if this process has shared dof (and is not the owner)
      if (shared_unowned_dofs.find(received_old_dof_index) != shared_unowned_dofs.end())
      {
        // Add to old-to-new dof map
        old_to_new_dof_index[received_old_dof_index] = received_new_dof_index;

        // Store map from off-process dof to owner
        dofmap._off_process_owner[received_new_dof_index] = src;

        // Update UFC-to-renumbered map
        dofmap.ufc_map_to_dofmap[received_old_dof_index] = received_new_dof_index;
      }
    }
  }

  // Insert the shared-dof-to-process mapping into the dofmap, renumbering
  // as necessary
  for (vec_map::const_iterator it = shared_dof_processes.begin();
            it != shared_dof_processes.end(); ++it)
  {
    boost::unordered_map<std::size_t, std::size_t>::const_iterator
      new_index = old_to_new_dof_index.find(it->first);
    if (new_index == old_to_new_dof_index.end())
      dofmap._shared_dofs.insert(*it);
    else
      dofmap._shared_dofs.insert(std::make_pair(new_index->second, it->second));
    dofmap._neighbours.insert(it->second.begin(), it->second.end());
  }

  // Build new dofmap
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    // Skip cells not included in restriction
    if (restriction && !restriction->contains(*cell))
      continue;

    // Get cell index and dimension
    const std::size_t cell_index = cell->index();
    const std::size_t cell_dimension = dofmap.cell_dimension(cell_index);

    // Resize cell map and insert dofs
    new_dofmap[cell_index].resize(cell_dimension);
    for (std::size_t i = 0; i < cell_dimension; ++i)
    {
      // Get old dof
      std::size_t old_index = old_dofmap[cell_index][i];

      // Map back to original (and common) numbering for restricted space
      if (restriction)
      {
        const map_iterator it = restricted_dofs_inverse.find(old_index);
        dolfin_assert(it != restricted_dofs_inverse.end());
        old_index = it->second;
      }

      // Insert dof
      new_dofmap[cell_index][i] = old_to_new_dof_index[old_index];
    }
  }

  // Set new dof map
  dofmap._dofmap = new_dofmap;

  // Set ownership range
  dofmap._ownership_range = std::make_pair(process_offset,
                                          process_offset + owned_dofs.size());

  log(TRACE, "Finished renumbering dofs for parallel dof map");
}
//-----------------------------------------------------------------------------
DofMapBuilder::set DofMapBuilder::compute_global_dofs(const DofMap& dofmap,
                                                      const Mesh& mesh)
{
  // Wrap UFC dof map
  boost::shared_ptr<const ufc::dofmap> _dofmap(dofmap._ufc_dofmap.get(),
                                               NoDeleter());

  // Compute global dof indices
  std::size_t offset = 0;
  set global_dof_indices;
  compute_global_dofs(global_dof_indices, offset, _dofmap, mesh);

  return global_dof_indices;
}
//-----------------------------------------------------------------------------
void DofMapBuilder::compute_global_dofs(DofMapBuilder::set& global_dofs,
                                        std::size_t& offset,
                                        boost::shared_ptr<const ufc::dofmap> dofmap,
                                        const Mesh& mesh)
{
  dolfin_assert(dofmap);
  const std::size_t D = mesh.topology().dim();

  // Store global entity dimensions in vector
  std::vector<std::size_t> num_global_mesh_entities(mesh.topology().dim() + 1);
  for (std::size_t d = 0; d < num_global_mesh_entities.size(); d++)
    num_global_mesh_entities[d] = mesh.size_global(d);

  if (dofmap->num_sub_dofmaps() == 0)
  {
    // Check if dofmap is for global dofs
    bool global_dof = true;
    for (std::size_t d = 0; d <= D; ++d)
    {
      if (dofmap->needs_mesh_entities(d))
      {
        global_dof = false;
        break;
      }
    }

    if (global_dof)
    {
      // Check that we have just one dof
      if (dofmap->global_dimension(num_global_mesh_entities) != 1)
      {
        dolfin_error("DofMapBuilder.cpp",
                     "compute global degrees of freedom",
                     "Global degree of freedom has dimension != 1");
      }

      // Create dummy cell argument to tabulate single global dof
      boost::scoped_ptr<ufc::cell> ufc_cell(new ufc::cell);
      std::size_t dof = 0;
      dofmap->tabulate_dofs(&dof, num_global_mesh_entities, *ufc_cell);

      // Insert global dof index
      std::pair<DofMapBuilder::set::iterator, bool> ret = global_dofs.insert(dof + offset);
      if (!ret.second)
      {
        dolfin_error("DofMapBuilder.cpp",
                     "compute global degrees of freedom",
                     "Global degree of freedom already exists");
      }
    }
  }
  else
  {
    // Loop through sub-dofmao looking for globak dofs
    for (std::size_t i = 0; i < dofmap->num_sub_dofmaps(); ++i)
    {
      // Extract sub-dofmap and intialise
      boost::shared_ptr<ufc::dofmap> sub_dofmap(dofmap->create_sub_dofmap(i));
      DofMap::check_provided_entities(*sub_dofmap, mesh);

      compute_global_dofs(global_dofs, offset, sub_dofmap, mesh);

      // Get offset
      if (sub_dofmap->num_sub_dofmaps() == 0)
        offset += sub_dofmap->global_dimension(num_global_mesh_entities);
    }
  }
}
//-----------------------------------------------------------------------------
boost::shared_ptr<ufc::dofmap>
    DofMapBuilder::extract_ufc_sub_dofmap(const ufc::dofmap& ufc_dofmap,
                                          std::size_t& offset,
                                          const std::vector<std::size_t>& component,
                                          const Mesh& mesh)
{
  // Check if there are any sub systems
  if (ufc_dofmap.num_sub_dofmaps() == 0)
  {
    dolfin_error("DofMap.cpp",
                 "extract subsystem of degree of freedom mapping",
                 "There are no subsystems");
  }

  // Check that a sub system has been specified
  if (component.empty())
  {
    dolfin_error("DofMap.cpp",
                 "extract subsystem of degree of freedom mapping",
                 "No system was specified");
  }

  // Check the number of available sub systems
  if (component[0] >= ufc_dofmap.num_sub_dofmaps())
  {
    dolfin_error("DofMap.cpp",
                 "extract subsystem of degree of freedom mapping",
                 "Requested subsystem (%d) out of range [0, %d)",
                 component[0], ufc_dofmap.num_sub_dofmaps());
  }

  // Store global entity dimensions in vector
  std::vector<std::size_t> num_global_mesh_entities(mesh.topology().dim() + 1);
  for (std::size_t d = 0; d < num_global_mesh_entities.size(); d++)
    num_global_mesh_entities[d] = mesh.size_global(d);

  // Add to offset if necessary
  for (std::size_t i = 0; i < component[0]; i++)
  {
    // Extract sub dofmap
    boost::scoped_ptr<ufc::dofmap> ufc_tmp_dofmap(ufc_dofmap.create_sub_dofmap(i));
    dolfin_assert(ufc_tmp_dofmap);

    // Check dimensional consistency between UFC dofmap and the mesh
    //check_dimensional_consistency(ufc_dofmap, mesh);

    // Get offset
    offset += ufc_tmp_dofmap->global_dimension(num_global_mesh_entities);
  }

  // Create UFC sub-system
  boost::shared_ptr<ufc::dofmap> sub_dofmap(ufc_dofmap.create_sub_dofmap(component[0]));
  dolfin_assert(sub_dofmap);

  // Return sub-system if sub-sub-system should not be extracted,
  // otherwise recursively extract the sub sub system
  if (component.size() == 1)
    return sub_dofmap;
  else
  {
    std::vector<std::size_t> sub_component;
    for (std::size_t i = 1; i < component.size(); ++i)
      sub_component.push_back(component[i]);

    boost::shared_ptr<ufc::dofmap> sub_sub_dofmap
        = extract_ufc_sub_dofmap(*sub_dofmap, offset, sub_component, mesh);

    return sub_sub_dofmap;
  }
}
//-----------------------------------------------------------------------------

// Copyright (C) 2008-2014 Niclas Jansson, Ola Skavhaug, Anders Logg
// Garth N. Wells and Chris Richardson
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
// Modified by Kent-Andre Mardal 2011
// Modified by Anders Logg 2011
// Modified by Garth N. Wells 2011-2012
// Modified by Chris Richardson 2013-2014
//

#include <algorithm>
#include <iterator>
#include <map>
#include <numeric>
#include <set>
#include <boost/multi_array.hpp>

#include <dolfin/log/log.h>
#include <dolfin/common/MPI.h>
#include <dolfin/common/Timer.h>
#include <dolfin/geometry/Point.h>
#include <dolfin/graph/GraphBuilder.h>
#include <dolfin/graph/ParMETIS.h>
#include <dolfin/graph/SCOTCH.h>
#include <dolfin/graph/ZoltanPartition.h>
#include <dolfin/parameter/GlobalParameters.h>

#include "DistributedMeshTools.h"
#include "Facet.h"
#include "LocalMeshData.h"
#include "Mesh.h"
#include "MeshEditor.h"
#include "MeshEntity.h"
#include "MeshEntityIterator.h"
#include "MeshFunction.h"
#include "MeshTopology.h"
#include "MeshValueCollection.h"
#include "Vertex.h"
#include "MeshPartitioning.h"

using namespace dolfin;

// Explicitly instantiate some templated functions to help the Python
// wrappers
template void MeshPartitioning::build_mesh_value_collection(const Mesh& mesh,
   const std::vector<std::pair<std::pair<std::size_t, std::size_t>, std::size_t>>&
                                                            local_value_data,
   MeshValueCollection<std::size_t>& mesh_values);

template void MeshPartitioning::build_mesh_value_collection(const Mesh& mesh,
   const std::vector<std::pair<std::pair<std::size_t, std::size_t>, int>>&
                                                            local_value_data,
   MeshValueCollection<int>& mesh_values);

template void MeshPartitioning::build_mesh_value_collection(const Mesh& mesh,
   const std::vector<std::pair<std::pair<std::size_t, std::size_t>, double>>&
                                                            local_value_data,
   MeshValueCollection<double>& mesh_values);

template void MeshPartitioning::build_mesh_value_collection(const Mesh& mesh,
   const std::vector<std::pair<std::pair<std::size_t, std::size_t>, bool>>&
                                                            local_value_data,
                                     MeshValueCollection<bool>& mesh_values);

//-----------------------------------------------------------------------------
void MeshPartitioning::build_distributed_mesh(Mesh& mesh)
{
  if (MPI::size(mesh.mpi_comm()) > 1)
  {
    // Create and distribute local mesh data
    LocalMeshData local_mesh_data(mesh);

    // Build distributed mesh
    build_distributed_mesh(mesh, local_mesh_data);
  }
}
//-----------------------------------------------------------------------------
void MeshPartitioning::build_distributed_mesh(Mesh& mesh,
                            const std::vector<std::size_t>& cell_destinations)
{
  if (MPI::size(mesh.mpi_comm()) > 1)
  {
    // Create and distribute local mesh data
    LocalMeshData local_mesh_data(mesh);

    // Attach cell destinations
    local_mesh_data.cell_partition = cell_destinations;

    // Build distributed mesh
    build_distributed_mesh(mesh, local_mesh_data);
  }
}
//-----------------------------------------------------------------------------
void MeshPartitioning::build_distributed_mesh(Mesh& mesh,
                                              const LocalMeshData& local_data)
{
  // Compute cell partitioning or use partitioning provided in local_data
  std::vector<std::size_t> cell_partition;
  std::map<std::size_t, dolfin::Set<unsigned int>> ghost_procs;
  if (local_data.cell_partition.empty())
    partition_cells(mesh.mpi_comm(), local_data, cell_partition, ghost_procs);
  else
  {
    cell_partition = local_data.cell_partition;
    dolfin_assert(cell_partition.size()
                  == local_data.global_cell_indices.size());
    dolfin_assert(*std::max_element(cell_partition.begin(),
                                    cell_partition.end())
                  < MPI::size(mesh.mpi_comm()));
  }

  const std::string ghost_mode = parameters["ghost_mode"];

  if (ghost_procs.empty() && ghost_mode != "none")
  {
    // FIXME: need to generate ghost cell information here
    // by doing a facet-matching operation "GraphBuilder" style
    dolfin_error("MeshPartitioning.cpp",
                 "build ghost mesh",
                 "Ghost cell information not available");
  }

  // Build mesh from local mesh data and provided cell partition
  build(mesh, local_data, cell_partition, ghost_procs);

  // Create MeshDomains from local_data
  // FIXME: probably not working with ghost cells?
  build_mesh_domains(mesh, local_data);

  // Initialise number of globally connected cells to each facet. This is
  // necessary to distinguish between facets on an exterior boundary and
  // facets on a partition boundary (see
  // https://bugs.launchpad.net/dolfin/+bug/733834).
  DistributedMeshTools::init_facet_cell_connections(mesh);
}
//-----------------------------------------------------------------------------
void MeshPartitioning::partition_cells(
  const MPI_Comm& mpi_comm,
  const LocalMeshData& mesh_data,
  std::vector<std::size_t>& cell_partition,
  std::map<std::size_t, dolfin::Set<unsigned int>>& ghost_procs)
{

  // Compute cell partition using partitioner from parameter system
  const std::string partitioner = parameters["mesh_partitioner"];
  if (partitioner == "SCOTCH")
    SCOTCH::compute_partition(mpi_comm, cell_partition, ghost_procs, mesh_data);
  else if (partitioner == "ParMETIS")
  {
    ParMETIS::compute_partition(mpi_comm, cell_partition, ghost_procs,
                                mesh_data);
  }
  else if (partitioner == "Zoltan_RCB")
  {
    ZoltanPartition::compute_partition_rcb(mpi_comm, cell_partition,
                                           mesh_data);
  }
  else if (partitioner == "Zoltan_PHG")
  {
    ZoltanPartition::compute_partition_phg(mpi_comm, cell_partition,
                                           mesh_data);
  }
  else
  {
    dolfin_error("MeshPartitioning.cpp",
                 "compute cell partition",
                 "Mesh partitioner '%s' is unknown.", partitioner.c_str());
  }
}
//-----------------------------------------------------------------------------
void MeshPartitioning::build(Mesh& mesh, const LocalMeshData& mesh_data,
     const std::vector<std::size_t>& cell_partition,
     const std::map<std::size_t, dolfin::Set<unsigned int>>& ghost_procs)
{
  // Distribute cells
  Timer timer("PARALLEL 2: Distribute mesh (cells and vertices)");

  // Structure to hold received data about local mesh
  LocalMeshData new_mesh_data(mesh.mpi_comm());

  // Copy over some basic information
  new_mesh_data.tdim = mesh_data.tdim;
  new_mesh_data.gdim = mesh_data.gdim;
  new_mesh_data.num_global_cells = mesh_data.num_global_cells;
  new_mesh_data.num_vertices_per_cell
    = mesh_data.num_vertices_per_cell;

  new_mesh_data.num_global_vertices = mesh_data.num_global_vertices;

  // Keep tabs on ghost cell ownership
  std::map<unsigned int, std::set<unsigned int>> shared_cells;
  // Send cells to processes that need them
  const unsigned int num_regular_cells =
    distribute_cells(mesh.mpi_comm(), mesh_data,
                     cell_partition, ghost_procs,
                     shared_cells, new_mesh_data);

  const std::string ghost_mode = parameters["ghost_mode"];

  if (ghost_mode == "shared_vertex")
  {
    // Send/receive additional cells
    // defined by connectivity to the shared vertices.
    // Add new cells to new_mesh_data
    distribute_cell_layer(mesh.mpi_comm(),
                          num_regular_cells,
                          shared_cells,
                          new_mesh_data);
  }
  else if (ghost_mode == "none")
  {
    // Resize to remove all ghost cells
    new_mesh_data.cell_partition.resize(num_regular_cells);
    new_mesh_data.global_cell_indices.resize(num_regular_cells);
    const std::size_t num_cell_vertices = mesh_data.num_vertices_per_cell;
    new_mesh_data.cell_vertices.resize
      (boost::extents[num_regular_cells][num_cell_vertices]);
    shared_cells.clear();
  }

#ifdef HAS_SCOTCH
  if (parameters["reorder_cells_gps"])
    reorder_cells_gps(mesh.mpi_comm(), num_regular_cells,
                      shared_cells, new_mesh_data);
#endif

  // Generate mapping from global to local indexing for vertices
  // also calculating which vertices are 'ghost' and putting them
  // at the end of the local range
  std::map<std::size_t, std::size_t> vertex_global_to_local;
  const std::size_t num_regular_vertices
    = compute_vertex_mapping(mesh.mpi_comm(),
                             num_regular_cells, new_mesh_data,
                             vertex_global_to_local);

#ifdef HAS_SCOTCH
  if (parameters["reorder_vertices_gps"])
    reorder_vertices_gps(mesh.mpi_comm(), num_regular_vertices,
                         num_regular_cells, vertex_global_to_local,
                         new_mesh_data);
#endif

  // Send vertices to processes that need them, informing all
  // sharing processes of their destinations
  std::map<unsigned int, std::set<unsigned int>> shared_vertices;
  distribute_vertices(mesh.mpi_comm(), mesh_data, new_mesh_data,
                      vertex_global_to_local, shared_vertices);
  timer.stop();

  // Build mesh from new_mesh_data
  build_mesh(mesh, vertex_global_to_local, new_mesh_data);

  // Fix up some of the ancilliary data about sharing and ownership
  // now that the mesh has been initialised

  // Copy cell ownership
  std::vector<unsigned int>& cell_owner = mesh.topology().cell_owner();
  cell_owner.clear();
  cell_owner.insert(cell_owner.begin(),
                    new_mesh_data.cell_partition.begin() + num_regular_cells,
                    new_mesh_data.cell_partition.end());

  // Set the ghost cell offset
  mesh.topology().init_ghost(mesh_data.tdim, num_regular_cells);

  // Set the ghost vertex offset
  mesh.topology().init_ghost(0, num_regular_vertices);

  // Assign map of shared cells and vertices
  mesh.topology().shared_entities(mesh_data.tdim) = shared_cells;
  mesh.topology().shared_entities(0) = shared_vertices;
}
//-----------------------------------------------------------------------------
void MeshPartitioning::reorder_cells_gps(
  MPI_Comm mpi_comm,
  unsigned int num_regular_cells,
  std::map<unsigned int, std::set<unsigned int>>& shared_cells,
  LocalMeshData& new_mesh_data)
{
  Timer t("Reorder cells GPS");

  // Make dual graph from vertex indices, using GraphBuilder
  // FIXME: this should be reused later to add the facet-cell topology
  std::vector<std::set<std::size_t>> local_graph;
  GraphBuilder::FacetCellMap facet_cell_map;
  GraphBuilder::compute_local_dual_graph(mpi_comm,
                                         new_mesh_data,
                                         local_graph,
                                         facet_cell_map);
  const std::size_t num_all_cells
    = new_mesh_data.cell_vertices.shape()[0];

  const std::size_t local_cell_offset
    = MPI::global_offset(mpi_comm, num_all_cells, true);

  // Convert between graph types, removing offset
  // FIXME: make all graphs the same type
  Graph g_dual;
  // Ignore the ghost cells - they will not be reordered
  // FIXME: reorder ghost cells too
  for (unsigned int i = 0; i != num_regular_cells; ++i)
  {
    dolfin::Set<int> conn_set;
    for (auto q = local_graph[i].begin();
         q != local_graph[i].end(); ++q)
    {
      dolfin_assert(*q >= local_cell_offset);
      const int local_index = *q - local_cell_offset;
      // Ignore ghost cells in connectivity
      if (local_index < (int)num_regular_cells)
        conn_set.insert(local_index);
    }
    g_dual.push_back(conn_set);
  }
  std::vector<int> remap = SCOTCH::compute_gps(g_dual);

  boost::multi_array<std::size_t, 2>
    remapped_cell_vertices(new_mesh_data.cell_vertices);
  std::vector<std::size_t>
    remapped_global_cell_indices(new_mesh_data.global_cell_indices);

  for (unsigned int i = 0; i != g_dual.size(); ++i)
  {
    // Remap data
    const unsigned int j = remap[i];
    remapped_cell_vertices[j] = new_mesh_data.cell_vertices[i];
    remapped_global_cell_indices[j] = new_mesh_data.global_cell_indices[i];
  }

  std::map<unsigned int, std::set<unsigned int>> remapped_shared_cells;
  for (auto p = shared_cells.begin(); p != shared_cells.end(); ++p)
  {
    const unsigned int cell_index = p->first;
    if (cell_index < num_regular_cells)
      remapped_shared_cells.insert(std::make_pair
                                   (remap[cell_index], p->second));
    else
      remapped_shared_cells.insert(*p);
  }

  // Assign
  new_mesh_data.cell_vertices = remapped_cell_vertices;
  new_mesh_data.global_cell_indices = remapped_global_cell_indices;
  shared_cells = remapped_shared_cells;
}
//-----------------------------------------------------------------------------
void MeshPartitioning::reorder_vertices_gps(MPI_Comm mpi_comm,
     unsigned int num_regular_vertices,
     unsigned int num_regular_cells,
     std::map<std::size_t, std::size_t>& vertex_global_to_local,
     LocalMeshData& new_mesh_data)
{
  Timer t("Reorder vertices GPS");

  const unsigned int num_cell_vertices
    = new_mesh_data.num_vertices_per_cell;

  // Make local real graph (vertices are nodes, edges are edges)
  Graph g(num_regular_vertices);
  for (unsigned int i = 0; i != num_regular_cells; ++i)
  {
    for (unsigned int j = 0; j != num_cell_vertices; ++j)
    {
      const unsigned int vj
        = vertex_global_to_local[new_mesh_data.cell_vertices[i][j]];
      if (vj < num_regular_vertices)
      {
        for (unsigned int k = j + 1; k != num_cell_vertices; ++k)
        {
          const unsigned int vk
            = vertex_global_to_local[new_mesh_data.cell_vertices[i][k]];
          if (vk < num_regular_vertices)
          {
            g[vj].insert(vk);
            g[vk].insert(vj);
          }
        }
      }
    }
  }

  std::vector<int> remap = SCOTCH::compute_gps(g);

  // Remap global-to-local mapping
  for (auto p = vertex_global_to_local.begin();
       p != vertex_global_to_local.end(); ++p)
    if (p->second < num_regular_vertices)
      p->second = (std::size_t)remap[p->second];

  // Remap local-to-global mapping
  std::vector<std::size_t>
    remapped_vertex_indices(new_mesh_data.vertex_indices);
  for (unsigned int i = 0; i != num_regular_vertices; ++i)
  {
    const unsigned int j = remap[i];
    remapped_vertex_indices[j] = new_mesh_data.vertex_indices[i];
  }

  // Assign
  new_mesh_data.vertex_indices = remapped_vertex_indices;
}
//-----------------------------------------------------------------------------
void MeshPartitioning::distribute_cell_layer(MPI_Comm mpi_comm,
  unsigned int num_regular_cells,
  std::map<unsigned int, std::set<unsigned int>>& shared_cells,
  LocalMeshData& new_mesh_data)
{
  Timer timer("Distribute cell layer");

  const unsigned int mpi_size = MPI::size(mpi_comm);
  const unsigned int mpi_rank = MPI::rank(mpi_comm);

  boost::multi_array<std::size_t, 2>& cell_vertices
    = new_mesh_data.cell_vertices;

  // Get set of vertices in ghost cells
  std::map<std::size_t, std::vector<std::size_t>> sh_vert_to_cell;
  // Make global-to-local map of shared cells
  std::map<std::size_t, unsigned int> cell_global_to_local;
  for (unsigned int i = num_regular_cells; i != cell_vertices.size(); ++i)
  {
    // Add map entry for each vertex
    for(auto p = cell_vertices[i].begin(); p != cell_vertices[i].end(); ++p)
      sh_vert_to_cell.insert(std::make_pair(*p, std::vector<std::size_t>()));

    cell_global_to_local.insert(std::make_pair
                                (new_mesh_data.global_cell_indices[i], i));
  }

  // Reduce vertex set to those which also appear in local cells
  // giving the effective boundary vertices.
  // Make a map from these vertices to the set of connected cells
  // (but only adding locally owned cells)

  // Go through all regular cells
  // to add any previously unshared cells.
  for (unsigned int i = 0; i != num_regular_cells; ++i)
  {
    for (auto v = cell_vertices[i].begin();
         v != cell_vertices[i].end(); ++v)
    {
      auto vc_it = sh_vert_to_cell.find(*v);
      if (vc_it != sh_vert_to_cell.end())
      {
        cell_global_to_local.insert(std::make_pair
                                    (new_mesh_data.global_cell_indices[i], i));
        vc_it->second.push_back(i);
      }
    }
  }

  // Send lists of cells/owners to MPI::index_owner of vertex,
  // collating and sending back out...
  std::vector<std::vector<std::size_t>> send_vertcells(mpi_size);
  std::vector<std::vector<std::size_t>> recv_vertcells(mpi_size);
  for (auto vc_it = sh_vert_to_cell.begin();
       vc_it != sh_vert_to_cell.end(); ++vc_it)
  {
    const std::size_t dest = MPI::index_owner(mpi_comm,
                                              vc_it->first,
                                              new_mesh_data.num_global_vertices);

    std::vector<std::size_t>& sendv = send_vertcells[dest];

    // Pack as [cell_global_index, this_vertex, [other_vertices]]
    for (auto q = vc_it->second.begin(); q != vc_it->second.end(); ++q)
    {
      sendv.push_back(new_mesh_data.global_cell_indices[*q]);
      sendv.push_back(vc_it->first);
      for (auto v = new_mesh_data.cell_vertices[*q].begin();
           v != new_mesh_data.cell_vertices[*q].end(); ++v)
        if (*v != vc_it->first)
          sendv.push_back(*v);
    }
  }

  MPI::all_to_all(mpi_comm, send_vertcells, recv_vertcells);

  const unsigned int num_cell_vertices
    = new_mesh_data.cell_vertices.shape()[1];

  // Collect up cells on common vertices

  // Reset map
  sh_vert_to_cell.clear();

  for (unsigned int i = 0; i != mpi_size; ++i)
  {
    const std::vector<std::size_t>& recv_i = recv_vertcells[i];
    for (auto q = recv_i.begin(); q != recv_i.end();
         q += num_cell_vertices + 1)
    {
      const std::size_t vertex_index = *(q + 1);
      std::vector<std::size_t> cell_set(1, i);
      cell_set.insert(cell_set.end(), q,
                      q + num_cell_vertices + 1);

      // Packing: [owner, cell_index, this_vertex, [other_vertices]]
      // Look for vertex in map, and add the attached cell
      auto it = sh_vert_to_cell.find(vertex_index);
      if (it == sh_vert_to_cell.end())
        sh_vert_to_cell.insert(std::make_pair(vertex_index, cell_set));
      else
        it->second.insert(it->second.end(), cell_set.begin(), cell_set.end());
    }
  }

  // Clear sending arrays
  send_vertcells = std::vector<std::vector<std::size_t>>(mpi_size);

  // Send back out to all processes which share the same vertex
  // FIXME: avoid sending back own cells to owner?
  for (auto p = sh_vert_to_cell.begin(); p != sh_vert_to_cell.end(); ++p)
  {
    for (auto q = p->second.begin(); q != p->second.end();
         q += (num_cell_vertices + 2))
      send_vertcells[*q].insert(send_vertcells[*q].end(),
                                p->second.begin(), p->second.end());
  }

  MPI::all_to_all(mpi_comm, send_vertcells, recv_vertcells);

  // Count up new cells, assign local index, set owner
  // and initialise shared_cells

  const unsigned int num_cells
    = new_mesh_data.cell_vertices.shape()[0];
  unsigned int count = num_cells;

  for (auto p = recv_vertcells.begin(); p != recv_vertcells.end(); ++p)
    for (auto q = p->begin(); q != p->end();
         q += num_cell_vertices + 2)
    {
      const std::size_t owner = *q;
      const std::size_t cell_index = *(q + 1);
      auto cell_it = cell_global_to_local.find(cell_index);
      if (cell_it == cell_global_to_local.end())
      {
        cell_global_to_local.insert(std::make_pair(cell_index, count));
        shared_cells.insert(std::make_pair(count, std::set<unsigned int>()));
        new_mesh_data.global_cell_indices.push_back(cell_index);
        new_mesh_data.cell_partition.push_back(owner);
        ++count;
      }
    }

  cell_vertices.resize(boost::extents[count][num_cell_vertices]);

  std::set<unsigned int> sharing_procs;
  std::vector<std::size_t> sharing_cells;

  std::size_t last_vertex = std::numeric_limits<std::size_t>::max();
  for (auto p = recv_vertcells.begin(); p != recv_vertcells.end(); ++p)
    for (auto q = p->begin(); q != p->end();
         q += num_cell_vertices + 2)
    {
      const std::size_t shared_vertex = *(q + 2);
      const std::size_t owner = *q;
      const std::size_t cell_index = *(q + 1);
      const std::size_t local_index
        = cell_global_to_local.find(cell_index)->second;

      // Add vertices to new cells
      if (local_index >= num_cells)
      {
        for (unsigned int j = 0; j != num_cell_vertices; ++j)
          cell_vertices[local_index][j] = *(q + j + 2);
      }

      // If starting on a new shared vertex, dump old data
      // into shared_cells
      if (shared_vertex != last_vertex)
      {
        last_vertex = shared_vertex;
        for (auto c = sharing_cells.begin(); c != sharing_cells.end(); ++c)
        {
          auto it = shared_cells.find(*c);
          if (it == shared_cells.end())
            shared_cells.insert(std::make_pair(*c, sharing_procs));
          else
            it->second.insert(sharing_procs.begin(), sharing_procs.end());
        }
        sharing_procs.clear();
        sharing_cells.clear();
      }

      // Don't include self in sharing processes
      if (owner != mpi_rank)
        sharing_procs.insert(owner);
      sharing_cells.push_back(local_index);
    }

  for (auto c = sharing_cells.begin(); c != sharing_cells.end(); ++c)
  {
    auto it = shared_cells.find(*c);
    if (it == shared_cells.end())
      shared_cells.insert(std::make_pair(*c, sharing_procs));
    else
      it->second.insert(sharing_procs.begin(), sharing_procs.end());
  }

}
//-----------------------------------------------------------------------------
unsigned int MeshPartitioning::distribute_cells(
  const MPI_Comm mpi_comm,
  const LocalMeshData& mesh_data,
  const std::vector<std::size_t>& cell_partition,
  const std::map<std::size_t, dolfin::Set<unsigned int>>& ghost_procs,
  std::map<unsigned int, std::set<unsigned int>>& shared_cells,
  LocalMeshData& new_mesh_data)
{
  // This function takes the partition computed by the partitioner
  // stored in cell_partition/ghost_procs
  // Some cells go to multiple destinations.
  // Each cell is transmitted to its final destination(s) including
  // its global index, and the cell owner (for ghost cells this
  // will be different from the destination)

  Timer timer("Distribute cells");

  const std::size_t mpi_size = MPI::size(mpi_comm);
  const std::size_t mpi_rank = MPI::rank(mpi_comm);

  // Get dimensions of local mesh_data
  const std::size_t num_local_cells = mesh_data.cell_vertices.size();
  dolfin_assert(mesh_data.global_cell_indices.size() == num_local_cells);
  const std::size_t num_cell_vertices = mesh_data.num_vertices_per_cell;
  if (!mesh_data.cell_vertices.empty())
  {
    if (mesh_data.cell_vertices[0].size() != num_cell_vertices)
    {
      dolfin_error("MeshPartitioning.cpp",
                   "distribute cells",
                   "Mismatch in number of cell vertices (%d != %d) on process %d",
                   mesh_data.cell_vertices[0].size(), num_cell_vertices,
                   mpi_rank);
    }
  }

  // Send all cells to their destinations including their global indices.
  // First element of vector is cell count of unghosted cells,
  // second element is count of ghost cells.
  std::vector<std::vector<std::size_t>>
    send_cell_vertices(mpi_size, std::vector<std::size_t>(2, 0) );

  for (unsigned int i = 0; i != cell_partition.size(); ++i)
  {
    // If cell is in ghost_procs map, use that to determine
    // destinations, otherwise just use the cell_partition
    // vector

    auto map_it = ghost_procs.find(i);
    if (map_it != ghost_procs.end())
    {
      const dolfin::Set<unsigned int>& destinations
        = map_it->second;

      for (auto dest = destinations.begin();
           dest != destinations.end(); ++dest)
      {
        // Create reference to destination vector
        std::vector<std::size_t>& send_cell_dest
          = send_cell_vertices[*dest];

        // Count of ghost cells, followed by ghost processes
        send_cell_dest.push_back(destinations.size());
        send_cell_dest.insert(send_cell_dest.end(),
                              destinations.begin(),
                              destinations.end());

        // Global cell index
        send_cell_dest.push_back(mesh_data.global_cell_indices[i]);
        // Global vertex indices
        send_cell_dest.insert(send_cell_dest.end(),
                              mesh_data.cell_vertices[i].begin(),
                              mesh_data.cell_vertices[i].end());
        // First entry is the owner, so this counts as a 'local' cell
        // subsequent entries are 'remote ghosts'
        if (dest == destinations.begin())
          send_cell_dest[0]++;
        else
          send_cell_dest[1]++;
      }
    }
    else
    {
      // Single destination (unghosted cell)
      std::vector<std::size_t>& send_cell_dest
        = send_cell_vertices[cell_partition[i]];
      send_cell_dest.push_back(0);
      // Global cell index
      send_cell_dest.push_back(mesh_data.global_cell_indices[i]);
      // Global vertex indices
      send_cell_dest.insert(send_cell_dest.end(),
                            mesh_data.cell_vertices[i].begin(),
                            mesh_data.cell_vertices[i].end());
      send_cell_dest[0]++;
    }
  }

  // Distribute cell-vertex connectivity and ownership information
  std::vector<std::vector<std::size_t>> received_cell_vertices(mpi_size);
  MPI::all_to_all(mpi_comm, send_cell_vertices, received_cell_vertices);

  // Count number of received cells (first entry in vector)
  // and find out how many ghost cells there are...
  std::size_t local_count = 0;
  std::size_t ghost_count = 0;
  for (std::size_t p = 0; p < mpi_size; ++p)
  {
    std::vector<std::size_t>& received_data = received_cell_vertices[p];
    local_count += received_data[0];
    ghost_count += received_data[1];
  }

  const std::size_t all_count = ghost_count + local_count;

  // Put received mesh data into new_mesh_data structure
  new_mesh_data.cell_vertices.resize(boost::extents[all_count]
                                     [num_cell_vertices]);
  new_mesh_data.global_cell_indices.resize(all_count);
  new_mesh_data.cell_partition.resize(all_count);

  // Unpack received data
  // Create a map from cells which are shared, to the remote processes
  // which share them - corral ghost cells to end of range
  std::size_t c = 0;
  std::size_t gc = local_count;
  for (std::size_t p = 0; p < mpi_size; ++p)
  {
    std::vector<std::size_t>& received_data = received_cell_vertices[p];
    for (auto it = received_data.begin() + 2;
         it != received_data.end();
         it += (*it + num_cell_vertices + 2))
    {
      auto tmp_it = it;
      const unsigned int num_ghosts = *tmp_it++;

      // Determine owner, and indexing.
      // Note that *tmp_it may be equal to mpi_rank
      const std::size_t owner = (num_ghosts == 0) ? mpi_rank : *tmp_it ;
      const std::size_t idx = (owner == mpi_rank) ? c : gc ;

      new_mesh_data.cell_partition[idx] = owner;
      if (num_ghosts != 0)
      {
        std::set<unsigned int> proc_set(tmp_it, tmp_it + num_ghosts);
        // Remove self from set of sharing processes
        proc_set.erase(mpi_rank);
        shared_cells.insert(std::make_pair(idx, proc_set));
        tmp_it += num_ghosts;
      }
      new_mesh_data.global_cell_indices[idx] = *tmp_it++;
      for (std::size_t j = 0; j < num_cell_vertices; ++j)
        new_mesh_data.cell_vertices[idx][j] = *tmp_it++;
      if (owner == mpi_rank)
        ++c;
      else
        ++gc;
    }
  }

  dolfin_assert(c == local_count);
  dolfin_assert(gc == all_count);

  return local_count;
}
//-----------------------------------------------------------------------------
std::size_t MeshPartitioning::compute_vertex_mapping(MPI_Comm mpi_comm,
                         unsigned int num_regular_cells,
                         LocalMeshData& new_mesh_data,
                         std::map<std::size_t, std::size_t>& vertex_global_to_local)
{
  vertex_global_to_local.clear();

  std::vector<std::size_t>& vertex_indices
    = new_mesh_data.vertex_indices;
  dolfin_assert(vertex_indices.size() == 0);

  const boost::multi_array<std::size_t, 2>& cell_vertices
    = new_mesh_data.cell_vertices;

  // Get set of unique vertices from cells and start constructing a
  // global_to_local map.  Ghost vertices will be at the end of the
  // range (v >= num_regular_vertices).
  std::size_t v = 0;
  std::size_t num_regular_vertices = 0;
  for (unsigned int i = 0; i != cell_vertices.size(); ++i)
  {
    for (auto q = cell_vertices[i].begin();
         q != cell_vertices[i].end(); ++q)
    {
      auto map_it = vertex_global_to_local.find(*q);
      if (map_it == vertex_global_to_local.end())
      {
        vertex_global_to_local.insert(std::make_pair(*q, v));
        vertex_indices.push_back(*q);
        ++v;
        if (i < num_regular_cells)
          num_regular_vertices = v;
      }
    }
  }

  return num_regular_vertices;
}
//-----------------------------------------------------------------------------
void MeshPartitioning::distribute_vertices(
  const MPI_Comm mpi_comm,
  const LocalMeshData& mesh_data,
  LocalMeshData& new_mesh_data,
  std::map<std::size_t, std::size_t>& vertex_global_to_local,
  std::map<unsigned int, std::set<unsigned int>>& shared_vertices_local)
{
  // This function distributes all vertices (coordinates and
  // local-to-global mapping) according to the cells that are stored
  // on each process. This happens in several stages: First each
  // process figures out which vertices it needs (by looking at its
  // cells) and where those vertices are located. That information is
  // then distributed so that each process learns where it needs to
  // send its vertices.

  Timer t("Distribute vertices");

  std::vector<std::size_t>& vertex_indices
    = new_mesh_data.vertex_indices;

  // Get number of processes
  const std::size_t mpi_size = MPI::size(mpi_comm);

  // Get geometric dimension
  const std::size_t gdim = mesh_data.gdim;

  // Compute where (process number) the vertices we need are located
  // using MPI::index_owner()
  std::vector<std::vector<std::size_t>> send_vertex_indices(mpi_size);
  for (auto required_vertex = vertex_indices.begin();
       required_vertex != vertex_indices.end(); ++required_vertex)
  {
    // Get process that has required vertex
    const std::size_t location = MPI::index_owner(mpi_comm, *required_vertex,
                                                  mesh_data.num_global_vertices);
    send_vertex_indices[location].push_back(*required_vertex);
  }

  // Convenience reference
  const std::vector<std::vector<std::size_t>>& vertex_location
    = send_vertex_indices;

  // Send required vertices to other processes, and receive back
  // vertices required by other processes.
  std::vector<std::vector<std::size_t>> received_vertex_indices;
  MPI::all_to_all(mpi_comm, send_vertex_indices, received_vertex_indices);

  // Redistribute received_vertex_indices as vertex sharing
  // information
  build_shared_vertices(mpi_comm, shared_vertices_local,
                        vertex_global_to_local, received_vertex_indices);

  // Distribute vertex coordinates
  std::vector<std::vector<double>> send_vertex_coordinates(mpi_size);
  const std::pair<std::size_t, std::size_t> local_vertex_range
    = MPI::local_range(mpi_comm, mesh_data.num_global_vertices);
  for (std::size_t p = 0; p < mpi_size; ++p)
  {
    send_vertex_coordinates[p].reserve(received_vertex_indices[p].size()*gdim);
    for (auto q = received_vertex_indices[p].begin();
         q != received_vertex_indices[p].end(); ++q)
    {
      dolfin_assert(*q >= local_vertex_range.first
                 && *q < local_vertex_range.second);

      const std::size_t location = *q - local_vertex_range.first;

      send_vertex_coordinates[p].insert(send_vertex_coordinates[p].end(),
                                        mesh_data.vertex_coordinates[location].begin(),
                                        mesh_data.vertex_coordinates[location].end());
    }
  }

  // Send actual coordinates to destinations
  std::vector<std::vector<double>> received_vertex_coordinates;
  MPI::all_to_all(mpi_comm, send_vertex_coordinates,
                  received_vertex_coordinates);

  // Count number of received local vertices and check it agrees with map
  std::size_t num_received_vertices = 0;
  for (std::size_t p = 0; p < mpi_size; ++p)
    num_received_vertices += received_vertex_coordinates[p].size()/gdim;
  dolfin_assert(num_received_vertices == vertex_indices.size());

  // Reference for convenience
  boost::multi_array<double, 2>& vertex_coordinates
    = new_mesh_data.vertex_coordinates;
  vertex_coordinates.resize(boost::extents[vertex_indices.size()][gdim]);

  // Store coordinates according to global_to_local mapping
  for (std::size_t p = 0; p < mpi_size; ++p)
  {
    for (std::size_t i = 0;
         i < received_vertex_coordinates[p].size()/gdim; ++i)
    {
      const std::size_t global_vertex_index
        = vertex_location[p][i];
      auto v = vertex_global_to_local.find(global_vertex_index);
      dolfin_assert(v != vertex_global_to_local.end());
      dolfin_assert(vertex_indices[v->second] == global_vertex_index);

      for (std::size_t j = 0; j < gdim; ++j)
        vertex_coordinates[v->second][j]
          = received_vertex_coordinates[p][i*gdim + j];
    }
  }
}
//-----------------------------------------------------------------------------
void MeshPartitioning::build_shared_vertices(MPI_Comm mpi_comm,
     std::map<unsigned int, std::set<unsigned int>>& shared_vertices_local,
     const std::map<std::size_t, std::size_t>& vertex_global_to_local,
     const std::vector<std::vector<std::size_t>>& received_vertex_indices)
{
  const std::size_t mpi_size = MPI::size(mpi_comm);

  // Generate vertex sharing information
  std::map<std::size_t, std::set<unsigned int>> vertex_to_proc;
  for (std::size_t p = 0; p < mpi_size; ++p)
  {
    for (auto q = received_vertex_indices[p].begin();
         q != received_vertex_indices[p].end(); ++q)
    {
      auto map_it = vertex_to_proc.find(*q);
      if (map_it == vertex_to_proc.end())
      {
        std::set<unsigned int> proc_set;
        proc_set.insert(p);
        vertex_to_proc.insert(std::make_pair(*q, proc_set));
      }
      else
        map_it->second.insert(p);
    }
  }

  std::vector<std::vector<std::size_t>> send_sharing(mpi_size);
  std::vector<std::vector<std::size_t>> recv_sharing(mpi_size);
  for (auto map_it = vertex_to_proc.begin();
       map_it != vertex_to_proc.end(); ++map_it)
  {
    if (map_it->second.size() != 1)
    {
      for (auto proc = map_it->second.begin();
           proc != map_it->second.end(); ++proc)
      {
        std::vector<std::size_t>& ss = send_sharing[*proc];
        ss.push_back(map_it->second.size() - 1);
        ss.push_back(map_it->first);
        for (auto p =  map_it->second.begin();
             p != map_it->second.end(); ++p)
          if (*p != *proc)
            ss.push_back(*p);
      }
    }
  }

  MPI::all_to_all(mpi_comm, send_sharing, recv_sharing);

  for (std::size_t p = 0; p < mpi_size; ++p)
  {
    for (auto q = recv_sharing[p].begin(); q != recv_sharing[p].end();
         q += (*q + 2))
    {
      const std::size_t num_sharing = *q;
      const std::size_t global_vertex_index = *(q + 1);
      std::set<unsigned int> sharing_processes(q + 2, q + 2 + num_sharing);

      auto local_index_it = vertex_global_to_local.find(global_vertex_index);
      dolfin_assert(local_index_it != vertex_global_to_local.end());
      const unsigned int local_index = local_index_it->second;
      dolfin_assert(shared_vertices_local.find(local_index)
                    == shared_vertices_local.end());
      shared_vertices_local.insert(std::make_pair(local_index,
                                                  sharing_processes));
    }
  }
}
//-----------------------------------------------------------------------------
void MeshPartitioning::build_mesh(Mesh& mesh,
  const std::map<std::size_t, std::size_t>& vertex_global_to_local,
  const LocalMeshData& new_mesh_data)
{
  Timer timer("PARALLEL 3: Build mesh (from local mesh data)");

  const std::vector<std::size_t>& global_cell_indices
    = new_mesh_data.global_cell_indices;
  const boost::multi_array<std::size_t, 2>& cell_global_vertices
    = new_mesh_data.cell_vertices;
  const std::vector<std::size_t>& vertex_indices
    = new_mesh_data.vertex_indices;
  const boost::multi_array<double, 2>& vertex_coordinates
    = new_mesh_data.vertex_coordinates;

  const unsigned int gdim = new_mesh_data.gdim;
  const unsigned int tdim = new_mesh_data.tdim;

  // Open mesh for editing
  mesh.clear();
  MeshEditor editor;
  editor.open(mesh, tdim, gdim);

  // Add vertices
  editor.init_vertices_global(vertex_coordinates.size(),
                              new_mesh_data.num_global_vertices);
  Point point(gdim);
  dolfin_assert(vertex_indices.size() == vertex_coordinates.size());
  for (std::size_t i = 0; i < vertex_coordinates.size(); ++i)
  {
    for (std::size_t j = 0; j < gdim; ++j)
      point[j] = vertex_coordinates[i][j];
    editor.add_vertex_global(i, vertex_indices[i], point);
  }

  // Add cells
  editor.init_cells_global(cell_global_vertices.size(),
                           new_mesh_data.num_global_cells);
  const std::size_t num_cell_vertices = tdim + 1;
  std::vector<std::size_t> cell(num_cell_vertices);
  for (std::size_t i = 0; i < cell_global_vertices.size(); ++i)
  {
    for (std::size_t j = 0; j < num_cell_vertices; ++j)
    {
      // Get local cell vertex
      auto iter = vertex_global_to_local.find(cell_global_vertices[i][j]);
      dolfin_assert(iter != vertex_global_to_local.end());
      cell[j] = iter->second;
    }
    editor.add_cell(i, global_cell_indices[i], cell);
  }

  // Close mesh: Note that this must be done after creating the global
  // vertex map or otherwise the ordering in mesh.close() will be wrong
  // (based on local numbers).
  editor.close();
}
//-----------------------------------------------------------------------------
void MeshPartitioning::build_mesh_domains(Mesh& mesh,
                                          const LocalMeshData& local_data)
{
  // Get topological dimension
  const std::size_t D = mesh.topology().dim();

  // Local domain data
  const std::map<std::size_t, std::vector<
                                std::pair<std::pair<std::size_t, std::size_t>,
                                          std::size_t>>>&
    domain_data = local_data.domain_data;

  // Check which dimensions we have (can't use empty() on domain_data
  // because other processes might have data)
  std::vector<std::size_t> dims;
  for (std::size_t d = 0; d < D + 1; ++d)
  {
    auto it = domain_data.find(d);
    const int have_dim = (it == domain_data.end()) ? 0 : 1;
    int dsum = dolfin::MPI::sum(mesh.mpi_comm(), have_dim);
    if (dsum > 0)
      dims.push_back(d);
  }

  // Return if no processes have domain data
  if (dims.empty())
    return;

  // Initialise mesh domains
  mesh.domains().init(D);

  // Loop over dimension and build mesh data
  for (std::size_t d : dims)
  {
    // Initialise mesh
    mesh.init(d);

    // Create empty MeshValueCollection
    MeshValueCollection<std::size_t> mvc(mesh, d);

    // Get domain data and build mesh value collection
    auto dim_data = domain_data.find(d);
    if (dim_data != domain_data.end())
    {
      const std::vector<std::pair<std::pair<std::size_t, std::size_t>,
                                  std::size_t>>& local_value_data
        = dim_data->second;

      // Build mesh value collection
      build_mesh_value_collection(mesh, local_value_data, mvc);
    }
    else
    {
      const std::vector<std::pair<std::pair<std::size_t, std::size_t>,
                                  std::size_t>> local_value_data;
      build_mesh_value_collection(mesh, local_value_data, mvc);
    }

    // Get data from mesh value collection
    const std::map<std::pair<std::size_t, std::size_t>, std::size_t>& values
      = mvc.values();

    // Get map from mesh domains
    std::map<std::size_t, std::size_t>& markers = mesh.domains().markers(d);

    std::map<std::pair<std::size_t, std::size_t>,
             std::size_t>::const_iterator it;
    for (it = values.begin(); it != values.end(); ++it)
    {
      const std::size_t cell_index = it->first.first;
      const std::size_t local_entity_index = it->first.second;

      if (d == D)
        markers[0] = it->second;
      else
      {
        const Cell cell(mesh, cell_index);
        const MeshEntity e(mesh, d, cell.entities(d)[local_entity_index]);
        markers[e.index()] = it->second;
      }
    }
  }
}
//-----------------------------------------------------------------------------

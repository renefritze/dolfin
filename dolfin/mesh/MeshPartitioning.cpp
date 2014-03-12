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
#include <dolfin/graph/ParMETIS.h>
#include <dolfin/graph/SCOTCH.h>
#include <dolfin/graph/ZoltanPartition.h>
#include <dolfin/parameter/GlobalParameters.h>
#include "BoundaryMesh.h"
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
   const std::vector<std::pair<std::pair<std::size_t, std::size_t>, std::size_t> >&
                                                            local_value_data,
   MeshValueCollection<std::size_t>& mesh_values);

template void MeshPartitioning::build_mesh_value_collection(const Mesh& mesh,
   const std::vector<std::pair<std::pair<std::size_t, std::size_t>, int> >&
                                                            local_value_data,
   MeshValueCollection<int>& mesh_values);

template void MeshPartitioning::build_mesh_value_collection(const Mesh& mesh,
   const std::vector<std::pair<std::pair<std::size_t, std::size_t>, double> >&
                                                            local_value_data,
   MeshValueCollection<double>& mesh_values);

template void MeshPartitioning::build_mesh_value_collection(const Mesh& mesh,
   const std::vector<std::pair<std::pair<std::size_t, std::size_t>, bool> >&
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
std::set<std::size_t> MeshPartitioning::cell_vertex_set(
    const boost::multi_array<std::size_t, 2>& cell_vertices)
{
  std::set<std::size_t> vertex_set;
  boost::multi_array<std::size_t, 2>::const_iterator vertices;
  for (vertices = cell_vertices.begin(); vertices != cell_vertices.end();
       ++vertices)
    vertex_set.insert(vertices->begin(), vertices->end());

  return vertex_set;
}
//-----------------------------------------------------------------------------
void MeshPartitioning::build_distributed_mesh(Mesh& mesh,
                                              const LocalMeshData& local_data)
{
  // Compute cell partitioning or use partitioning provided in local_data
  std::vector<std::size_t> cell_partition;
  std::map<std::size_t, dolfin::Set<unsigned int> > ghost_procs;
  if (local_data.cell_partition.empty())
    partition_cells(mesh.mpi_comm(), local_data, cell_partition, ghost_procs);
  else
  {
    cell_partition = local_data.cell_partition;
    dolfin_assert(cell_partition.size()
                  == local_data.global_cell_indices.size());
    dolfin_assert(*std::max_element(cell_partition.begin(), cell_partition.end())
                  < MPI::size(mesh.mpi_comm()));
  }

  // Build mesh from local mesh data and provided cell partition
  build(mesh, local_data, cell_partition, ghost_procs);

  // Create MeshDomains from local_data
  build_mesh_domains(mesh, local_data);

  // Initialise number of globally connected cells to each facet. This is
  // necessary to distinguish between facets on an exterior boundary and
  // facets on a partition boundary (see
  // https://bugs.launchpad.net/dolfin/+bug/733834).

  DistributedMeshTools::init_facet_cell_connections_by_ghost(mesh);
}
//-----------------------------------------------------------------------------
void MeshPartitioning::partition_cells(const MPI_Comm& mpi_comm,
      const LocalMeshData& mesh_data,
      std::vector<std::size_t>& cell_partition,
      std::map<std::size_t, dolfin::Set<unsigned int> >& ghost_procs)
{

  // Compute cell partition using partitioner from parameter system
  const std::string partitioner = parameters["mesh_partitioner"];
  if (partitioner == "SCOTCH")
    SCOTCH::compute_partition(mpi_comm, cell_partition, ghost_procs, mesh_data);
  else if (partitioner == "ParMETIS")
    ParMETIS::compute_partition(mpi_comm, cell_partition, ghost_procs, mesh_data);
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
     const std::map<std::size_t, dolfin::Set<unsigned int> >& ghost_procs)
{

  // Distribute cells
  Timer timer("PARALLEL 2: Distribute mesh (cells and vertices)");

  // Get all cells, ghost and regular onto correct processes
  std::vector<std::size_t> global_cell_indices;
  boost::multi_array<std::size_t, 2> cell_vertices;

  // Keep tabs on ghost cell ownership
  std::vector<std::size_t> ghost_remote_process;
  std::map<unsigned int, std::set<unsigned int> > shared_cells;
  distribute_ghost_cells(mesh.mpi_comm(), mesh_data,
                         cell_partition,
                         ghost_procs, global_cell_indices,
                         ghost_remote_process,
                         shared_cells,
                         cell_vertices);

  // Distribute vertices
  std::vector<std::size_t> vertex_indices;
  boost::multi_array<double, 2> vertex_coordinates;
  std::map<std::size_t, std::size_t> vertex_global_to_local;

  // Find set of locally required vertices
  const std::set<std::size_t> vertex_set = cell_vertex_set(cell_vertices);
  distribute_vertices(mesh.mpi_comm(), mesh_data, vertex_set, vertex_indices,
                      vertex_global_to_local, vertex_coordinates);

  timer.stop();

  // Build mesh
  build_mesh(mesh, global_cell_indices, cell_vertices, vertex_indices,
             vertex_coordinates, vertex_global_to_local,
             mesh_data.tdim, mesh_data.gdim, mesh_data.num_global_cells,
             mesh_data.num_global_vertices);

  // FIXME: this may later become a Mesh member variable
  mesh.data().create_array("ghost_owner", mesh_data.tdim);
  // Copy array over
  std::vector<std::size_t>& ghost_cell_owner = 
    mesh.data().array("ghost_owner", mesh_data.tdim);
  ghost_cell_owner.insert(ghost_cell_owner.begin(),
                          ghost_remote_process.begin(),
                          ghost_remote_process.end());

  // Assign map
  mesh.topology().shared_entities(mesh_data.tdim) = shared_cells;

}
//-----------------------------------------------------------------------------
void MeshPartitioning::ghost_build_shared_vertices(Mesh& mesh,
     const boost::multi_array<std::size_t, 2>& ghost_cell_vertices,
     const std::vector<std::size_t>& ghost_remote_process,
     const std::map<std::size_t, std::size_t>& vertex_global_to_local)
{

  // Map from global vertex index to sharing processes
  std::map<std::size_t, std::set<unsigned int> > shared_vertices_global;

  // Iterate through boundary ghost cell vertices to get sharing processes
  // using global indices
  for(std::size_t i = 0; i < ghost_cell_vertices.size(); ++i)
    for(std::size_t j=0; j < ghost_cell_vertices[i].size(); ++j)
    {
      const std::size_t vindex = ghost_cell_vertices[i][j];
      // Ensure it is on boundary by enforcing local existence
      if(vertex_global_to_local.find(vindex) != vertex_global_to_local.end())
        shared_vertices_global[vindex].insert(ghost_remote_process[i]);
    }

  // Multiple-shared vertices (i.e. shared by more than two processes)
  // may not have all process information on all processes, so this needs
  // to be transmitted to all possible owners.
  // FIXME: avoid a global broadcast here, to enhance scalability.
  // This can be done as follows:
  // Send all known shared_vertices_global map entries with
  // more than one entry to the MPI::index_owner of the global vertex
  // which then collates the ownership and reflects
  // the results back to the senders.

  std::vector<std::size_t> shared_ownership_list;

  const std::size_t num_processes = MPI::size(mesh.mpi_comm());
  const std::size_t process_number = MPI::rank(mesh.mpi_comm());

  for(std::map<std::size_t, std::set<unsigned int> >::iterator map_it
    = shared_vertices_global.begin(); map_it != shared_vertices_global.end();
      ++map_it)
  {
    if(map_it->second.size() > 1)
    {
      // Multiple-shared vertex on boundary. Need to broadcast to
      // make sure it is correctly found on all processes
      shared_ownership_list.push_back(map_it->first);
      shared_ownership_list.push_back(map_it->second.size());
      for(std::set<unsigned int>::const_iterator proc = map_it->second.begin();
          proc != map_it->second.end(); ++proc)
        shared_ownership_list.push_back(*proc);
    }
  }

  // Send same data to all processes
  std::vector<std::vector<std::size_t> > recv_shared_ownership(num_processes);
  MPI::all_gather(mesh.mpi_comm(), shared_ownership_list, recv_shared_ownership);

  // Unpack received data
  for (unsigned int i = 0; i < num_processes; ++i)
  {
    // Ignore data from self
    if(i == process_number)
      continue;

    const std::vector<std::size_t>& recv_data = recv_shared_ownership[i];
    std::vector<std::size_t>::const_iterator c = recv_data.begin();
    while(c != recv_data.end())
    {
      const std::size_t global_index = *c++;
      const unsigned int n_remotes = *c++;

      std::map<std::size_t, std::set<unsigned int> >::iterator find_it
        = shared_vertices_global.find(global_index);
      if (find_it != shared_vertices_global.end())
      {
        // Already know about this vertex, may need to add processes...
        // Add process from which the message came, as well as
        // those in the message
        find_it->second.insert(i);
        for (unsigned int j = 0; j < n_remotes; ++j)
        {
          const unsigned int remote = *c++;
          if (remote != process_number) // ignore self
            find_it->second.insert(remote);
        }
      }
      else
        c += n_remotes;
    }
  }

  // Finally convert map from global to local indexing in mesh
  std::map<unsigned int, std::set<unsigned int> >& shared_vertices
        = mesh.topology().shared_entities(0);
  shared_vertices.clear();

  for(std::map<std::size_t, std::set<unsigned int> >::iterator map_it
        = shared_vertices_global.begin(); map_it != shared_vertices_global.end();
      ++map_it)
  {
    // Get local index
    std::map<std::size_t, std::size_t>::const_iterator local_index;
    local_index = vertex_global_to_local.find(map_it->first);
    dolfin_assert(local_index != vertex_global_to_local.end());
    shared_vertices[local_index->second] = map_it->second;
  }
}
//-----------------------------------------------------------------------------
void MeshPartitioning::distribute_ghost_cells(const MPI_Comm mpi_comm,
      const LocalMeshData& mesh_data,
      const std::vector<std::size_t>& cell_partition,
      const std::map<std::size_t, dolfin::Set<unsigned int> >& ghost_procs,
      std::vector<std::size_t>& global_cell_indices,
      std::vector<std::size_t>& ghost_remote_process,
      std::map<unsigned int, std::set<unsigned int> >& shared_cells,
      boost::multi_array<std::size_t, 2>& cell_vertices)
{
  // This function takes the partition computed by the partitioner
  // stored in ghost_procs - some cells go to multiple destinations.
  // Each cell is transmitted to its final destination(s) including
  // its global index, and the cell owner (for ghost cells this
  // will be different from the destination)

  const std::size_t num_processes = MPI::size(mpi_comm);
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

  // Send all cells to their destinations
  // including their global indices. 
  // First element of vector is cell count.
  std::vector<std::vector<std::size_t> > send_cell_vertices(num_processes,
                               std::vector<std::size_t>(1, 0) );
  
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
        // Count of ghost cells, followed by ghost processes
        send_cell_vertices[*dest].push_back(destinations.size());
        for (std::size_t j = 0; j < destinations.size(); j++)
          send_cell_vertices[*dest].push_back(destinations[j]);

        // Global cell index
        send_cell_vertices[*dest].push_back
          (mesh_data.global_cell_indices[i]); 
        // Global vertex indices
        for (std::size_t j = 0; j < num_cell_vertices; j++)
          send_cell_vertices[*dest].push_back
            (mesh_data.cell_vertices[i][j]);
        send_cell_vertices[*dest][0]++;
      }
    }
    else
    {
      // Single destination (unghosted cell)
      const unsigned int dest = cell_partition[i];
      send_cell_vertices[dest].push_back(0);
      // Global cell index
      send_cell_vertices[dest].push_back
        (mesh_data.global_cell_indices[i]); 
      // Global vertex indices
      for (std::size_t j = 0; j < num_cell_vertices; j++)
        send_cell_vertices[dest].push_back
          (mesh_data.cell_vertices[i][j]);
      send_cell_vertices[dest][0]++;
    }
    
  }

  // Distribute cell-vertex connectivity
  std::vector<std::vector<std::size_t> > received_cell_vertices(num_processes);
  MPI::all_to_all(mpi_comm, send_cell_vertices, received_cell_vertices);

  // Count number of received cells
  std::size_t num_new_local_cells = 0;
  for (std::size_t p = 0; p < num_processes; ++p)
    num_new_local_cells += received_cell_vertices[p][0]; 

  // Put mesh_data back into mesh_data.cell_vertices
  cell_vertices.resize(boost::extents[num_new_local_cells]
                             [num_cell_vertices]);
  global_cell_indices.resize(num_new_local_cells);
  ghost_remote_process.resize(num_new_local_cells);

  // Loop over new cells
  std::size_t c = 0;
  for (std::size_t p = 0; p < num_processes; ++p)
  {
    std::vector<std::size_t>& received_data = received_cell_vertices[p];
    for (auto it = received_data.begin() + 1;
         it != received_data.end();
         it += (*it + num_cell_vertices + 2))
    {
      auto tmp_it = it;
      const unsigned int num_ghosts = *tmp_it++;
      
      if (num_ghosts == 0)
        ghost_remote_process[c] = mpi_rank;
      else
      {
        ghost_remote_process[c] = *tmp_it;
        std::set<unsigned int> proc_set(tmp_it, tmp_it + num_ghosts);
        // Remove self from set of sharing processes
        proc_set.erase(mpi_rank);
        shared_cells.insert(std::make_pair(c, proc_set));
      }

      tmp_it += num_ghosts;
      global_cell_indices[c] = *tmp_it++;

      for (std::size_t j = 0; j < num_cell_vertices; ++j)
        cell_vertices[c][j] = *tmp_it++;
      ++c;
    }
  }

}
//-----------------------------------------------------------------------------
void  MeshPartitioning::distribute_cells(const MPI_Comm mpi_comm,
                                         const LocalMeshData& mesh_data,
                            const std::vector<std::size_t>& cell_partition,
                            std::vector<std::size_t>& global_cell_indices,
                            boost::multi_array<std::size_t, 2>& cell_vertices)
{

  // Number of MPI processes
  const std::size_t num_processes = MPI::size(mpi_comm);

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
                   MPI::rank(mpi_comm));
    }
  }

  // Build array of cell-vertex connectivity and partition vector
  // Distribute the global cell number as well
  std::vector<std::vector<std::size_t> > send_cell_vertices(num_processes);
  for (std::size_t i = 0; i < num_local_cells; i++)
  {
    const std::size_t dest = cell_partition[i];
    send_cell_vertices[dest].push_back(mesh_data.global_cell_indices[i]);
    for (std::size_t j = 0; j < num_cell_vertices; j++)
      send_cell_vertices[dest].push_back(mesh_data.cell_vertices[i][j]);
  }

  // Distribute cell-vertex connectivity
  std::vector<std::vector<std::size_t> > received_cell_vertices(num_processes);
  MPI::all_to_all(mpi_comm, send_cell_vertices, received_cell_vertices);

  // Count number of received cells
  std::size_t num_new_local_cells = 0;
  for (std::size_t p = 0; p < received_cell_vertices.size(); ++p)
  {
    num_new_local_cells
      += received_cell_vertices[p].size()/(num_cell_vertices + 1);
  }

  // Put mesh_data back into mesh_data.cell_vertices
  cell_vertices.resize(boost::extents[num_new_local_cells][num_cell_vertices]);
  global_cell_indices.resize(num_new_local_cells);

  // Loop over new cells
  std::size_t c = 0;
  for (std::size_t p = 0; p < num_processes; ++p)
  {
    for (std::size_t i = 0; i < received_cell_vertices[p].size();
         i += (num_cell_vertices + 1))
    {
      global_cell_indices[c] = received_cell_vertices[p][i];
      for (std::size_t j = 0; j < num_cell_vertices; ++j)
        cell_vertices[c][j] = received_cell_vertices[p][i + 1 + j];

      ++c;
    }
  }
}
//-----------------------------------------------------------------------------
void MeshPartitioning::distribute_vertices(const MPI_Comm mpi_comm,
                    const LocalMeshData& mesh_data,
                    const std::set<std::size_t>& needed_vertex_indices,
                    std::vector<std::size_t>& vertex_indices,
                    std::map<std::size_t, std::size_t>& vertex_global_to_local,
                    boost::multi_array<double, 2>& vertex_coordinates)
{
  // This function distributes all vertices (coordinates and
  // local-to-global mapping) according to the cells that are stored on
  // each process. This happens in several stages: First each process
  // figures out which vertices it needs (by looking at its cells)
  // and where those vertices are located. That information is then
  // distributed so that each process learns where it needs to send
  // its vertices.

  // Get number of processes
  const std::size_t num_processes = MPI::size(mpi_comm);

  // Get geometric dimension
  const std::size_t gdim = mesh_data.gdim;

  // Compute where (process number) the vertices we need are located
  std::vector<std::vector<std::size_t> > send_vertex_indices(num_processes);
  //  std::vector<std::vector<std::size_t> > vertex_location(num_processes);
  std::set<std::size_t>::const_iterator required_vertex;
  for (required_vertex = needed_vertex_indices.begin();
       required_vertex != needed_vertex_indices.end(); ++required_vertex)
  {
    // Get process that has required vertex
    const std::size_t location = MPI::index_owner(mpi_comm, *required_vertex,
                                                mesh_data.num_global_vertices);
    send_vertex_indices[location].push_back(*required_vertex);
    //    vertex_location[location].push_back(*required_vertex);
  }

  const std::vector<std::vector<std::size_t> >& vertex_location = send_vertex_indices;

  // Send required vertices to other processes, and receive back vertices
  // required by other processes.
  std::vector<std::vector<std::size_t> > received_vertex_indices;
  MPI::all_to_all(mpi_comm, send_vertex_indices, received_vertex_indices);

  // Distribute vertex coordinates
  std::vector<std::vector<double> > send_vertex_coordinates(num_processes);
  const std::pair<std::size_t, std::size_t> local_vertex_range
    = MPI::local_range(mpi_comm, mesh_data.num_global_vertices);
  for (std::size_t p = 0; p < num_processes; ++p)
  {
    send_vertex_coordinates[p].reserve(received_vertex_indices[p].size()*gdim);
    for (std::size_t i = 0; i < received_vertex_indices[p].size(); ++i)
    {
      dolfin_assert(received_vertex_indices[p][i] >= local_vertex_range.first
                 && received_vertex_indices[p][i] < local_vertex_range.second);
      const std::size_t location
        = received_vertex_indices[p][i] - local_vertex_range.first;
      for (std::size_t j = 0; j < gdim; ++j)
        send_vertex_coordinates[p].push_back(mesh_data.vertex_coordinates[location][j]);
    }
  }
  std::vector<std::vector<double> > received_vertex_coordinates;
  MPI::all_to_all(mpi_comm, send_vertex_coordinates,
                  received_vertex_coordinates);

  // Count number of new local vertices
  std::size_t num_local_vertices = vertex_indices.size();
  std::size_t v = num_local_vertices;
  dolfin_assert(num_local_vertices == vertex_coordinates.size());
  for (std::size_t p = 0; p < num_processes; ++p)
    num_local_vertices += received_vertex_coordinates[p].size()/gdim;

  // Store coordinates and construct global to local mapping
  vertex_coordinates.resize(boost::extents[num_local_vertices][gdim]);
  vertex_indices.resize(num_local_vertices);

  for (std::size_t p = 0; p < num_processes; ++p)
  {
    for (std::size_t i = 0;
         i < received_vertex_coordinates[p].size()/gdim; ++i)
    {
      for (std::size_t j = 0; j < gdim; ++j)
        vertex_coordinates[v][j]
          = received_vertex_coordinates[p][i*gdim + j];

      const std::size_t global_vertex_index
        = vertex_location[p][i];
      vertex_global_to_local[global_vertex_index] = v;
      vertex_indices[v] = global_vertex_index;

      ++v;
    }
  }
}
//-----------------------------------------------------------------------------
void MeshPartitioning::build_mesh(Mesh& mesh,
              const std::vector<std::size_t>& global_cell_indices,
              const boost::multi_array<std::size_t, 2>& cell_global_vertices,
              const std::vector<std::size_t>& vertex_indices,
              const boost::multi_array<double, 2>& vertex_coordinates,
              const std::map<std::size_t, std::size_t>& vertex_global_to_local,
              std::size_t tdim, std::size_t gdim, std::size_t num_global_cells,
              std::size_t num_global_vertices)
{
  Timer timer("PARALLEL 3: Build mesh (from local mesh data)");

  // Open mesh for editing
  mesh.clear();
  MeshEditor editor;
  editor.open(mesh, tdim, gdim);

  // Add vertices
  editor.init_vertices_global(vertex_coordinates.size(), num_global_vertices);
  Point point(gdim);
  dolfin_assert(vertex_indices.size() == vertex_coordinates.size());
  for (std::size_t i = 0; i < vertex_coordinates.size(); ++i)
  {
    for (std::size_t j = 0; j < gdim; ++j)
      point[j] = vertex_coordinates[i][j];
    editor.add_vertex_global(i, vertex_indices[i], point);
  }

  // Add cells
  editor.init_cells_global(cell_global_vertices.size(), num_global_cells);
  const std::size_t num_cell_vertices = tdim + 1;
  std::vector<std::size_t> cell(num_cell_vertices);
  for (std::size_t i = 0; i < cell_global_vertices.size(); ++i)
  {
    for (std::size_t j = 0; j < num_cell_vertices; ++j)
    {
      // Get local cell vertex
      std::map<std::size_t, std::size_t>::const_iterator iter
          = vertex_global_to_local.find(cell_global_vertices[i][j]);
      dolfin_assert(iter != vertex_global_to_local.end());
      cell[j] = iter->second;
    }
    editor.add_cell(i, global_cell_indices[i], cell);
  }

  // Close mesh: Note that this must be done after creating the global
  // vertex map or otherwise the ordering in mesh.close() will be wrong
  // (based on local numbers).
  editor.close();

  // Set global number of cells and vertices
  // mesh.topology().init_global(0, num_global_vertices);
  //   mesh.topology().init_global(tdim,  num_global_cells);
}
//-----------------------------------------------------------------------------
std::vector<std::size_t> MeshPartitioning::boundary_vertices(Mesh& mesh,
                        const std::vector<std::size_t>& vertex_indices)
{
  // Construct boundary mesh
  BoundaryMesh bmesh(mesh, "exterior");

  const MeshFunction<std::size_t>& boundary_vertex_map = bmesh.entity_map(0);
  const std::size_t boundary_size = boundary_vertex_map.size();

  // Build sorted array of global boundary vertex indices (global
  // numbering)
  std::vector<std::size_t> global_vertex_list(boundary_size);
  for (std::size_t i = 0; i < boundary_size; ++i)
    global_vertex_list[i] = vertex_indices[boundary_vertex_map[i]];
  std::sort(global_vertex_list.begin(), global_vertex_list.end());

  return global_vertex_list;
}
//-----------------------------------------------------------------------------
void MeshPartitioning::build_shared_vertices(Mesh& mesh,
              const std::vector<std::size_t>& boundary_vertex_indices,
              const std::map<std::size_t, std::size_t>& vertex_global_to_local)
{
  // Get number of processes and process number
  const std::size_t num_processes = MPI::size(mesh.mpi_comm());
  const std::size_t process_number = MPI::rank(mesh.mpi_comm());

  // Create shared_vertices data structure: mapping from shared vertices
  // to list of neighboring processes
  std::map<unsigned int, std::set<unsigned int> >& shared_vertices
        = mesh.topology().shared_entities(0);
  shared_vertices.clear();

  // Send boundary vertex indices to all other processes
  std::vector<std::vector<std::size_t> > global_vertex_recv(num_processes);
  MPI::all_gather(mesh.mpi_comm(), boundary_vertex_indices, global_vertex_recv);

  // Build shared vertex to sharing processes map
  for (std::size_t i = 0; i < num_processes; ++i)
  {

    if (i == process_number)
      continue;

    // Compute intersection of global indices
    std::vector<std::size_t> intersection(
                          std::min(boundary_vertex_indices.size(),
                                   global_vertex_recv[i].size()));

    std::vector<std::size_t>::iterator intersection_end
      = std::set_intersection(boundary_vertex_indices.begin(),
                              boundary_vertex_indices.end(),
                              global_vertex_recv[i].begin(),
                              global_vertex_recv[i].end(),
                              intersection.begin());

    // Fill shared vertices information
    std::vector<std::size_t>::const_iterator global_index;
    for (global_index = intersection.begin(); global_index != intersection_end;
         ++global_index)
    {
      // Get local index
      std::map<std::size_t, std::size_t>::const_iterator local_index;
      local_index = vertex_global_to_local.find(*global_index);
      dolfin_assert(local_index != vertex_global_to_local.end());

      // Insert (local index, [proc])
      shared_vertices[local_index->second].insert(i);
    }
  }
}
//-----------------------------------------------------------------------------
void MeshPartitioning::build_mesh_domains(Mesh& mesh,
                                          const LocalMeshData& local_data)
{
  // Local domain data
  const std::map<std::size_t,  std::vector<
    std::pair<std::pair<std::size_t, std::size_t>, std::size_t> > >&
    domain_data = local_data.domain_data;

  if (domain_data.empty())
    return;

  // Initialise mesh domains
  const std::size_t D = mesh.topology().dim();
  mesh.domains().init(D);

  std::map<std::size_t, std::vector<
    std::pair<std::pair<std::size_t, std::size_t>,
              std::size_t> > >::const_iterator dim_data;
  for (dim_data = domain_data.begin(); dim_data != domain_data.end();
       ++dim_data)
  {
    // Get mesh value collection used for marking
    const std::size_t dim = dim_data->first;

    // Initialise mesh
    mesh.init(dim);

    // Create empty MeshValueCollection
    MeshValueCollection<std::size_t> mvc(mesh, dim);

    // Get domain data
    const std::vector<std::pair<std::pair<std::size_t, std::size_t>,
                                std::size_t> >& local_value_data
                                = dim_data->second;

    // Build mesh value collection
    build_mesh_value_collection(mesh, local_value_data, mvc);

    // Get data from mesh value collection
    const std::map<std::pair<std::size_t, std::size_t>, std::size_t>& values
      = mvc.values();

    // Get map from mesh domains
    std::map<std::size_t, std::size_t>& markers = mesh.domains().markers(dim);

    std::map<std::pair<std::size_t, std::size_t>,
             std::size_t>::const_iterator it;
    for (it = values.begin(); it != values.end(); ++it)
    {
      const std::size_t cell_index = it->first.first;
      const std::size_t local_entity_index = it->first.second;

      const Cell cell(mesh, cell_index);
      const MeshEntity e(mesh, dim, cell.entities(dim)[local_entity_index]);
      markers[e.index()] = it->second;
    }
  }
}
//-----------------------------------------------------------------------------

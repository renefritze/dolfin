// Copyright (C) 2008-2011 Niclas Jansson, Ola Skavhaug and Anders Logg
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
// Modified by Garth N. Wells 2010
// Modified by Chris Richardson 2013
//
// First added:  2010-02-10
// Last changed: 2014-01-09

#include <dolfin/common/Timer.h>
#include <dolfin/common/MPI.h>
#include <dolfin/log/log.h>
#include <dolfin/parameter/GlobalParameters.h>
#include <dolfin/mesh/LocalMeshData.h>
#include "ParMETIS.h"
#include "GraphBuilder.h"

#ifdef HAS_PARMETIS
#include <parmetis.h>
#endif

using namespace dolfin;

#ifdef HAS_PARMETIS

namespace dolfin
{
  // This class builds a ParMETIS dual graph

  class ParMETISDualGraph
  {
  public:

    // Constructor
    ParMETISDualGraph(MPI_Comm mpi_comm, const LocalMeshData& mesh_data);

    // Destructor
    ~ParMETISDualGraph();

    // ParMETIS data
    std::vector<idx_t> elmdist;
    std::vector<idx_t> eptr;
    std::vector<idx_t> eind;
    idx_t numflag;
    idx_t* xadj;
    idx_t* adjncy;

    // Number of partitions (one for each process)
    idx_t nparts;

    // Strange weight arrays needed by ParMETIS
    idx_t ncon;
    std::vector<real_t> tpwgts;
    std::vector<real_t> ubvec;

    // Prepare remaining arguments for ParMETIS
    idx_t* elmwgt;
    idx_t wgtflag;
    idx_t edgecut;

  };
}
//-----------------------------------------------------------------------------
void ParMETIS::compute_partition(
  const MPI_Comm mpi_comm,
  std::vector<std::size_t>& cell_partition,
  std::map<std::size_t, dolfin::Set<unsigned int>>& ghost_procs,
  const LocalMeshData& mesh_data,
  std::string mode)
{
  // Duplicate MPI communicator (ParMETIS does not take const
  // arguments, so duplicate communicator to be sure it isn't changed)
  MPI_Comm comm;
  MPI_Comm_dup(mpi_comm, &comm);

  // Build dual graph
  ParMETISDualGraph g(mpi_comm, mesh_data);

  dolfin_assert(g.eptr.size() - 1 == mesh_data.cell_vertices.size());

  // Partition graph
  if (mode == "partition")
    partition(comm, cell_partition, ghost_procs, g);
  else if (mode == "adaptive_repartition")
    adaptive_repartition(comm, cell_partition, g);
  else if (mode == "refine")
    refine(comm, cell_partition, g);
  else
  {
    dolfin_error("ParMETIS.cpp",
                 "compute mesh partitioning using ParMETIS",
                 "partition model %s is unknown. Must be \"partition\", \"adactive_partition\" or \"refine\"",
                 mode.c_str());
  }

  MPI_Comm_free(&comm);
}
//-----------------------------------------------------------------------------
void ParMETIS::partition(
  MPI_Comm mpi_comm,
  std::vector<std::size_t>& cell_partition,
  std::map<std::size_t, dolfin::Set<unsigned int>>& ghost_procs,
  ParMETISDualGraph& g)
{
  Timer timer1("PARALLEL 1b: Compute graph partition (calling ParMETIS)");

  // Options for ParMETIS
  idx_t options[3];
  options[0] = 1;
  options[1] = 0;
  options[2] = 15;

  // Check that data arrays are not empty
  dolfin_assert(!g.tpwgts.empty());
  dolfin_assert(!g.ubvec.empty());

  // Call ParMETIS to partition graph
  const std::size_t num_local_cells = g.eptr.size() - 1;
  std::vector<idx_t> part(num_local_cells);
  dolfin_assert(!part.empty());
  int err = ParMETIS_V3_PartKway(g.elmdist.data(), g.xadj, g.adjncy, g.elmwgt,
                                 NULL, &g.wgtflag, &g.numflag, &g.ncon,
                                 &g.nparts,
                                 g.tpwgts.data(), g.ubvec.data(), options,
                                 &g.edgecut, part.data(),
                                 &mpi_comm);
  dolfin_assert(err == METIS_OK);

  // Work out halo cells for current division of dual graph
  const unsigned int num_processes = MPI::size(mpi_comm);
  const unsigned int process_number = MPI::rank(mpi_comm);
  const idx_t elm_begin = g.elmdist[process_number];
  const idx_t elm_end = g.elmdist[process_number + 1];
  const unsigned int ncells = elm_end - elm_begin;

  std::map<idx_t, std::set<unsigned int>> halo_cell_to_remotes;
  // local indexing "i"
  for(unsigned int i = 0; i < ncells; i++)
  {
    for(idx_t j = g.xadj[i]; j != g.xadj[i + 1]; ++j)
    {
      const idx_t other_cell = g.adjncy[j];
      if (other_cell < elm_begin || other_cell >= elm_end)
      {
        const unsigned int remote = std::upper_bound(g.elmdist.begin(),
                                       g.elmdist.end() ,
                                       other_cell) - g.elmdist.begin() - 1;
        dolfin_assert(remote < num_processes);
        if (halo_cell_to_remotes.find(i) == halo_cell_to_remotes.end())
          halo_cell_to_remotes[i] = std::set<unsigned int>();
        halo_cell_to_remotes[i].insert(remote);
      }
    }
  }

  // Do halo exchange of cell partition data
  std::vector<std::vector<std::size_t>> send_cell_partition(num_processes);
  std::vector<std::vector<std::size_t>> recv_cell_partition(num_processes);
  for(std::map<idx_t, std::set<unsigned int>>::iterator hcell
        = halo_cell_to_remotes.begin(); hcell != halo_cell_to_remotes.end();
      ++hcell)
  {
    for(std::set<unsigned int>::iterator proc = hcell->second.begin();
         proc != hcell->second.end(); ++proc)
    {
      dolfin_assert(*proc < num_processes);
      // global cell number
      send_cell_partition[*proc].push_back(hcell->first + elm_begin);
      //partitioning
      send_cell_partition[*proc].push_back(part[hcell->first]);
    }
  }

  // Actual halo exchange
  MPI::all_to_all(mpi_comm, send_cell_partition, recv_cell_partition);

  // Construct a map from all currently foreign cells to their new
  // partition number
  std::map<std::size_t, unsigned int> cell_ownership;
  for (unsigned int i = 0; i < num_processes; ++i)
  {
    std::vector<std::size_t>& recv_data = recv_cell_partition[i];
    for (unsigned int j = 0; j != recv_data.size(); j += 2)
    {
      const std::size_t global_cell = recv_data[j];
      const unsigned int cell_owner = recv_data[j+1];
      cell_ownership[global_cell] = cell_owner;
    }
  }

  // Generate mapping for where new boundary cells need to be sent
  for(unsigned int i = 0; i < ncells; i++)
  {
    const std::size_t proc_this = part[i];
    for (idx_t j = g.xadj[i]; j != g.xadj[i + 1]; ++j)
    {
      const idx_t other_cell = g.adjncy[j];
      std::size_t proc_other;

      if (other_cell < elm_begin || other_cell >= elm_end)
      { // remote cell - should be in map
        const std::map<std::size_t, unsigned int>::const_iterator
          find_other_proc = cell_ownership.find(other_cell);
        dolfin_assert(find_other_proc != cell_ownership.end());
        proc_other = find_other_proc->second;
      }
      else
        proc_other = part[other_cell - elm_begin];

      if (proc_this != proc_other)
      {
        auto map_it = ghost_procs.find(i);
        if (map_it == ghost_procs.end())
        {
          dolfin::Set<unsigned int> sharing_processes;
          sharing_processes.insert(proc_this);
          sharing_processes.insert(proc_other);
          ghost_procs.insert(std::make_pair(i, sharing_processes));
        }
        else
          map_it->second.insert(proc_other);
      }
    }
  }

  // Copy cell partition data
  cell_partition = std::vector<std::size_t>(part.begin(), part.end());
}
//-----------------------------------------------------------------------------
void ParMETIS::adaptive_repartition(MPI_Comm mpi_comm,
                                    std::vector<std::size_t>& cell_partition,
                                    ParMETISDualGraph& g)
{
  Timer timer1("PARALLEL 1b: Compute graph partition (calling ParMETIS Adaptive Repartition)");

  // Options for ParMETIS
  idx_t options[4];
  options[0] = 1;
  options[1] = 0;
  options[2] = 15;
  options[3] = PARMETIS_PSR_UNCOUPLED;
  // For repartition, PARMETIS_PSR_COUPLED seems to suppress all
  // migration if already balanced.  Try PARMETIS_PSR_UNCOUPLED for
  // better edge cut.

  // Check that data arrays are not empty
  dolfin_assert(!g.tpwgts.empty());
  dolfin_assert(!g.ubvec.empty());

  // Call ParMETIS to partition graph
  const double itr = parameters["ParMETIS_repartitioning_weight"];
  real_t _itr = itr;
  std::vector<idx_t> part(g.eptr.size() - 1);
  std::vector<idx_t> vsize(part.size(), 1);
  dolfin_assert(!part.empty());
  int err = ParMETIS_V3_AdaptiveRepart(g.elmdist.data(), g.xadj, g.adjncy,
                                       g.elmwgt, NULL, vsize.data(), &g.wgtflag,
                                       &g.numflag, &g.ncon, &g.nparts,
                                       g.tpwgts.data(), g.ubvec.data(), &_itr,
                                       options, &g.edgecut, part.data(),
                                       &mpi_comm);
  dolfin_assert(err == METIS_OK);

  // Copy cell partition data
  cell_partition = std::vector<std::size_t>(part.begin(), part.end());
}
//-----------------------------------------------------------------------------
void ParMETIS::refine(MPI_Comm mpi_comm,
                      std::vector<std::size_t>& cell_partition,
                      ParMETISDualGraph& g)
{
  Timer timer1("PARALLEL 1b: Compute graph partition (calling ParMETIS Refine)");

  // Get some MPI data
  const std::size_t process_number = MPI::rank(mpi_comm);

  // Options for ParMETIS
  idx_t options[4];
  options[0] = 1;
  options[1] = 0;
  options[2] = 15;
  //options[3] = PARMETIS_PSR_UNCOUPLED;

  // For repartition, PARMETIS_PSR_COUPLED seems to suppress all
  // migration if already balanced.  Try PARMETIS_PSR_UNCOUPLED for
  // better edge cut.

  // Check that data arrays are not empty
  dolfin_assert(!g.tpwgts.empty());
  dolfin_assert(!g.ubvec.empty());

  // Partitioning array to be computed by ParMETIS. Prefill with
  // process_number.
  const std::size_t num_local_cells = g.eptr.size() - 1;
  std::vector<idx_t> part(num_local_cells, process_number);
  dolfin_assert(!part.empty());

  // Call ParMETIS to partition graph
  int err = ParMETIS_V3_RefineKway(g.elmdist.data(), g.xadj, g.adjncy, g.elmwgt,
                                   NULL, &g.wgtflag, &g.numflag, &g.ncon,
                                   &g.nparts,
                                   g.tpwgts.data(), g.ubvec.data(), options,
                                   &g.edgecut, part.data(), &mpi_comm);
  dolfin_assert(err == METIS_OK);

  // Copy cell partition data
  cell_partition = std::vector<std::size_t>(part.begin(), part.end());
}
//-----------------------------------------------------------------------------
ParMETISDualGraph::ParMETISDualGraph(MPI_Comm mpi_comm,
                                     const LocalMeshData& mesh_data)
{
  // Get number of processes and process number
  const std::size_t num_processes = MPI::size(mpi_comm);

  // Get dimensions of local mesh_data
  const std::size_t num_local_cells = mesh_data.cell_vertices.size();
  const std::size_t num_cell_vertices = mesh_data.num_vertices_per_cell;

  // Check that number of local graph nodes (cells) is > 0
  if (num_local_cells == 0)
  {
    dolfin_error("ParMETIS.cpp",
                 "compute mesh partitioning using ParMETIS",
                 "ParMETIS cannot be used if a process has no cells (graph nodes). Use SCOTCH to perform partitioning instead");
  }

  // Communicate number of cells between all processors
  std::vector<std::size_t> num_cells;
  MPI::all_gather(mpi_comm, num_local_cells, num_cells);

  // Build elmdist array with cell offsets for all processors
  elmdist.assign(num_processes + 1, 0);
  for (std::size_t i = 1; i < num_processes + 1; ++i)
    elmdist[i] = elmdist[i - 1] + num_cells[i - 1];

  eptr.resize(num_local_cells + 1);
  eind.assign(num_local_cells*num_cell_vertices, 0);
  for (std::size_t i = 0; i < num_local_cells; i++)
  {
    dolfin_assert(mesh_data.cell_vertices[i].size() == num_cell_vertices);
    eptr[i] = i*num_cell_vertices;
    for (std::size_t j = 0; j < num_cell_vertices; j++)
      eind[eptr[i] + j] = mesh_data.cell_vertices[i][j];
  }
  eptr[num_local_cells] = num_local_cells*num_cell_vertices;

  dolfin_assert(!eptr.empty());
  dolfin_assert(!eind.empty());

  // Number of nodes shared for dual graph (partition along facets)
  idx_t ncommonnodes = num_cell_vertices - 1;
  numflag = 0;
  xadj = 0;
  adjncy = 0;

  dolfin_assert(!eptr.empty());
  dolfin_assert(!eind.empty());

  // Could use GraphBuilder::compute_dual_graph() instead
  int err = ParMETIS_V3_Mesh2Dual(elmdist.data(), eptr.data(), eind.data(),
                                  &numflag, &ncommonnodes,
                                  &xadj, &adjncy,
                                  &mpi_comm);
  dolfin_assert(err == METIS_OK);


  // Number of partitions (one for each process)
  nparts = num_processes;

  // Strange weight arrays needed by ParMETIS
  ncon = 1;
  tpwgts.assign(ncon*nparts, 1.0/static_cast<real_t>(nparts));
  ubvec.assign(ncon, 1.05);

  // Prepare remaining arguments for ParMETIS
  elmwgt = NULL;
  wgtflag = 0;
  edgecut = 0;
}
//-----------------------------------------------------------------------------
ParMETISDualGraph::~ParMETISDualGraph()
{
  // Free metis data structures
  METIS_Free(xadj);
  METIS_Free(adjncy);
}
//-----------------------------------------------------------------------------
#else
void ParMETIS::compute_partition(
  const MPI_Comm mpi_comm,
  std::vector<std::size_t>& cell_partition,
  std::map<std::size_t, dolfin::Set<unsigned int>>& ghost_procs,
  const LocalMeshData& data,
  std::string mode)
{
  dolfin_error("ParMETIS.cpp",
               "compute mesh partitioning using ParMETIS",
               "DOLFIN has been configured without support for ParMETIS");
}
//-----------------------------------------------------------------------------
#endif

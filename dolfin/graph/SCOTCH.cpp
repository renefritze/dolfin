// Copyright (C) 2010-2013 Garth N. Wells, Anders Logg and Chris Richardson
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
// Modified by Anders Logg 2011
// Modified by Chris Richardson 2013
//
// First added:  2010-02-10
// Last changed: 2014-09-09

#include <algorithm>
#include <map>
#include <numeric>
#include <set>
#include <boost/lexical_cast.hpp>

#include <dolfin/common/Set.h>
#include <dolfin/common/Timer.h>
#include <dolfin/common/MPI.h>
#include <dolfin/mesh/LocalMeshData.h>
#include "GraphBuilder.h"
#include "SCOTCH.h"

#include <dolfin/common/dolfin_common.h>

#ifdef HAS_SCOTCH
extern "C"
{
#include <stdint.h>
#include <ptscotch.h>
}
#endif

using namespace dolfin;

#ifdef HAS_SCOTCH

//-----------------------------------------------------------------------------
void SCOTCH::compute_partition(
  const MPI_Comm mpi_comm,
  std::vector<std::size_t>& cell_partition,
  std::map<std::size_t, dolfin::Set<unsigned int> >& ghost_procs,
  const LocalMeshData& mesh_data)
{
  // Create data structures to hold graph
  std::vector<std::set<std::size_t> > local_graph;
  std::set<std::size_t> ghost_vertices;

  // Compute local dual graph
  GraphBuilder::compute_dual_graph(mpi_comm, mesh_data, local_graph,
                                   ghost_vertices);

  // Compute partitions
  const std::size_t num_global_vertices = mesh_data.num_global_cells;
  const std::vector<std::size_t>& global_cell_indices
    = mesh_data.global_cell_indices;
  partition(mpi_comm, local_graph, ghost_vertices, global_cell_indices,
            num_global_vertices, cell_partition, ghost_procs);

}
//-----------------------------------------------------------------------------
std::vector<int> SCOTCH::compute_gps(const Graph& graph,
                                     std::size_t num_passes)
{
  // Create strategy string for Gibbs-Poole-Stockmeyer ordering
  std::string strategy = "g{pass= "
    + boost::lexical_cast<std::string>(num_passes) + "}";

  return compute_reordering(graph, strategy);
}
//-----------------------------------------------------------------------------
std::vector<int> SCOTCH::compute_reordering(const Graph& graph,
                                            std::string scotch_strategy)
{
  std::vector<int> permutation, inverse_permutation;
  compute_reordering(graph, permutation, inverse_permutation, scotch_strategy);
  return permutation;
}
//-----------------------------------------------------------------------------
void SCOTCH::compute_reordering(const Graph& graph,
                                std::vector<int>& permutation,
                                std::vector<int>& inverse_permutation,
                                std::string scotch_strategy)
{
  Timer timer("SCOTCH graph ordering");

  // Number of local graph vertices (cells)
  const SCOTCH_Num vertnbr = graph.size();

  // Data structures for graph input to SCOTCH (add 1 for case that
  // graph size is zero)
  std::vector<SCOTCH_Num> verttab;
  verttab.reserve(vertnbr + 1);
  std::vector<SCOTCH_Num> edgetab;
  edgetab.reserve(20*vertnbr);

  // Build local graph input for SCOTCH
  // (number of local + ghost graph vertices (cells),
  // number of local edges + edges connecting to ghost vertices)
  SCOTCH_Num edgenbr = 0;
  verttab.push_back(0);
  Graph::const_iterator vertex;
  for (vertex = graph.begin(); vertex != graph.end(); ++vertex)
  {
    edgenbr += vertex->size();
    verttab.push_back(verttab.back() + vertex->size());
    edgetab.insert(edgetab.end(), vertex->begin(), vertex->end());
  }

  // Create SCOTCH graph
  SCOTCH_Graph scotch_graph;

  // C-style array indexing
  const SCOTCH_Num baseval = 0;

  // Create SCOTCH graph and initialise
  if (SCOTCH_graphInit(&scotch_graph) != 0)
  {
    dolfin_error("SCOTCH.cpp",
                 "re-order graph using SCOTCH",
                 "Error initializing SCOTCH graph");
  }

  // Build SCOTCH graph
  if (SCOTCH_graphBuild(&scotch_graph, baseval,
                        vertnbr, &verttab[0], &verttab[1], NULL, NULL,
                        edgenbr, &edgetab[0], NULL))
  {
    dolfin_error("SCOTCH.cpp",
                 "partition mesh using SCOTCH",
                 "Error building SCOTCH graph");
  }

  // Check graph data for consistency
  /*
  #ifdef DEBUG
  if (SCOTCH_graphCheck(&scotch_graph))
  {
    dolfin_error("SCOTCH.cpp",
                 "partition mesh using SCOTCH",
                 "Consistency error in SCOTCH graph");
  }
  #endif
  */

  // Re-ordering strategy
  SCOTCH_Strat strat;
  SCOTCH_stratInit(&strat);

  // Set SCOTCH strategy (if provided)
  //SCOTCH_stratGraphOrderBuild(&strat, SCOTCH_STRATQUALITY, 0, 0);
  //SCOTCH_stratGraphOrderBuild(&strat, SCOTCH_STRATSPEED, 0, 0);
  if (!scotch_strategy.empty())
    SCOTCH_stratGraphOrder(&strat, scotch_strategy.c_str());

  // Vector to hold permutation vectors
  std::vector<SCOTCH_Num> permutation_indices(vertnbr);
  std::vector<SCOTCH_Num> inverse_permutation_indices(vertnbr);

  // Reset SCOTCH random number generator to produce deterministic
  // partitions
  SCOTCH_randomReset();

  // Compute re-ordering
  if (SCOTCH_graphOrder(&scotch_graph, &strat, permutation_indices.data(),
                        inverse_permutation_indices.data(), NULL, NULL, NULL))
  {
    dolfin_error("SCOTCH.cpp",
                 "re-order graph using SCOTCH",
                 "Error during re-ordering");
  }

  // Clean up SCOTCH objects
  SCOTCH_graphExit(&scotch_graph);
  SCOTCH_stratExit(&strat);

  // Copy permutation vectors
  permutation.resize(vertnbr);
  inverse_permutation.resize(vertnbr);
  std::copy(permutation_indices.begin(), permutation_indices.end(),
            permutation.begin());
  std::copy(inverse_permutation_indices.begin(),
            inverse_permutation_indices.end(), inverse_permutation.begin());
}
//-----------------------------------------------------------------------------
void SCOTCH::partition(
  const MPI_Comm mpi_comm,
  const std::vector<std::set<std::size_t> >& local_graph,
  const std::set<std::size_t>& ghost_vertices,
  const std::vector<std::size_t>& global_cell_indices,
  const std::size_t num_global_vertices,
  std::vector<std::size_t>& cell_partition,
  std::map<std::size_t, dolfin::Set<unsigned int> >& ghost_procs)
{
  Timer timer("Partition graph (calling SCOTCH)");

  // C-style array indexing
  const SCOTCH_Num baseval = 0;

  // Number of processes
  const std::size_t num_processes = MPI::size(mpi_comm);

  // This process number
  const std::size_t proc_num = MPI::rank(mpi_comm);

  // Local data ---------------------------------

  // Number of local graph vertices (cells)
  const SCOTCH_Num vertlocnbr = local_graph.size();
  const std::size_t vertgstnbr = vertlocnbr + ghost_vertices.size();

  // Data structures for graph input to SCOTCH (add 1 for case that
  // local graph size is zero)
  std::vector<SCOTCH_Num> vertloctab;
  vertloctab.reserve(local_graph.size() + 1);
  std::vector<SCOTCH_Num> edgeloctab;

  // Build local graph input for SCOTCH
  // (number of local + ghost graph vertices (cells),
  // number of local edges + edges connecting to ghost vertices)
  SCOTCH_Num edgelocnbr = 0;
  vertloctab.push_back((SCOTCH_Num) 0);
  std::vector<std::set<std::size_t> >::const_iterator vertex;
  for(vertex = local_graph.begin(); vertex != local_graph.end(); ++vertex)
  {
    edgelocnbr += vertex->size();
    vertloctab.push_back(vertloctab.back() + vertex->size());
    edgeloctab.insert(edgeloctab.end(), vertex->begin(), vertex->end());
  }

  // Handle case that local graph size is zero
  if (edgeloctab.empty())
    edgeloctab.resize(1);

  // Global data ---------------------------------

  // Number of local vertices (cells) on each process
  std::vector<SCOTCH_Num> proccnttab;
  const SCOTCH_Num local_graph_size = local_graph.size();
  MPI::all_gather(mpi_comm, local_graph_size, proccnttab);

  // FIXME: explain this test
  // Array containing . . . . (some sanity checks)
  std::vector<std::size_t> procvrttab(num_processes + 1);
  for (std::size_t i = 0; i < num_processes; ++i)
  {
    procvrttab[i] = std::accumulate(proccnttab.begin(),
                                    proccnttab.begin() + i, (std::size_t) 0);
  }
  procvrttab[num_processes] = procvrttab[num_processes - 1]
    + proccnttab[num_processes - 1];

  // Sanity check
  for (std::size_t i = 1; i <= proc_num; ++i)
    dolfin_assert(procvrttab[i] >= (procvrttab[i - 1] + proccnttab[i - 1]));

  // Print graph data -------------------------------------
  /*
  {
    const SCOTCH_Num vertgstnbr = local_graph.size() + ghost_vertices.size();

    // Total  (global) number of vertices (cells) in the graph
    const SCOTCH_Num vertglbnbr = num_global_vertices;

    // Total (global) number of edges (cell-cell connections) in the graph
    const SCOTCH_Num edgeglbnbr = MPI::sum(mpi_comm, edgelocnbr);

    for (std::size_t proc = 0; proc < num_processes; ++proc)
    {
      // Print data for one process at a time
      if (proc == proc_num)
      {
        // Number of processes
        const SCOTCH_Num procglbnbr = num_processes;

        cout << "--------------------------------------------------" << endl;
        cout << "Num vertices (vertglbnbr)     : " << vertglbnbr << endl;
        cout << "Num edges (edgeglbnbr)        : " << edgeglbnbr << endl;
        cout << "Num of processes (procglbnbr) : " << procglbnbr << endl;
        cout << "Vert per processes (proccnttab) : " << endl;
        for (std::size_t i = 0; i < proccnttab.size(); ++i)
          cout << "  " << proccnttab[i];
        cout << endl;
        cout << "Offsets (procvrttab): " << endl;
        for (std::size_t i = 0; i < procvrttab.size(); ++i)
          cout << "  " << procvrttab[i];
        cout << endl;

        //------ Print local data
        cout << "(*) Num vertices (vertlocnbr)        : " << vertlocnbr << endl;
        cout << "(*) Num vert (inc ghost) (vertgstnbr): " << vertgstnbr << endl;
        cout << "(*) Num edges (edgelocnbr)           : " << edgelocnbr << endl;
        cout << "(*) Vertloctab: " << endl;
        for (std::size_t i = 0; i < vertloctab.size(); ++i)
          cout << "  " << vertloctab[i];
        cout << endl;
        cout << "edgeloctab: " << endl;
        for (std::size_t i = 0; i < edgeloctab.size(); ++i)
          cout << "  " << edgeloctab[i];
        cout << endl;
        cout << "--------------------------------------------------" << endl;
      }
      MPI::barrier(mpi_comm);
    }
    MPI::barrier(mpi_comm);
  }
  */
  // ------------------------------------------------------

  // Create SCOTCH graph and initialise
  SCOTCH_Dgraph dgrafdat;
  if (SCOTCH_dgraphInit(&dgrafdat, mpi_comm) != 0)
  {
    dolfin_error("SCOTCH.cpp",
                 "partition mesh using SCOTCH",
                 "Error initializing SCOTCH graph");
  }

  // Build SCOTCH distributed graph
  if (SCOTCH_dgraphBuild(&dgrafdat, baseval, vertlocnbr, vertlocnbr,
                              &vertloctab[0], NULL, NULL, NULL,
                              edgelocnbr, edgelocnbr,
                              &edgeloctab[0], NULL, NULL) )
  {
    dolfin_error("SCOTCH.cpp",
                 "partition mesh using SCOTCH",
                 "Error building SCOTCH graph");
  }

  // Check graph data for consistency
  #ifdef DEBUG
  if (SCOTCH_dgraphCheck(&dgrafdat))
  {
    dolfin_error("SCOTCH.cpp",
                 "partition mesh using SCOTCH",
                 "Consistency error in SCOTCH graph");
  }
  #endif

  // Number of partitions (set equal to number of processes)
  const SCOTCH_Num npart = num_processes;

  // Partitioning strategy
  SCOTCH_Strat strat;
  SCOTCH_stratInit(&strat);

  // Set strategy (SCOTCH uses very cryptic strings for this, and they
  // can change between versions)
  //std::string strategy = "b{sep=m{asc=b{bnd=q{strat=f},org=q{strat=f}},low=q{strat=m{type=h,vert=80,low=h{pass=10}f{bal=0.0005,move=80},asc=b{bnd=d{dif=1,rem=1,pass=40}f{bal=0.005,move=80},org=f{bal=0.005,move=80}}}|m{type=h,vert=80,low=h{pass=10}f{bal=0.0005,move=80},asc=b{bnd=d{dif=1,rem=1,pass=40}f{bal=0.005,move=80},org=f{bal=0.005,move=80}}}},seq=q{strat=m{type=h,vert=80,low=h{pass=10}f{bal=0.0005,move=80},asc=b{bnd=d{dif=1,rem=1,pass=40}f{bal=0.005,move=80},org=f{bal=0.005,move=80}}}|m{type=h,vert=80,low=h{pass=10}f{bal=0.0005,move=80},asc=b{bnd=d{dif=1,rem=1,pass=40}f{bal=0.005,move=80},org=f{bal=0.005,move=80}}}}},seq=b{job=t,map=t,poli=S,sep=m{type=h,vert=80,low=h{pass=10}f{bal=0.0005,move=80},asc=b{bnd=d{dif=1,rem=1,pass=40}f{bal=0.005,move=80},org=f{bal=0.005,move=80}}}|m{type=h,vert=80,low=h{pass=10}f{bal=0.0005,move=80},asc=b{bnd=d{dif=1,rem=1,pass=40}f{bal=0.005,move=80},org=f{bal=0.005,move=80}}}}}";
  //SCOTCH_stratDgraphMap (&strat, strategy.c_str());

  // Resize vector to hold cell partition indices with enough extra
  // space for ghost cell partition information too When there are no
  // nodes, vertgstnbr may be zero, and at least one dummy location
  // must be created.
  std::vector<SCOTCH_Num>
    _cell_partition(std::max((std::size_t) 1, vertgstnbr), 0);

  // Reset SCOTCH random number generator to produce deterministic
  // partitions
  SCOTCH_randomReset();

  // Partition graph
  if (SCOTCH_dgraphPart(&dgrafdat, npart, &strat, _cell_partition.data()))
  {
    dolfin_error("SCOTCH.cpp",
                 "partition mesh using SCOTCH",
                 "Error during partitioning");
  }

  // Exchange halo with cell_partition data for ghosts
  // FIXME: check MPI type compatibility with SCOTCH_Num. Getting this
  //        wrong will cause a SEGV
  // FIXME: is there a better way to do this?
  MPI_Datatype MPI_SCOTCH_Num;
  if (sizeof(SCOTCH_Num) == 4)
    MPI_SCOTCH_Num = MPI_INT;
  else if (sizeof(SCOTCH_Num)==8)
    MPI_SCOTCH_Num = MPI_LONG_LONG_INT;

  // Double check size is correct
  int tsize;
  MPI_Type_size(MPI_SCOTCH_Num, &tsize);
  dolfin_assert(tsize == sizeof(SCOTCH_Num));

  if (SCOTCH_dgraphHalo(&dgrafdat, (void *)_cell_partition.data(),
                        MPI_SCOTCH_Num))
  {
    dolfin_error("SCOTCH.cpp",
                 "partition mesh using SCOTCH",
                 "Error during halo exchange");
  }

  // Get SCOTCH's locally indexed graph
  SCOTCH_Num* edge_ghost_tab;
  SCOTCH_dgraphData(&dgrafdat,
                    NULL, NULL, NULL, NULL, NULL, NULL,
                    NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                    &edge_ghost_tab, NULL, (MPI_Comm *)&mpi_comm);

  // Iterate through SCOTCH's local compact graph to find partition
  // boundaries and save to map
  for(SCOTCH_Num i = 0; i < vertlocnbr; ++i)
  {
    const std::size_t proc_this =  _cell_partition[i];
    for(SCOTCH_Num j = vertloctab[i]; j < vertloctab[i + 1]; ++j)
    {
      const std::size_t proc_other = _cell_partition[edge_ghost_tab[j]];
      if(proc_this != proc_other)
      {
        auto map_it = ghost_procs.find(i);
        if (map_it == ghost_procs.end())
        {
          dolfin::Set<unsigned int> sharing_processes;
          // Owning process goes first into dolfin::Set
          // (unordered set) so will always be first.
          sharing_processes.insert(proc_this);
          sharing_processes.insert(proc_other);
          ghost_procs.insert(std::make_pair(i, sharing_processes));
        }
        else
          map_it->second.insert(proc_other);
      }
    }
  }

  // Clean up SCOTCH objects
  SCOTCH_dgraphExit(&dgrafdat);
  SCOTCH_stratExit(&strat);

  // Only copy the local nodes partition information. Ghost process
  // data is already in the ghost_procs map
  cell_partition.resize(vertlocnbr);
  std::copy(_cell_partition.begin(), _cell_partition.begin() + vertlocnbr,
            cell_partition.begin());
}
//-----------------------------------------------------------------------------
#else
//-----------------------------------------------------------------------------
void SCOTCH::compute_partition(
  const MPI_Comm mpi_comm,
  std::vector<std::size_t>& cell_partition,
  std::map<std::size_t, dolfin::Set<unsigned int> >& ghost_procs,
  const LocalMeshData& mesh_data)
{
  dolfin_error("SCOTCH.cpp",
               "partition mesh using SCOTCH",
               "DOLFIN has been configured without support for SCOTCH");
}
//-----------------------------------------------------------------------------
std::vector<int> SCOTCH::compute_gps(const Graph& graph,
                                     std::size_t num_passes)
{
  dolfin_error("SCOTCH.cpp",
               "re-order graph using SCOTCH",
               "DOLFIN has been configured without support for SCOTCH");
  return std::vector<int>();
}
//-----------------------------------------------------------------------------
std::vector<int>
SCOTCH::compute_reordering(const Graph& graph,
                           std::string scotch_strategy)
{
  dolfin_error("SCOTCH.cpp",
               "re-order graph using SCOTCH",
               "DOLFIN has been configured without support for SCOTCH");
  return std::vector<int>();
}
//-----------------------------------------------------------------------------
void SCOTCH::compute_reordering(const Graph& graph,
                                std::vector<int>& permutation,
                                std::vector<int>& inverse_permutation,
                                std::string scotch_strategy)

{
  dolfin_error("SCOTCH.cpp",
               "re-order graph using SCOTCH",
               "DOLFIN has been configured without support for SCOTCH");
}
//-----------------------------------------------------------------------------
void SCOTCH::partition(const MPI_Comm mpi_comm,
                       const std::vector<std::set<std::size_t> >& local_graph,
                       const std::set<std::size_t>& ghost_vertices,
                       const std::vector<std::size_t>& global_cell_indices,
                       const std::size_t num_global_vertices,
                       std::vector<std::size_t>& cell_partition,
                       std::map<std::size_t, dolfin::Set<unsigned int> >& ghost_procs)
{
  dolfin_error("SCOTCH.cpp",
               "partition mesh using SCOTCH",
               "DOLFIN has been configured without support for SCOTCH");
}
//-----------------------------------------------------------------------------

#endif

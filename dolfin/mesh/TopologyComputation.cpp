// Copyright (C) 2006-2014 Anders Logg and Garth N. Wells
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
// Modified by Garth N. Wells 2012.
//
// First added:  2006-06-02
// Last changed: 2014-07-02

#include <algorithm>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include <boost/multi_array.hpp>
#include <boost/unordered_map.hpp>
#include <boost/version.hpp>

#include <dolfin/common/Timer.h>
#include <dolfin/common/utils.h>
#include <dolfin/log/log.h>
#include "Cell.h"
#include "CellType.h"
#include "Mesh.h"
#include "MeshConnectivity.h"
#include "MeshEntityIterator.h"
#include "MeshTopology.h"
#include "TopologyComputation.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
std::size_t TopologyComputation::compute_entities(Mesh& mesh, std::size_t dim)
{
  log(TRACE, "Computing mesh entities of dimension %d", dim);

  // Check if entities have already been computed
  const MeshTopology& topology = mesh.topology();
  const MeshConnectivity& ce = topology(topology.dim(), dim);
  const MeshConnectivity& ev = topology(dim, 0);
  if (topology.size(dim) > 0)
  {
    // Make sure we really have the connectivity
    if ((ce.empty() && dim != topology.dim()) || (ev.empty() && dim != 0))
    {
      dolfin_error("TopologyComputation.cpp",
                   "compute topological entities",
                   "Entities of topological dimension %d exist but connectivity is missing", dim);
    }
    return topology.size(dim);
  }

  // Call specialised function to compute entities
  const CellType& cell_type = mesh.type();
  const std::int8_t num_entity_vertices = cell_type.num_vertices(dim);
  switch (num_entity_vertices)
  {
    case  1:
      return TopologyComputation::compute_entities_by_key_matching<1>(mesh, dim);
    case  2:
      return TopologyComputation::compute_entities_by_key_matching<2>(mesh, dim);
    case  3:
      return TopologyComputation::compute_entities_by_key_matching<3>(mesh, dim);
    case  4:
      return TopologyComputation::compute_entities_by_key_matching<4>(mesh, dim);
    default:
      dolfin_error("TopologyComputation.cpp",
                   "compute topological entities",
                   "Entities with %d vertices not supported",
                   num_entity_vertices);
       return 0;
   }
}
//-----------------------------------------------------------------------------
std::size_t TopologyComputation::compute_entities_old(Mesh& mesh, std::size_t dim)
{
  // Get mesh topology and connectivity
  MeshTopology& topology = mesh.topology();
  MeshConnectivity& ce = topology(topology.dim(), dim);
  MeshConnectivity& ev = topology(dim, 0);

  // Check if entities have already been computed
  if (topology.size(dim) > 0)
  {
    // Make sure we really have the connectivity
    if ((ce.empty() && dim != topology.dim()) || (ev.empty() && dim != 0))
    {
      dolfin_error("TopologyComputation.cpp",
                   "compute topological entities",
                   "Entities of topological dimension %d exist but connectivity is missing", dim);
    }
    return topology.size(dim);
  }

  // Make sure connectivity does not already exist
  if (!ce.empty() || !ev.empty())
  {
    dolfin_error("TopologyComputation.cpp",
                 "compute topological entities",
                 "Connectivity for topological dimension %d exists but entities are missing", dim);
  }

  // Optimisation for common case where facets lie between two cells
  bool erase_visited_facets = false;
  if (mesh.geometry().dim() == topology.dim() and dim == topology.dim() - 1)
    erase_visited_facets = true;

  // Start timer
  Timer timer("Compute entities dim = " + std::to_string(dim));

  // Get cell type
  const CellType& cell_type = mesh.type();

  // Initialize local array of entities
  const std::size_t m = cell_type.num_entities(dim);
  const std::size_t n = cell_type.num_vertices(dim);
  boost::multi_array<unsigned int, 2> e_vertices(boost::extents[m][n]);

  // List of entity e indices connected to cell
  std::vector<std::size_t> connectivity_ce;
  connectivity_ce.reserve(mesh.num_cells()*m);

  // List of vertex indices connected to entity e
  std::vector<boost::multi_array<unsigned int, 1>> connectivity_ev;

  boost::unordered_map<std::vector<unsigned int>, unsigned int>
    evertices_to_index;

  // Rehash/reserve map for efficiency
  const std::size_t max_elements
    = mesh.num_cells()*mesh.type().num_entities(dim)/2;
  #if BOOST_VERSION < 105000
  evertices_to_index.rehash(max_elements/evertices_to_index.max_load_factor()
                            + 1);
  #else
  evertices_to_index.reserve(max_elements);
  #endif

  unsigned int current_entity = 0;
  unsigned int num_regular_entities = 0;

  // Reserve space for vector of vertex indices for each entity
  std::vector<unsigned int> evec(n);

  // Loop over cells
  for (CellIterator c(mesh, "all"); !c.end(); ++c)
  {
    // Get vertices from cell
    const unsigned int* vertices = c->entities(0);
    dolfin_assert(vertices);

    // Create entities from vertices
    cell_type.create_entities(e_vertices, dim, vertices);

    // Iterate over the given list of entities
    for (auto const &entity : e_vertices)
    {
      // Sort entities (to use as map key)
      std::partial_sort_copy(entity.begin(), entity.end(),
                             evec.begin(), evec.end());

      // Insert into map
      auto it = evertices_to_index.insert({evec, current_entity});

      // Entity index
      std::size_t e_index = it.first->second;

      // Add entity index to cell - e connectivity
      connectivity_ce.push_back(e_index);

      // If new key was inserted, increment entity counter
      if (it.second)
      {
        // Add list of new entity vertices
        connectivity_ev.push_back(entity);

        // Increase counter
        ++current_entity;
        if (!c->is_ghost())
          num_regular_entities = current_entity;
      }
      else
      {
        if (erase_visited_facets) // reduce map size for efficiency
          evertices_to_index.erase(it.first);
      }
    }
  }

  // Initialise connectivity data structure
  topology.init(dim, connectivity_ev.size(), 0);

  // Initialise ghost entity offset
  topology.init_ghost(dim, num_regular_entities);

  // Copy connectivity data into static MeshTopology data structures
  std::size_t* connectivity_ce_ptr = connectivity_ce.data();
  ce.init(mesh.num_cells(), m);
  for (unsigned int i = 0; i != mesh.num_cells(); ++i)
  {
    ce.set(i, connectivity_ce_ptr);
    connectivity_ce_ptr += m;
  }

  ev.set(connectivity_ev);

  return current_entity;
}
//-----------------------------------------------------------------------------
void TopologyComputation::compute_connectivity(Mesh& mesh,
                                               std::size_t d0,
                                               std::size_t d1)
{
  // This is where all the logic takes place to find a strategy for
  // the connectivity computation. For any given pair (d0, d1), the
  // connectivity is computed by suitably combining the following
  // basic building blocks:
  //
  //   1. compute_entities():     d  - 0  from dim - 0
  //   2. compute_transpose():    d0 - d1 from d1 - d0
  //   3. compute_intersection(): d0 - d1 from d0 - d' - d1
  //   4. compute_from_map():     d0 - d1 from d1 - 0 and d0 - 0
  // Each of these functions assume a set of preconditions that we
  // need to satisfy.

  log(TRACE, "Requesting connectivity %d - %d.", d0, d1);

  // Get mesh topology and connectivity
  MeshTopology& topology = mesh.topology();
  MeshConnectivity& connectivity = topology(d0, d1);

  // Check if connectivity has already been computed
  if (!connectivity.empty())
    return;

  // Compute entities if they don't exist
  if (topology.size(d0) == 0)
    compute_entities(mesh, d0);
  if (topology.size(d1) == 0)
    compute_entities(mesh, d1);

  // Check is mesh has entities
  if (topology.size(d0) == 0 && topology.size(d1) == 0)
    return;

  // Check if connectivity still needs to be computed
  if (!connectivity.empty())
    return;

  // Start timer
  Timer timer("Compute connectivity " + std::to_string(d0) + "-"
              + std::to_string(d1));

  // Decide how to compute the connectivity
  if (d0 == d1)
  {
    std::vector<std::vector<std::size_t>>
      connectivity_dd(topology.size(d0), std::vector<std::size_t>(1));
    for (MeshEntityIterator e(mesh, d0, "all"); !e.end(); ++e)
      connectivity_dd[e->index()][0] = e->index();
    topology(d0, d0).set(connectivity_dd);
  }
  else if (d0 < d1)
  {
    // Compute connectivity d1 - d0 and take transpose
    compute_connectivity(mesh, d1, d0);
    compute_from_transpose(mesh, d0, d1);
  }
  else
  {
    // Compute by mapping vertices from a lower dimension entity
    // to those of a higher dimension entity
    compute_from_map(mesh, d0, d1);
  }
}
//--------------------------------------------------------------------------
template<int N>
std::int32_t TopologyComputation::compute_entities_by_key_matching(Mesh& mesh,
                                                                   int dim)
{
  // Get mesh topology and connectivity
  MeshTopology& topology = mesh.topology();
  MeshConnectivity& ce = topology(topology.dim(), dim);
  MeshConnectivity& ev = topology(dim, 0);

  // Check if entities have already been computed
  if (topology.size(dim) > 0)
  {
    // Make sure we really have the connectivity
    if ((ce.empty() && dim != (int) topology.dim()) || (ev.empty() && dim != 0))
    {
      dolfin_error("TopologyComputation.cpp",
                   "compute topological entities",
                   "Entities of topological dimension %d exist but connectivity is missing", dim);
    }
    return topology.size(dim);
  }

  // Make sure connectivity does not already exist
  if (!ce.empty() || !ev.empty())
  {
    dolfin_error("TopologyComputation.cpp",
                 "compute topological entities",
                 "Connectivity for topological dimension %d exists but entities are missing", dim);
  }

  // Start timer
  Timer timer("Compute entities dim = " + std::to_string(dim));

  // Get cell type
  const CellType& cell_type = mesh.type();

  // Initialize local array of entities
  const std::int8_t num_entities = cell_type.num_entities(dim);
  const int num_vertices = cell_type.num_vertices(dim);

  // Create map from cell vertices to entity vertices
  boost::multi_array<unsigned int, 2>
    e_vertices(boost::extents[num_entities][num_vertices]);
  const int num_vertices_per_cell = cell_type.num_vertices();
  std::vector<unsigned int> v(num_vertices_per_cell);
  std::iota(v.begin(), v.end(), 0);
  cell_type.create_entities(e_vertices, dim, v.data());

  dolfin_assert(N == num_vertices);

  // Create data structure to hold entities
  // ([vertices key], (cell_local_index, cell index))
  std::vector<std::pair<std::array<std::int32_t, N>,
    std::pair<std::int8_t, std::int32_t>>>
      keyed_entities(num_entities*mesh.num_cells());

  // Loop over cells to build list of keyed entities
  int entity_counter = 0;
  for (CellIterator c(mesh, "all"); !c.end(); ++c)
  {
    // Get vertices from cell
    const unsigned int* vertices = c->entities(0);
    dolfin_assert(vertices);


    // Iterate over entities of cell
    const int cell_index = c->index();
    for (std::int8_t i = 0; i < num_entities; ++i)
    {
      auto& entity_key = keyed_entities[entity_counter].first;
      for (std::int8_t j = 0; j < num_vertices; ++j)
        entity_key[j] = vertices[e_vertices[i][j]];

      // Sort entity vertices
      std::sort(entity_key.begin(), entity_key.end());

      // Attach (local index, cell index), making local_index negative if it is
      // not a ghost cell. This ensures that non-ghosts come before ghosts when
      // sorted. The index is corrected later.
      if (!c->is_ghost())
        keyed_entities[entity_counter].second = {-i - 1, cell_index};
      else
        keyed_entities[entity_counter].second = {i, cell_index};

      // Increment entity counter
      ++entity_counter;
    }
  }

  // Sort entities by key
  std::sort(keyed_entities.begin(), keyed_entities.end());

  // List of vertex indices connected to entity e
  std::vector<std::array<int, N>> connectivity_ev;
  std::vector<std::array<int, N>> connectivity_ev_ghost;

  // List of entity e indices connected to cell
  boost::multi_array<int, 2>
    connectivity_ce(boost::extents[mesh.num_cells()][num_entities]);

  // Marker for whether or not the most recently created entity is a ghost
  // entity
  bool ghost_entity = false;

  // Find duplicate keys
  if (keyed_entities.size() > 1)
  {
    const auto& e = keyed_entities[0].first;
    const auto& cell = keyed_entities[0].second;

    const std::int32_t cell_index = cell.second;
    const std::int8_t local_index = cell.first;

    if (local_index < 0)
    {
      // 'Create' new entity and mark that most recently created entity is not a
      // ghost

      std::int8_t index = -local_index - 1;
      dolfin_assert(index >= 0);
      dolfin_assert(index < (int) connectivity_ce[cell_index].size());
      connectivity_ce[cell_index][index] = connectivity_ev.size();

      connectivity_ev.push_back(e);
      ghost_entity = false;
    }
    else
    {
      // 'Create' new entity and mark that most recently created entity is a
      // ghost
      dolfin_assert(local_index < (int) connectivity_ce[cell_index].size());
      connectivity_ce[cell_index][local_index] = -connectivity_ev_ghost.size()  -1;
      connectivity_ev_ghost.push_back(e);
      ghost_entity = true;
    }
  }

  for (std::size_t i = 1; i < keyed_entities.size(); ++i)
  {
    const auto& e1 = keyed_entities[i].first;
    const auto& cell1 = keyed_entities[i].second;

    const std::int32_t cell_index = cell1.second;
    const std::int8_t local_index = cell1.first;

    // Compare entity with the preceding entity
    const auto& e0 = keyed_entities[i - 1].first;
    if (!std::equal(e1.begin(), e1.end(), e0.begin()))
    {
      if (local_index < 0)
      {
        // 'Create' new entity and flag that the most recent entity is not a
        // ghost
        connectivity_ev.push_back(e1);
        ghost_entity = false;
      }
      else
      {
        // 'Create' new entity and flag that the most recent entity is a ghost
        connectivity_ev_ghost.push_back(e1);
        ghost_entity = true;
      }
    }

    // Set entity index. Use negative index for ghost that will be corrected
    // later once the number of entities is known
    const std::int8_t _local_index = (local_index < 0) ? (-local_index - 1) : local_index;
    dolfin_assert(_local_index >= 0);
    if (!ghost_entity)
    {
      // Not a ghost entity
      dolfin_assert(_local_index < (int) connectivity_ce[cell_index].size());
      connectivity_ce[cell_index][_local_index] = connectivity_ev.size() - 1;
    }
    else
    {
      // Is a ghost entity, so use negative index
      dolfin_assert(connectivity_ev_ghost.size() > 0);
      connectivity_ce[cell_index][_local_index] = -connectivity_ev_ghost.size();
    }
  }

  // Initialise connectivity data structure
  topology.init(dim, connectivity_ev.size() + connectivity_ev_ghost.size(), 0);

  // Initialise ghost entity offset
  topology.init_ghost(dim, connectivity_ev.size());

  // Copy connectivity data into static MeshTopology data structures
  dolfin_assert(connectivity_ce.size() == mesh.num_cells());
  if (!connectivity_ev_ghost.empty())
  {
    // Re-map any ghost enties with the correct index
    for (unsigned int i = 0; i < mesh.num_cells(); ++i)
    {
      for (auto& x : connectivity_ce[i])
      {
        if (x < 0)
          x = (connectivity_ev.size() - x) - 1;
      }
    }
  }

  // Set cell-entity connectivity
  ce.set(connectivity_ce);

  // Add ghost entity-to-vertices connections to list of rectangular entities
  connectivity_ev.insert(connectivity_ev.end(), connectivity_ev_ghost.begin(),
                         connectivity_ev_ghost.end());
  ev.set(connectivity_ev);

  return connectivity_ev.size();
}
//-----------------------------------------------------------------------------
void TopologyComputation::compute_from_transpose(Mesh& mesh, std::size_t d0,
                                                 std::size_t d1)
{
  // The transpose is computed in three steps:
  //
  //   1. Iterate over entities of dimension d1 and count the number
  //      of connections for each entity of dimension d0
  //
  //   2. Allocate memory / prepare data structures
  //
  //   3. Iterate again over entities of dimension d1 and add connections
  //      for each entity of dimension d0

  log(TRACE, "Computing mesh connectivity %d - %d from transpose.", d0, d1);

  // Get mesh topology and connectivity
  MeshTopology& topology = mesh.topology();
  MeshConnectivity& connectivity = topology(d0, d1);

  // Need connectivity d1 - d0
  dolfin_assert(!topology(d1, d0).empty());

  // Temporary array
  std::vector<std::size_t> tmp(topology.size(d0), 0);

  // Count the number of connections
  for (MeshEntityIterator e1(mesh, d1, "all"); !e1.end(); ++e1)
    for (MeshEntityIterator e0(*e1, d0); !e0.end(); ++e0)
      tmp[e0->index()]++;

  // Initialize the number of connections
  connectivity.init(tmp);

  // Reset current position for each entity
  std::fill(tmp.begin(), tmp.end(), 0);

  // Add the connections
  for (MeshEntityIterator e1(mesh, d1, "all"); !e1.end(); ++e1)
    for (MeshEntityIterator e0(*e1, d0); !e0.end(); ++e0)
      connectivity.set(e0->index(), e1->index(), tmp[e0->index()]++);
}
//----------------------------------------------------------------------------
void TopologyComputation::compute_from_map(Mesh& mesh,
                                           std::size_t d0,
                                           std::size_t d1)
{
  dolfin_assert(d1 > 0);
  dolfin_assert(d0 > d1);

  // Get the type of entity d0
  std::unique_ptr<CellType> cell_type(CellType::create(mesh.type()
                                                       .entity_type(d0)));

  MeshConnectivity& connectivity = mesh.topology()(d0, d1);
  connectivity.init(mesh.size(d0), cell_type->num_entities(d1));

  // Make a map from the sorted d1 entity vertices to the d1 entity index
  boost::unordered_map<std::vector<unsigned int>, unsigned int>
    entity_to_index;
  entity_to_index.reserve(mesh.size(d1));

  const std::size_t num_verts_d1 = mesh.type().num_vertices(d1);
  std::vector<unsigned int> key(num_verts_d1);
  for (MeshEntityIterator e(mesh, d1, "all"); !e.end(); ++e)
  {
    std::partial_sort_copy(e->entities(0), e->entities(0) + num_verts_d1,
                           key.begin(), key.end());
    entity_to_index.insert({key, e->index()});
  }

  // Search for d1 entities of d0 in map, and recover index
  std::vector<std::size_t> entities;
  boost::multi_array<unsigned int, 2> keys;
  for (MeshEntityIterator e(mesh, d0, "all"); !e.end(); ++e)
  {
    entities.clear();
    cell_type->create_entities(keys, d1, e->entities(0));
    for (const auto &p : keys)
    {
      std::partial_sort_copy(p.begin(), p.end(), key.begin(), key.end());
      const auto it = entity_to_index.find(key);
      dolfin_assert(it != entity_to_index.end());
      entities.push_back(it->second);
    }
    connectivity.set(e->index(), entities.data());
  }

}
//-----------------------------------------------------------------------------
void TopologyComputation::compute_from_intersection(Mesh& mesh,
                                                    std::size_t d0,
                                                    std::size_t d1,
                                                    std::size_t d)
{
  log(TRACE, "Computing mesh connectivity %d - %d from intersection %d - %d - %d.",
      d0, d1, d0, d, d1);

  // Get mesh topology
  MeshTopology& topology = mesh.topology();

  // Check preconditions
  dolfin_assert(d0 >= d1);
  dolfin_assert(!topology(d0, d).empty());
  dolfin_assert(!topology(d, d1).empty());

  // Temporary dynamic storage, later copied into static storage
  std::vector<std::vector<std::size_t>> connectivity(topology.size(d0));

  // A bitmap used to ensure we do not store duplicates
  std::vector<bool> e1_visited(topology.size(d1));

  // Iterate over all entities of dimension d0
  std::size_t max_size = 1;
  const std::size_t e0_num_entities = mesh.type().num_vertices(d0);
  const std::size_t e1_num_entities = mesh.type().num_vertices(d1);
  std::vector<std::size_t> _e0(e0_num_entities);
  std::vector<std::size_t> _e1(e1_num_entities);
  for (MeshEntityIterator e0(mesh, d0, "all"); !e0.end(); ++e0)
  {
    // Get set of connected entities for current entity
    std::vector<std::size_t>& entities = connectivity[e0->index()];

    // Reserve space
    entities.reserve(max_size);

    // Sorted list of e0 vertex indices (necessary to test for
    // presence of one list in another)
    std::copy(e0->entities(0), e0->entities(0) + e0_num_entities, _e0.begin());
    std::sort(_e0.begin(), _e0.end());

    // Initialise e1_visited to false for all neighbours of e0. The
    // loop structure mirrors the one below.
    for (MeshEntityIterator e(*e0, d); !e.end(); ++e)
      for (MeshEntityIterator e1(*e, d1); !e1.end(); ++e1)
        e1_visited[e1->index()] = false;

    // Iterate over all connected entities of dimension d
    for (MeshEntityIterator e(*e0, d); !e.end(); ++e)
    {
      // Iterate over all connected entities of dimension d1
      for (MeshEntityIterator e1(*e, d1); !e1.end(); ++e1)
      {
        // Skip already visited connected entities (to avoid duplicates)
        if (e1_visited[e1->index()])
          continue;
        e1_visited[e1->index()] = true;

        if (d0 == d1)
        {
          // An entity is not a neighbor to itself (duplicate index
          // entries removed at end)
          if (e0->index() != e1->index())
            entities.push_back(e1->index());
        }
        else
        {
          // Sorted list of e1 vertex indices
          std::copy(e1->entities(0), e1->entities(0) + e1_num_entities,
                    _e1.begin());
          std::sort(_e1.begin(), _e1.end());

          // Entity e1 must be completely contained in e0
          if (std::includes(_e0.begin(), _e0.end(), _e1.begin(), _e1.end()))
            entities.push_back(e1->index());
        }
      }
    }

    // Store maximum size
    max_size = std::max(entities.size(), max_size);
  }

  // Copy to static storage
  topology(d0, d1).set(connectivity);
}
//-----------------------------------------------------------------------------

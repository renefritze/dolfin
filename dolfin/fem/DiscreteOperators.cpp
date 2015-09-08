// Copyright (C) 2015 Garth N. Wells
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

#include <vector>
#include <dolfin/common/ArrayView.h>
#include <dolfin/fem/GenericDofMap.h>
#include <dolfin/function/FunctionSpace.h>
#include <dolfin/la/GenericMatrix.h>
#include <dolfin/la/Matrix.h>
#include <dolfin/la/PETScMatrix.h>
#include <dolfin/la/TensorLayout.h>
#include <dolfin/mesh/Edge.h>
#include <dolfin/mesh/Mesh.h>
#include <dolfin/mesh/Vertex.h>
#include "DiscreteOperators.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
std::shared_ptr<GenericMatrix>
DiscreteOperators::build_gradient(const FunctionSpace& V0,
                                  const FunctionSpace& V1)
{
  // TODO: This function would be significantly simplified if it was
  // easier to build matrix sparsity patterns.

  // Get mesh
  dolfin_assert(V0.mesh());
  const Mesh& mesh = *(V0.mesh());

  // Check that mesh is the same for both function spaces
  dolfin_assert(V1.mesh());
  if (&mesh != V1.mesh().get())
  {
    dolfin_error("DiscreteGradient.cpp",
                 "compute discrete gradient operator",
                 "function spaces do not share the same mesh");
  }

  // Check that V0 is a (lowest-order) edge basis
  mesh.init(1);
  if (V0.dim() != mesh.size_global(1))
  {
    dolfin_error("DiscreteGradient.cpp",
                 "compute discrete gradient operator",
                 "function spaces is not a lowest-order edge space");
  }

  // Check that V1 is a linear nodal basis
  if (V1.dim() != mesh.size_global(0))
  {
    dolfin_error("DiscreteGradient.cpp",
                 "compute discrete gradient operator",
                 "function space is not a linear nodal function space");
  }

  // Build maps from entities to local dof indices
  const std::vector<dolfin::la_index> edge_to_dof = V0.dofmap()->dofs(mesh, 1);
  const std::vector<dolfin::la_index> vertex_to_dof
    = V1.dofmap()->dofs(mesh, 0);

  // Build maps from local dof numbering to global
  std::vector<std::size_t> local_to_global_map0;
  std::vector<std::size_t> local_to_global_map1;
  V0.dofmap()->tabulate_local_to_global_dofs(local_to_global_map0);
  V1.dofmap()->tabulate_local_to_global_dofs(local_to_global_map1);

  // Declare matrix
  auto A = std::make_shared<Matrix>();

  // Create layout for initialising tensor
  std::shared_ptr<TensorLayout> tensor_layout;
  tensor_layout = A->factory().create_layout(2);
  dolfin_assert(tensor_layout);

  //std::vector<std::size_t> block_sizes = {1, 1};
  std::vector<std::size_t> global_dimensions
    = {V0.dofmap()->global_dimension(), V1.dofmap()->global_dimension()};
  std::vector<std::pair<std::size_t, std::size_t>> local_range
    = { V0.dofmap()->ownership_range(), V1.dofmap()->ownership_range()};

  // Initialise tensor layout
  tensor_layout->init(mesh.mpi_comm(), global_dimensions, 1,
                      local_range);

  // Initialize edge -> vertex connections
  mesh.init(1, 0);

  // Build sparsity pattern
  if (tensor_layout->sparsity_pattern())
  {
    std::vector<std::vector<dolfin::la_index>>
      sparsity_entries(2);
    for (EdgeIterator edge(mesh); !edge.end(); ++edge)
    {
      // Row index (global indices)
      const std::size_t row = local_to_global_map0[edge_to_dof[edge->index()]];

      if (row >= local_range[0].first and row < local_range[0].second)
      {
        // Column indices (global indices)
        const Vertex v0(mesh, edge->entities(0)[0]);
        const Vertex v1(mesh, edge->entities(0)[1]);
        const int col0 = local_to_global_map1[vertex_to_dof[v0.index()]];
        const int col1 = local_to_global_map1[vertex_to_dof[v1.index()]];

        sparsity_entries[0].push_back(row);
        sparsity_entries[1].push_back(col0);

        sparsity_entries[0].push_back(row);
        sparsity_entries[1].push_back(col1);
      }
    }
    std::vector<ArrayView<const std::size_t>> local_to_global(2);
    std::vector<ArrayView<const int>> off_process_owner(2);
    std::vector<std::size_t> block_sizes = {1, 1};
    local_to_global[0].set(V0.dofmap()->local_to_global_unowned());
    local_to_global[1].set(V1.dofmap()->local_to_global_unowned());
    off_process_owner[0].set(V0.dofmap()->off_process_owner());
    off_process_owner[1].set(V1.dofmap()->off_process_owner());

    GenericSparsityPattern& pattern = *tensor_layout->sparsity_pattern();
    pattern.init(mesh.mpi_comm(), global_dimensions, local_range,
                 local_to_global,
                 off_process_owner, block_sizes);

    std::vector<ArrayView<const dolfin::la_index>> _sparsity_entries
      = {{ArrayView<const la_index>(sparsity_entries[0]),
          ArrayView<const la_index>(sparsity_entries[1])}};
    pattern.insert_global(_sparsity_entries);
    pattern.apply();
  }

  // Initialise matrix
  A->init(*tensor_layout);

  // Build discrete gradient operator/matrix
  for (EdgeIterator edge(mesh); !edge.end(); ++edge)
  {
    dolfin::la_index row;
    dolfin::la_index cols[2];
    double values[2];

    row = local_to_global_map0[edge_to_dof[edge->index()]];

    Vertex v0(mesh, edge->entities(0)[0]);
    Vertex v1(mesh, edge->entities(0)[1]);

    cols[0] = local_to_global_map1[vertex_to_dof[v0.index()]];
    cols[1] = local_to_global_map1[vertex_to_dof[v1.index()]];
    if (v1.global_index() < v0.global_index())
    {
      values[0] =  1.0;
      values[1] = -1.0;
    }
    else
    {
      values[0] = -1.0;
      values[1] =  1.0;
    }

    // Set values in matrix
    A->set(values, 1, &row, 2, cols);
  }

  // Finalise matrix
  A->apply("insert");

  return A;
}
//-----------------------------------------------------------------------------

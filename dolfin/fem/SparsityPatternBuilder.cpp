// Copyright (C) 2007-2010 Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Ola Skavhaug, 2007.
// Modified by Anders Logg, 2008-2009.
//
// First added:  2007-05-24
// Last changed: 2010-12-29

#include <dolfin/la/GenericSparsityPattern.h>
#include <dolfin/mesh/Cell.h>
#include <dolfin/mesh/Facet.h>
#include <dolfin/mesh/Mesh.h>
#include "GenericDofMap.h"
#include "SparsityPatternBuilder.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
void SparsityPatternBuilder::build(GenericSparsityPattern& sparsity_pattern,
                                   const Mesh& mesh,
                                   std::vector<const GenericDofMap*>& dof_maps,
                                   bool cells, bool interior_facets)
{
  const uint rank = dof_maps.size();

  // Get global dimensions
  std::vector<uint> global_dimensions(rank);
  for (uint i = 0; i < rank; ++i)
    global_dimensions[i] = dof_maps[i]->global_dimension();

  // Initialise sparsity pattern
  sparsity_pattern.init(global_dimensions);

  // Only build for rank >= 2 (matrices and higher order tensors)
  if (rank < 2)
    return;

  // Create vector to point to dofs
  std::vector<const std::vector<uint>* > dofs(rank);

  // Build sparsity pattern for cell integrals
  if (cells)
  {
    Progress p("Building sparsity pattern over cells", mesh.num_cells());
    for (CellIterator cell(mesh); !cell.end(); ++cell)
    {
      // Tabulate dofs for each dimension and get local dimensions
      for (uint i = 0; i < rank; ++i)
        dofs[i] = &dof_maps[i]->cell_dofs(cell->index());

      // Insert non-zeroes in sparsity pattern
      sparsity_pattern.insert(dofs);
      p++;
    }
  }

  // FIXME: The below note is not true when there are no cell integrals,
  //        e.g. finite volume method
  // Note: no need to iterate over exterior facets since those dofs
  // are included when tabulating dofs on all cells

  // Vector to store macro-dofs
  std::vector<std::vector<uint> > macro_dofs(rank);

  // Build sparsity pattern for interior facet integrals
  if (interior_facets)
  {
    // Compute facets and facet - cell connectivity if not already computed
    mesh.init(mesh.topology().dim() - 1);
    mesh.init(mesh.topology().dim() - 1, mesh.topology().dim());
    if (!mesh.ordered())
      error("Mesh has not been ordered. Cannot compute sparsity pattern. Consider calling Mesh::order().");

    Progress p("Building sparsity pattern over interior facets", mesh.num_facets());
    for (FacetIterator facet(mesh); !facet.end(); ++facet)
    {
      // Check if we have an interior facet
      if (facet->num_entities(mesh.topology().dim()) != 2)
      {
        p++;
        continue;
      }

      // Get cells incident with facet
      Cell cell0(mesh, facet->entities(mesh.topology().dim())[0]);
      Cell cell1(mesh, facet->entities(mesh.topology().dim())[1]);

      // Tabulate dofs for each dimension on macro element
      for (uint i = 0; i < rank; i++)
      {
        // Get dofs for each cell
        const std::vector<uint>& cell_dofs0 = dof_maps[i]->cell_dofs(cell0.index());
        const std::vector<uint>& cell_dofs1 = dof_maps[i]->cell_dofs(cell1.index());

        // Create space in macro dof vector
        macro_dofs[i].resize(cell_dofs0.size() + cell_dofs1.size());

        // Copy cell dofs into macro dof vector
        std::copy(cell_dofs0.begin(), cell_dofs0.end(), macro_dofs[i].begin());
        std::copy(cell_dofs1.begin(), cell_dofs1.end(), macro_dofs[i].begin() + cell_dofs0.size());

        // Store pointer to macro dofs
        dofs[i] = &macro_dofs[i];
      }

      // Vector to store macro-dofs
      sparsity_pattern.insert(dofs);

      p++;
    }
  }

  // Finalize sparsity pattern
  sparsity_pattern.apply();
}
//-----------------------------------------------------------------------------
